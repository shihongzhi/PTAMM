// -*- c++ -*-
// Copyright 2009 Isis Innovation Limited
// ARDriver.h
// This file declares the ARDriver class
//
// ARDriver provides basic graphics services for drawing augmented
// graphics. It manages the OpenGL setup and the camera's radial
// distortion so that real and distorted virtual graphics can be
// properly blended.
//
#ifndef __AR_Driver_H
#define __AR_Driver_H
#include <TooN/se3.h>
#include "ATANCamera.h"
#include "GLWindow2.h"
#include "OpenGL.h"
#include <cvd/image.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>
#include "GLShader.h"

#ifndef NAN
#include <limits>
#endif 

namespace PTAMM {

using namespace std;
using namespace CVD;

class Map;

class ARDriver
{
  public:
    ARDriver(const ATANCamera &cam, ImageRef irFrameSize, GLWindow2 &glw, Map &map);
	void Render(Image<Rgb<CVD::byte> > &imFrame, SE3<> se3CamFromWorld, bool bLost);
    void Reset();
    void Init();

    void HandleClick(int nButton, ImageRef irWin );
    void HandleKeyPress( std::string sKey );
    void AdvanceLogic();
    void LoadGame(std::string sName);
  
    void SetCurrentMap(Map &map) { mpMap = &map; mnCounter = 0; }

  protected:
    void MakeFrameBuffer();
    void DrawDistortedFB();
    void SetFrustum();
    
    bool PosAndDirnInPlane(Vector<2> v2VidCoords, Vector<2> &v2Pos, Vector<2> &v2Dirn);

    
  protected:
    ATANCamera mCamera;
    GLWindow2 &mGLWindow;
    Map *mpMap;
  
    // Texture stuff:
    GLuint mnFrameBufferTex;  //最后显示的图片
    
    int mnCounter;
    ImageRef mirFBSize;
    ImageRef mirFrameSize;
    SE3<> mse3CfromW;
    bool mbInitialised;

	Image<Rgba<CVD::byte> > mLostOverlay;

private:
	void SetTexture(GLuint texId);
	void DrawQuad();
	void InitGaussTexture(GLuint texId);
	enum {FBO_RND = 0, FBO_ABS=1, FBO_BLD=2 };  //FBO_RND保存导入的model和阴影，FBO_BLD保存FBO_RND与视频图像的融合

	enum {TEX_DEPTH=0, TEX_XTOON, TEX_GAUSS1, TEX_GAUSS2, TEX_BUFF1, TEX_BUFF2, TEX_BUFF3,
		TEX_LINE3D1, TEX_LINE3D2, TEX_TRAN1, TEX_TRAN2, TEX_TRANX,
		TEX_IMAGE, TEX_MEDIATOR, TEX_BACKEDGE, TEX_BLEND};

	enum {PROG_BUFF=0, PROG_LINE3D1, PROG_LINE3D2, PROG_TRANX, PROG_TRANX1, PROG_TRANX2, PROG_TRANY, PROG_EDGE, PROG_BLEND};

	GLuint mfbo[3];
	GLuint mtex[16];
	Shader mshaders[9];
};

}

#endif
