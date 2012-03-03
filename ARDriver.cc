// Copyright 2009 Isis Innovation Limited
#define GL_GLEXT_PROTOTYPES 1
#include "ARDriver.h"
#include "Map.h"
#include "Games.h"

#include <GL/GLAux.h>  //add by myself
#include <cvd/image_io.h>

namespace PTAMM {

using namespace GVars3;
using namespace CVD;
using namespace std;

static bool CheckFramebufferStatus();

/**
 * Constructor
 * @param cam Reference to the camera
 * @param irFrameSize the size of the frame
 * @param glw the glwindow
 * @param map the current map
 */
ARDriver::ARDriver(const ATANCamera &cam, ImageRef irFrameSize, GLWindow2 &glw, Map &map)
  :mCamera(cam), mGLWindow(glw), mpMap( &map )
{
  mirFrameSize = irFrameSize;
  mCamera.SetImageSize(mirFrameSize);
  mbInitialised = false;
  
  //mshaders[PROG_BUFF].SetShaderFile("shaders/buffer.vs","shaders/buffer.fs");
  mshaders[PROG_BUFF].SetShaderFile("shaders/phong.vs","shaders/phong.fs");
  mshaders[PROG_BUFF].UseShader(false);
  mshaders[PROG_LINE3D1].SetShaderFile("shaders/default.vs","shaders/line3D.fs");
  mshaders[PROG_LINE3D1].UseShader(false);
  mshaders[PROG_LINE3D2].SetShaderFile("shaders/default.vs","shaders/line3D.fs");
  mshaders[PROG_LINE3D2].UseShader(false);
  mshaders[PROG_TRANX].SetShaderFile("shaders/default.vs","shaders/edgex.fs");
  mshaders[PROG_TRANX].UseShader(false);
  mshaders[PROG_TRANX1].SetShaderFile("shaders/default.vs","shaders/edgex.fs");
  mshaders[PROG_TRANX1].UseShader(false);
  mshaders[PROG_TRANX2].SetShaderFile("shaders/default.vs","shaders/edgex.fs");
  mshaders[PROG_TRANX2].UseShader(false);
  mshaders[PROG_TRANY].SetShaderFile("shaders/default.vs","shaders/edgey.fs");
  mshaders[PROG_TRANY].UseShader(false);
  mshaders[PROG_EDGE].SetShaderFile("shaders/default.vs", "shaders/edge.fs");
  mshaders[PROG_EDGE].UseShader(false);
  mshaders[PROG_BLEND].SetShaderFile("shaders/default.vs","shaders/test.fs");
  mshaders[PROG_BLEND].UseShader(false);
  glGenTextures(16,mtex);
  glGenFramebuffers(3,mfbo);
}

/**
 * Initialize the AR driver
 */
void ARDriver::Init()
{
  mbInitialised = true;
  mirFBSize = GV3::get<ImageRef>("ARDriver.FrameBufferSize", ImageRef(640,480), SILENT);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mtex[TEX_IMAGE]);
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,
	       GL_RGBA, mirFrameSize.x, mirFrameSize.y, 0,
	       GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  MakeFrameBuffer();

  try {
    CVD::img_load(mLostOverlay, "ARData/Overlays/searching.png");
  }
  catch(CVD::Exceptions::All err) {
    cerr << "Failed to load searching image " << "\"ARData/Overlays/searching.png\"" << ": " << err.what << endl;
  }  
  
}


/**
 * Reset the game and the frame counter
 */
void ARDriver::Reset()
{
  if(mpMap->pGame) {
    mpMap->pGame->Reset();
  }

  mnCounter = 0;
}


/**
 * Render the AR composite image
 * @param imFrame The camera frame
 * @param se3CfromW The camera position
 * @param bLost Is the camera lost
 */
void ARDriver::Render(Image<Rgb<CVD::byte> > &imFrame, SE3<> se3CfromW, bool bLost, int statusFlag)
{
  if(!mbInitialised)
  {
    Init();
    Reset();
  };

  mse3CfromW = se3CfromW;
  mnCounter++;

  // Upload the image to our frame texture
  int tempheight,tempwidth;
  tempheight = imFrame.size().y;
  tempwidth = imFrame.size().x;
  //convert coordinate
  unsigned char* tempdata = (unsigned char*)malloc(sizeof(CVD::Rgb<CVD::byte>)*tempwidth*tempheight);
  for (int j=0; j<tempheight; j++)
  {
	  memcpy(tempdata+tempwidth*sizeof(CVD::Rgb<CVD::byte>)*(tempheight-1-j),(CVD::byte*)imFrame.data()+tempwidth*sizeof(CVD::Rgb<CVD::byte>)*j,tempwidth*sizeof(CVD::Rgb<CVD::byte>));
  }
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
  glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB,
		  0, 0, 0,
		  mirFrameSize.x, mirFrameSize.y,
		  GL_RGB,
		  GL_UNSIGNED_BYTE,
		  tempdata);
  free(tempdata);

  // Set up rendering to go the FBO, draw undistorted video frame into BG
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat af[4]; 
  af[0]=0.5; af[1]=0.5; af[2]=0.5; af[3]=1.0;
  glLightfv(GL_LIGHT0, GL_AMBIENT, af);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, af);
  af[0]=0.0; af[1]=10.0; af[2]=0.0; af[3]=1.0;
  glLightfv(GL_LIGHT0, GL_POSITION, af);

  //FBO_RND shader
  GLenum buffers1[2] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
  if(!bLost)
  {
    if(mpMap->pGame) {
	  mpMap->pGame->Draw3D( mGLWindow, *mpMap, se3CfromW, mfbo[FBO_RND], buffers1, mCamera, statusFlag);
    }
  }

  ////ATTACHMENT2---TEX_BUFF3  ATTACHMENT3---TEX_TRANX
  //GLenum buffers2[2] = { GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT};
  ////only draw 3d stuff if not lost.
  //if(!bLost)
  //{
	 // if(mpMap->pGame) {
		//  mpMap->pGame->Draw3D( mGLWindow, *mpMap, se3CfromW, mfbo[FBO_RND], buffers2, mCamera);
	 // }
  //}
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  //inside tran
  //for (int i=0; i<3; ++i)
  //{
	 // glViewport(0,0,mirFBSize.x,mirFBSize.y);
	 // glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
	 // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 // glMatrixMode(GL_PROJECTION);
	 // glLoadIdentity();
	 // glOrtho(0,1,1,0,0,1);
	 // glMatrixMode(GL_MODELVIEW);
	 // glLoadIdentity();
	 // if (i==0)
	 // {
		//  mshaders[PROG_TRANX].UseShader(true);
		//  glEnable(GL_TEXTURE_RECTANGLE_ARB);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF1]);
		//  mshaders[PROG_TRANX].SetSampler("txBuff1",0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF3]);
		//  mshaders[PROG_TRANX].SetSampler("txBuff3",1);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
		//  mshaders[PROG_TRANX].SetSampler("txImage",2);
		//  glEnable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS1]);
		//  mshaders[PROG_TRANX].SetSampler("txGauss1",3);		  
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS2]);
		//  mshaders[PROG_TRANX].SetSampler("txGauss2",4);
		//  //mshaders[PROG_TRANX].SetUniVar("iPass", i);
		//  mshaders[PROG_TRANX].SetUniVar("iInside", 1);
		//  DrawQuad();
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glDisable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glDisable(GL_TEXTURE_RECTANGLE_ARB);
		//  mshaders[PROG_TRANX].UseShader(false);
	 // }
	 // else
	 // {
		//  mshaders[PROG_TRANX1].UseShader(true);
		//  glEnable(GL_TEXTURE_RECTANGLE_ARB);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRAN1]);
		//  mshaders[PROG_TRANX1].SetSampler("txBuff1",0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF3]);
		//  mshaders[PROG_TRANX1].SetSampler("txBuff3",1);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
		//  mshaders[PROG_TRANX1].SetSampler("txImage",2);
		//  glEnable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS1]);
		//  mshaders[PROG_TRANX1].SetSampler("txGauss1",3);
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS2]);
		//  mshaders[PROG_TRANX1].SetSampler("txGauss2",4);
		//  //mshaders[PROG_TRANX1].SetUniVar("iPass", i);
		//  mshaders[PROG_TRANX1].SetUniVar("iInside", 1);
		//  DrawQuad();
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glDisable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glDisable(GL_TEXTURE_RECTANGLE_ARB);
		//  mshaders[PROG_TRANX1].UseShader(false);
	 // }

	 // //GL_COLOR_ATTACHMENT4_EXT TEX_TRAN1
	 // glViewport(0,0,mirFBSize.x,mirFBSize.y);
	 // glDrawBuffer(GL_COLOR_ATTACHMENT4_EXT);
	 // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 // glMatrixMode(GL_PROJECTION);
	 // glLoadIdentity();
	 // glOrtho(0,1,1,0,0,1);
	 // glMatrixMode(GL_MODELVIEW);
	 // glLoadIdentity();
	 // mshaders[PROG_TRANY].UseShader(true);
	 // glEnable(GL_TEXTURE_RECTANGLE_ARB);
	 // glActiveTexture(GL_TEXTURE0);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRANX]);
	 // mshaders[PROG_TRANY].SetSampler("txTranX",0);
	 // glActiveTexture(GL_TEXTURE1);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
	 // mshaders[PROG_TRANY].SetSampler("txImage",1);
	 // glEnable(GL_TEXTURE_2D);
	 // glActiveTexture(GL_TEXTURE2);
	 // glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS1]);
	 // mshaders[PROG_TRANY].SetSampler("txGauss1",2);		  
	 // glActiveTexture(GL_TEXTURE3);
	 // glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS2]);
	 // mshaders[PROG_TRANY].SetSampler("txGauss2",3);
	 // mshaders[PROG_TRANY].SetUniVar("iInside", 1);
	 // DrawQuad();
	 // glActiveTexture(GL_TEXTURE3);
	 // glBindTexture(GL_TEXTURE_2D, 0);
	 // glActiveTexture(GL_TEXTURE2);
	 // glBindTexture(GL_TEXTURE_2D, 0);
	 // glDisable(GL_TEXTURE_2D);
	 // glActiveTexture(GL_TEXTURE1);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	 // glActiveTexture(GL_TEXTURE0);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	 // glDisable(GL_TEXTURE_RECTANGLE_ARB);
	 // mshaders[PROG_TRANY].UseShader(false);
  //}

  ////outside tran
  //for (int i=0; i<3; ++i)
  //{
	 // glViewport(0,0,mirFBSize.x,mirFBSize.y);
	 // glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
	 // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 // glMatrixMode(GL_PROJECTION);
	 // glLoadIdentity();
	 // glOrtho(0,1,1,0,0,1);
	 // glMatrixMode(GL_MODELVIEW);
	 // glLoadIdentity();
	 // if (i==0)
	 // {
		//  mshaders[PROG_TRANX].UseShader(true);
		//  glEnable(GL_TEXTURE_RECTANGLE_ARB);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF1]);
		//  mshaders[PROG_TRANX].SetSampler("txBuff1",0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF3]);
		//  mshaders[PROG_TRANX].SetSampler("txBuff3",1);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
		//  mshaders[PROG_TRANX].SetSampler("txImage",2);
		//  glEnable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS1]);
		//  mshaders[PROG_TRANX].SetSampler("txGauss1",3);		  
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS2]);
		//  mshaders[PROG_TRANX].SetSampler("txGauss2",4);
		//  //mshaders[PROG_TRANX].SetUniVar("iPass", i);
		//  mshaders[PROG_TRANX].SetUniVar("iInside", 0);
		//  DrawQuad();
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glDisable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glDisable(GL_TEXTURE_RECTANGLE_ARB);
		//  mshaders[PROG_TRANX].UseShader(false);
	 // }
	 // else
	 // {
		//  mshaders[PROG_TRANX2].UseShader(true);
		//  glEnable(GL_TEXTURE_RECTANGLE_ARB);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRAN1]);
		//  mshaders[PROG_TRANX2].SetSampler("txBuff1",0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF3]);
		//  mshaders[PROG_TRANX2].SetSampler("txBuff3",1);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
		//  mshaders[PROG_TRANX2].SetSampler("txImage",2);
		//  glEnable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS1]);
		//  mshaders[PROG_TRANX2].SetSampler("txGauss1",3);
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS2]);
		//  mshaders[PROG_TRANX2].SetSampler("txGauss2",4);
		//  //mshaders[PROG_TRANX2].SetUniVar("iPass", i);
		//  mshaders[PROG_TRANX2].SetUniVar("iInside", 0);
		//  DrawQuad();
		//  glActiveTexture(GL_TEXTURE4);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glActiveTexture(GL_TEXTURE3);
		//  glBindTexture(GL_TEXTURE_2D, 0);
		//  glDisable(GL_TEXTURE_2D);
		//  glActiveTexture(GL_TEXTURE2);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE1);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glActiveTexture(GL_TEXTURE0);
		//  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		//  glDisable(GL_TEXTURE_RECTANGLE_ARB);
		//  mshaders[PROG_TRANX2].UseShader(false);
	 // }

	 // //GL_COLOR_ATTACHMENT5_EXT TEX_TRAN1
	 // glViewport(0,0,mirFBSize.x,mirFBSize.y);
	 // glDrawBuffer(GL_COLOR_ATTACHMENT5_EXT);
	 // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 // glMatrixMode(GL_PROJECTION);
	 // glLoadIdentity();
	 // glOrtho(0,1,1,0,0,1);
	 // glMatrixMode(GL_MODELVIEW);
	 // glLoadIdentity();
	 // mshaders[PROG_TRANY].UseShader(true);
	 // glEnable(GL_TEXTURE_RECTANGLE_ARB);
	 // glActiveTexture(GL_TEXTURE0);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRANX]);
	 // mshaders[PROG_TRANY].SetSampler("txTranX",0);
	 // glActiveTexture(GL_TEXTURE1);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
	 // mshaders[PROG_TRANY].SetSampler("txImage",1);
	 // glEnable(GL_TEXTURE_2D);
	 // glActiveTexture(GL_TEXTURE2);
	 // glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS1]);
	 // mshaders[PROG_TRANY].SetSampler("txGauss1",2);		  
	 // glActiveTexture(GL_TEXTURE3);
	 // glBindTexture(GL_TEXTURE_2D, mtex[TEX_GAUSS2]);
	 // mshaders[PROG_TRANY].SetSampler("txGauss2",3);
	 // mshaders[PROG_TRANY].SetUniVar("iInside", 0);
	 // DrawQuad();
	 // glActiveTexture(GL_TEXTURE3);
	 // glBindTexture(GL_TEXTURE_2D, 0);
	 // glActiveTexture(GL_TEXTURE2);
	 // glBindTexture(GL_TEXTURE_2D, 0);
	 // glDisable(GL_TEXTURE_2D);
	 // glActiveTexture(GL_TEXTURE1);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	 // glActiveTexture(GL_TEXTURE0);
	 // glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	 // glDisable(GL_TEXTURE_RECTANGLE_ARB);
	 // mshaders[PROG_TRANY].UseShader(false);
  //}

  ////ATTACHMENT6 TEX_LINE3D1
  //glViewport(0,0,mirFBSize.x,mirFBSize.y);
  //glDrawBuffer(GL_COLOR_ATTACHMENT6_EXT);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  //glOrtho(0,1,1,0,0,1);
  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  //mshaders[PROG_LINE3D1].UseShader(true);
  //glEnable(GL_TEXTURE_RECTANGLE_ARB);
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF2]);
  //mshaders[PROG_LINE3D1].SetSampler("txBuff",0);
  //DrawQuad();
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  //glDisable(GL_TEXTURE_RECTANGLE_ARB);
  //mshaders[PROG_LINE3D1].UseShader(false);
  ////ATTACHMENT7 TEX_LINE3D2  不能用GL_COLOR_ATTACHMENT8_EXT，不然会出现“无效列举”的错误
  //glViewport(0,0,mirFBSize.x,mirFBSize.y);
  //glDrawBuffer(GL_COLOR_ATTACHMENT7_EXT);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  //glOrtho(0,1,1,0,0,1);
  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  //mshaders[PROG_LINE3D2].UseShader(true);
  //glEnable(GL_TEXTURE_RECTANGLE_ARB);
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRANX]);
  //mshaders[PROG_LINE3D2].SetSampler("txBuff",0);
  //DrawQuad();
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  //glDisable(GL_TEXTURE_RECTANGLE_ARB);
  //mshaders[PROG_LINE3D2].UseShader(false);


  //FBO_BLD
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mfbo[FBO_BLD]);
  CheckFramebufferStatus();

  //BACK EDGE
  glViewport(0,0,mirFBSize.x,mirFBSize.y);
  glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,1,1,0,0,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  mshaders[PROG_EDGE].UseShader(true);
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
  mshaders[PROG_EDGE].SetSampler("txImage",0);
  DrawQuad();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  mshaders[PROG_EDGE].UseShader(false);

  //blend
  glViewport(0,0,mirFBSize.x,mirFBSize.y);
  glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,1,1,0,0,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  mshaders[PROG_BLEND].UseShader(true);
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF1]);
  mshaders[PROG_BLEND].SetSampler("txBuff1",0);
  //glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF3]);
  //mshaders[PROG_BLEND].SetSampler("txBuff3",1);
  //glActiveTexture(GL_TEXTURE2);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_LINE3D1]);
  //mshaders[PROG_BLEND].SetSampler("txLine3D1",2);
  //glActiveTexture(GL_TEXTURE3);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_LINE3D2]);
  //mshaders[PROG_BLEND].SetSampler("txLine3D2",3);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_IMAGE]);
  mshaders[PROG_BLEND].SetSampler("txImage",4);
  //glActiveTexture(GL_TEXTURE5);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRAN1]);
  //mshaders[PROG_BLEND].SetSampler("txTRAN1",5);
  //glActiveTexture(GL_TEXTURE6);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRAN2]);
  //mshaders[PROG_BLEND].SetSampler("txTRAN2",6);
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BACKEDGE]);
  mshaders[PROG_BLEND].SetSampler("txBackEdge",7);
  DrawQuad();
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  //glActiveTexture(GL_TEXTURE6);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  //glActiveTexture(GL_TEXTURE5);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  //glActiveTexture(GL_TEXTURE3);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  //glActiveTexture(GL_TEXTURE2);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  //glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  mshaders[PROG_BLEND].UseShader(false);

  //Draw mnFrameBufferTex texture to Windows
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_BLEND);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set up for drawing 2D stuff:
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

  DrawDistortedFB();
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  mGLWindow.SetupViewport();
  mGLWindow.SetupVideoOrtho();
  mGLWindow.SetupVideoRasterPosAndZoom();

  
  //2d drawing
  if(!bLost)
  {   
    if(mpMap->pGame) {
      mpMap->pGame->Draw2D(mGLWindow, *mpMap);
    }
  }
  else
  {
    //draw the lost ar overlays
    glEnable(GL_BLEND);
    glRasterPos2i( ( mGLWindow.size().x - mLostOverlay.size().x )/2,
                   ( mGLWindow.size().y - mLostOverlay.size().y )/2 );
    glDrawPixels(mLostOverlay);
    glDisable(GL_BLEND);
  }

}

void ARDriver::DrawQuad()
{
	static bool bFirstRun = true;
	static GLuint nList;
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_BLEND);
	if(bFirstRun)
	{
		bFirstRun = false;
		nList = glGenLists(1);
		glNewList(nList, GL_COMPILE_AND_EXECUTE);
		// How many grid divisions in the x and y directions to use?
		int nStepsX = 24; // Pretty arbitrary..
		int nStepsY = (int) (nStepsX * ((double) mirFrameSize.x / mirFrameSize.y)); // Scaled by aspect ratio
		if(nStepsY < 2)
			nStepsY = 2;
		glColor3f(1,1,1);
		for(int ystep = 0; ystep<nStepsY; ystep++)
		{
			glBegin(GL_QUAD_STRIP);
			for(int xstep = 0; xstep<=nStepsX; xstep++)
				for(int yystep = ystep; yystep<=ystep + 1; yystep++) // Two y-coords in one go - magic.
				{
					Vector<2> v2Iter;
					v2Iter[0] = (double) xstep / nStepsX;
					v2Iter[1] = (double) yystep / nStepsY;
					Vector<2> v2UFBDistorted = v2Iter;
					Vector<2> v2UFBUnDistorted = mCamera.UFBLinearProject(mCamera.UFBUnProject(v2UFBDistorted));
					glTexCoord2d(v2UFBUnDistorted[0] * mirFBSize.x, (1.0-v2UFBUnDistorted[1]) * mirFBSize.y);
					glVertex(v2UFBDistorted);
				}
				glEnd();
		}
		glEndList();
	}
	else
		glCallList(nList);
}

void ARDriver::SetTexture(GLuint texId)
{
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,texId);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,
		GL_RGBA, mirFBSize.x, mirFBSize.y, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void ARDriver::InitGaussTexture(GLuint texId)
{
	glBindTexture(GL_TEXTURE_2D,texId);
	float fGaussKernel[]={0.01, 0.05, 0.09, 0.12, 0.15, 0.16, 0.15, 0.12, 0.09, 0.05, 0.01};
	float *fGaussTexData = new float[11*4];
	for (int i=0; i<11; ++i)
	{
		for (int j=0; j<4; ++j)
		{
			fGaussTexData[4*i+j] = fGaussKernel[i];
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0,
		GL_RGBA, 11, 1, 0,
		GL_RGBA, GL_FLOAT, fGaussTexData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	delete [] fGaussTexData;
}

/**
 * Make the frame buffer
 */
void ARDriver::MakeFrameBuffer()
{
  // Needs nvidia drivers >= 97.46
  cout << "  ARDriver: Creating FBO... ";

  //texture stuff
  glGenTextures(1, &mnFrameBufferTex);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mnFrameBufferTex);
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,
	       GL_RGBA, mirFBSize.x, mirFBSize.y, 0,
	       GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  SetTexture(mtex[TEX_BUFF1]);
  SetTexture(mtex[TEX_BUFF2]);
  SetTexture(mtex[TEX_BUFF3]);
  SetTexture(mtex[TEX_TRANX]);
  SetTexture(mtex[TEX_TRAN1]);  
  SetTexture(mtex[TEX_TRAN2]); 
  SetTexture(mtex[TEX_LINE3D1]); //o edge
  SetTexture(mtex[TEX_LINE3D2]); //o edge
  SetTexture(mtex[TEX_MEDIATOR]);
  SetTexture(mtex[TEX_BACKEDGE]);
  //TEX_GAUSS
  InitGaussTexture(mtex[TEX_GAUSS1]);
  InitGaussTexture(mtex[TEX_GAUSS2]);
  //TEX_XTOON
  glBindTexture(GL_TEXTURE_2D, mtex[TEX_XTOON]);
  AUX_RGBImageRec *texXtoonImage;
  texXtoonImage = auxDIBImageLoad("XTOON.bmp");
  if (texXtoonImage == NULL)
  {
	  std::cout << "no texture file,please convert rgb file to bmp file"<< std::cout;
	  return;
  }
  glTexImage2D(GL_TEXTURE_2D, 0,
	  3, texXtoonImage->sizeX, texXtoonImage->sizeY, 0,
	  GL_RGB, GL_UNSIGNED_BYTE, texXtoonImage->data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  if(texXtoonImage)
  {
	  if(texXtoonImage->data)
	  {
		  free(texXtoonImage->data);
	  }
	  free(texXtoonImage);
  }



  //bind texture with FBO_RND
  GLuint DepthBuffer1;
  glGenRenderbuffersEXT(1, &DepthBuffer1);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, DepthBuffer1);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mirFBSize.x, mirFBSize.y);

  //glGenFramebuffersEXT(1, &mnFrameBuffer);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mfbo[FBO_RND]);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
				GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF1], 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
	            GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF2], 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,
	            GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BUFF3], 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT,
	            GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRANX], 0);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT4_EXT,
				GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRAN1], 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT5_EXT,
				GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_TRAN2], 0);


  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT6_EXT,
				GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_LINE3D1], 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT7_EXT,
				GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_LINE3D2], 0);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  //一定要加上depth，要不然会出现黑色的小三角，不知道为什么
				GL_RENDERBUFFER_EXT, DepthBuffer1);

  //bind texture with FBO_BLD
  GLuint DepthBuffer2;
  glGenRenderbuffersEXT(1, &DepthBuffer2);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, DepthBuffer2);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mirFBSize.x, mirFBSize.y);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mfbo[FBO_BLD]);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
				GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_MEDIATOR], 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
				GL_TEXTURE_RECTANGLE_ARB, mtex[TEX_BACKEDGE], 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,
			    GL_TEXTURE_RECTANGLE_ARB, mnFrameBufferTex, 0);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
  			       GL_RENDERBUFFER_EXT, DepthBuffer2);

  CheckFramebufferStatus();
  cout << " .. created FBO." << endl;
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


/**
 * check the status of the frame buffer
 */
static bool CheckFramebufferStatus()
{
  GLenum n;
  n = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(n == GL_FRAMEBUFFER_COMPLETE_EXT)
    return true; // All good
  
  cout << "glCheckFrameBufferStatusExt returned an error." << endl;
  return false;
}



/**
 * Draw the distorted frame buffer
 */
void ARDriver::DrawDistortedFB()
{
  static bool bFirstRun = true;
  static GLuint nList;
  mGLWindow.SetupViewport();
  mGLWindow.SetupUnitOrtho();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mnFrameBufferTex);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_BLEND);
  if(bFirstRun)
  {
      bFirstRun = false;
      nList = glGenLists(1);
      glNewList(nList, GL_COMPILE_AND_EXECUTE);
      // How many grid divisions in the x and y directions to use?
      int nStepsX = 24; // Pretty arbitrary..
      int nStepsY = (int) (nStepsX * ((double) mirFrameSize.x / mirFrameSize.y)); // Scaled by aspect ratio
      if(nStepsY < 2)
		nStepsY = 2;
      glColor3f(1,1,1);
      for(int ystep = 0; ystep<nStepsY; ystep++)
	  {
		glBegin(GL_QUAD_STRIP);
		for(int xstep = 0; xstep<=nStepsX; xstep++)
			for(int yystep = ystep; yystep<=ystep + 1; yystep++) // Two y-coords in one go - magic.
			{
				Vector<2> v2Iter;
				v2Iter[0] = (double) xstep / nStepsX;
				v2Iter[1] = (double) yystep / nStepsY;
				Vector<2> v2UFBDistorted = v2Iter;
				Vector<2> v2UFBUnDistorted = mCamera.UFBLinearProject(mCamera.UFBUnProject(v2UFBDistorted));
				glTexCoord2d(v2UFBUnDistorted[0] * mirFBSize.x, (1.0 - v2UFBUnDistorted[1]) * mirFBSize.y);
				glVertex(v2UFBDistorted);
			}
		glEnd();
	  }
      glEndList();
  }
  else
    glCallList(nList);
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
}


/**
 * What to do when the user clicks on the screen.
 * Calculates the 3d postion of the click on the plane
 * and passes info to a game, if there is one.
 * @param nButton the button pressed
 * @param irWin the window x, y location 
 */
void ARDriver::HandleClick(int nButton, ImageRef irWin )
{
  //The window may have been resized, so want to work out the coords based on the orignal image size
  Vector<2> v2VidCoords = mGLWindow.VidFromWinCoords( irWin );
  
  
  Vector<2> v2UFBCoords;
#ifdef WIN32
  Vector<2> v2PlaneCoords;   v2PlaneCoords[0] = numeric_limits<double>::quiet_NaN();   v2PlaneCoords[1] = numeric_limits<double>::quiet_NaN();
#else
  Vector<2> v2PlaneCoords;   v2PlaneCoords[0] = NAN;   v2PlaneCoords[1] = NAN;
#endif
  Vector<3> v3RayDirn_W;

  // Work out image coords 0..1:
  v2UFBCoords[0] = (v2VidCoords[0] + 0.5) / mCamera.GetImageSize()[0];
  v2UFBCoords[1] = (v2VidCoords[1] + 0.5) / mCamera.GetImageSize()[1];

  // Work out plane coords:
  Vector<2> v2ImPlane = mCamera.UnProject(v2VidCoords);
  Vector<3> v3C = unproject(v2ImPlane);
  Vector<4> v4C = unproject(v3C);
  SE3<> se3CamInv = mse3CfromW.inverse();
  Vector<4> v4W = se3CamInv * v4C;
  double t = se3CamInv.get_translation()[2];
  double dDistToPlane = -t / (v4W[2] - t);

  if(v4W[2] -t <= 0) // Clicked the wrong side of the horizon?
  {
    v4C.slice<0,3>() *= dDistToPlane;
    Vector<4> v4Result = se3CamInv * v4C;
    v2PlaneCoords = v4Result.slice<0,2>(); // <--- result
  }

  // Ray dirn:
  v3RayDirn_W = v4W.slice<0,3>() - se3CamInv.get_translation();
  normalize(v3RayDirn_W);

  if(mpMap->pGame) {
    mpMap->pGame->HandleClick(v2VidCoords, v2UFBCoords, v3RayDirn_W, v2PlaneCoords, nButton);
  }
}



/**
 * Handle the user pressing a key
 * @param sKey the key the user pressed.
 */
void ARDriver::HandleKeyPress( std::string sKey )
{
  if(mpMap && mpMap->pGame ) {
    mpMap->pGame->HandleKeyPress( sKey );
  }

}


/**
 * Load a game by name.
 * @param sName Name of the game
 */
void ARDriver::LoadGame(std::string sName)
{
  if(mpMap->pGame)
  {
    delete mpMap->pGame;
    mpMap->pGame = NULL;
  }

  mpMap->pGame = LoadAGame( sName, "");
  if( mpMap->pGame ) {
    mpMap->pGame->Init();
  }
 
}



/**
 * Advance the game logic
 */
void ARDriver::AdvanceLogic()
{
  if(mpMap->pGame) {
    mpMap->pGame->Advance();
  }
}


}
