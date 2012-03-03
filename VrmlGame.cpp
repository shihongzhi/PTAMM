#include "VrmlGame.h"
#include "Map.h"
#include "KeyFrame.h"
#include "GLShader.h"
#include <cvd/gl_helpers.h>
#include <cvd/image_io.h>
#include <gvars3/instances.h>
#include <GL/GLAux.h>
#include <GL/glut.h>
#include <fstream>
#include "readrgb.h"
//#include <stdio.h>
//#include <stdlib.h>


namespace PTAMM{

#define SHADOW_WAP_RATIO 2
#define RENDER_WIDTH 640.0
#define RENDER_HEIGHT 480.0

static bool CheckFramebufferStatus();

	using namespace CVD;
	using namespace TooN;
	using namespace GVars3;
	extern "C" unsigned *read_texture(const char *name,int *width,int *height,int *components);

	VrmlGame::VrmlGame():Game("Vrml"),b(std::cout,std::cerr)
	{	
		mbInitialised = false;
		isInitMap = false;
		isBindTexture = false;

		std::vector<std::string> parameter;
		std::vector<std::string> uri;
		//u型水管
		//uri.push_back("../vrmlmodels/30.wrl");
		uri.push_back("../vrmlmodels/36.wrl");  //立方体和圆柱
		//uri.push_back("../vrmlmodels/39.wrl");  //书本和笔
		b.load_url(uri, parameter);
		//texture = new GLuint;
		glGenTextures(20, texture);
		//glBindTexture(GL_TEXTURE_2D, texture);
		

		phongShadow.SetShaderFile("shaders/phongPCF.vs","shaders/phongPCF.fs");
		phongShadow.UseShader(false);
		GeneratrShadowFBO();  //产生
		InitMedMaskTexture();
		std::cout<<"VrmlGame generate method......"<<std::endl;
	}

	void VrmlGame::InitMedMaskTexture(){
		glGenTextures(1, &medMashTextureId);
		glBindTexture(GL_TEXTURE_2D, medMashTextureId);
		AUX_RGBImageRec *texMedMaskImage;
		texMedMaskImage = auxDIBImageLoad("MEDMASK.bmp");
		if (texMedMaskImage == NULL)
		{
			std::cout << "no texture file,please convert rgb file to bmp file"<< std::endl;
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0,
			3, texMedMaskImage->sizeX, texMedMaskImage->sizeY, 0,
			GL_RGB, GL_UNSIGNED_BYTE, texMedMaskImage->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if(texMedMaskImage)
		{
			if(texMedMaskImage->data)
			{
				free(texMedMaskImage->data);
			}
			free(texMedMaskImage);
		}
		std::cout << "load MEDMASK.bmp" << std::endl;
	}

	void VrmlGame::DrawMediatorAndObject(int statusFlag)
	{
		if (statusFlag == 3)
		{
			//mediator
			glColor4f(1.0f, 1.0f, 1.0f, 0.0f);   //这里的alpha值在之后test.fs中用于判断是否为中介面  ---2.23
			glBegin(GL_POLYGON);
			glVertex3f(1.2f, 0.0f, 1.2f);
			glVertex3f(1.2f, 0.0f, -1.2f);
			glVertex3f(-1.2f, 0.0f, -1.2f);
			glVertex3f(-1.2f, 0.0f, 1.2f);
			glEnd();
		}
		if (statusFlag == 1 || statusFlag == 2)
		{
			//mediator
			glColor4f(0.8f, 0.8f, 0.8f, 0.0f);   //这里的alpha值在之后test.fs中用于判断是否为中介面  ---2.23
			glBegin(GL_POLYGON);
			glVertex3f(1.2f, 0.0f, 1.2f);
			glVertex3f(1.2f, 0.0f, -1.2f);
			glVertex3f(-1.2f, 0.0f, -1.2f);
			glVertex3f(-1.2f, 0.0f, 1.2f);
			glEnd();
		}
		if (statusFlag == 4)
		{
			glColor4f(0.8f, 0.8f, 0.8f, 0.0f);
			glBegin(GL_POLYGON);
			glVertex3f(0.6f, 0.0f, 0.6f);
			glVertex3f(0.6f, 0.0f, -0.6f);
			glVertex3f(-0.6f, 0.0f, -0.6f);
			glVertex3f(-0.6f, 0.0f, 0.6f);
			glEnd();

			//mediator
			glColor4f(1.0f, 1.0f, 1.0f, 0.0f);   //这里的alpha值在之后test.fs中用于判断是否为中介面  ---2.23
			glEnable(GL_TEXTURE_2D);
			//为什么加了这句话就没有阴影了  --2.23
			//glBindTexture(GL_TEXTURE_2D, medMashTextureId);

			//back face
			/*glBegin(GL_POLYGON);
			glVertex3f(1.2f, 0.0f, 1.2f);
			glVertex3f(0.5f, 0.0f, 1.2f);
			glVertex3f(0.5f, 0.0f, -1.2f);
			glVertex3f(1.2f, 0.0f, -1.2f);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(0.5f, 0.0f, 1.2f);
			glVertex3f(-0.5f, 0.0f, 1.2f);
			glVertex3f(-0.5f, 0.0f, 0.5f);
			glVertex3f(0.5f, 0.0f, 0.5f);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(-0.5f, 0.0f, 1.2f);
			glVertex3f(-1.2f, 0.0f, 1.2f);
			glVertex3f(-1.2f, 0.0f, -1.2f);
			glVertex3f(-0.5f, 0.0f, -1.2f);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(0.5f, 0.0f, -0.5f);
			glVertex3f(-0.5f, 0.0f, -0.5f);
			glVertex3f(-0.5f, 0.0f, -1.2f);
			glVertex3f(0.5f, 0.0f, -1.2f);
			glEnd();*/

			////front face
			glBegin(GL_POLYGON);
			glVertex3f(1.2f, 0.0f, 1.2f);
			glVertex3f(1.2f, 0.0f, -1.2f);
			glVertex3f(0.6f, 0.0f, -1.2f);
			glVertex3f(0.6f, 0.0f, 1.2f);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(0.6f, 0.0f, 1.2f);
			glVertex3f(0.6f, 0.0f, 0.6f);
			glVertex3f(-0.6f, 0.0f, 0.6f);
			glVertex3f(-0.6f, 0.0f, 1.2f);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(-0.6f, 0.0f, 1.2f);
			glVertex3f(-0.6f, 0.0f, -1.2f);
			glVertex3f(-1.2f, 0.0f, -1.2f);
			glVertex3f(-1.2f, 0.0f, 1.2f);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(0.6f, 0.0f, -0.6f);
			glVertex3f(0.6f, 0.0f, -1.2f);
			glVertex3f(-0.6f, 0.0f, -1.2f);
			glVertex3f(-0.6f, 0.0f, -0.6f);
			glEnd();
			//glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		if (statusFlag == 0 || statusFlag == 2 || statusFlag == 3 ||statusFlag == 4)
		{
			//Virtual object
			std::vector<openvrml::node_ptr> mfn;
			mfn = b.root_nodes();
			if (mfn.size() == 0)
			{
				return;
			}
			else
			{
				for (unsigned int i=0; i<mfn.size(); i++)
				{
					openvrml::node* vrml_node = mfn[i].get();
					Draw3DFromVRML(vrml_node);
				}
			}
		}
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

	//识别“标”，得到旋转平移矩阵
	void VrmlGame::GetMatrixMP(Map &map)
	{
		if (!isInitMap)
		{
			std::cout<<"find mark...."<<std::endl;
			mpMap = &map;
			std::vector<std::vector<Wml::Vector2d> > all_points_2d;
			detect_corners(std::string("./absolute.txt"),all_points_2d);

			std::vector<Wml::Matrix4d> all_cameras;
			std::vector<Wml::Matrix3d> all_Ks;
			vector<KeyFrame *>::iterator kf;
			int count = 1;
			for( kf = mpMap->vpKeyFrames.begin(); kf != mpMap->vpKeyFrames.end(); kf++ )
			{
				int width = (*kf)->Camera.ImageSize()[0];
				int height = (*kf)->Camera.ImageSize()[1];
				TooN::Vector<5> params = (*kf)->Camera.GetParams();
				Wml::Matrix3d K;
				K(0,0) = width * params[0];
				K(0,1) = 0;
				K(0,2) = width * params[2];
				K(1,0) = 0;
				K(1,1) = height * params[1];
				K(1,2) = height * params[3];
				K(2,0) = 0;
				K(2,1) = 0;
				K(2,2) = 1;
				if(count==1)
				{
					std::cout<<K(0,0)<<","<<K(1,1)<<","<<K(0,2)<<","<<K(1,2)<<std::endl;
				}
				all_Ks.push_back(K);

				Wml::Matrix4d P;
				P(0,3) = (*kf)->se3CfromW.get_translation()[0];
				P(1,3) = (*kf)->se3CfromW.get_translation()[1];
				P(2,3) = (*kf)->se3CfromW.get_translation()[2];
				P(0,0) = (*kf)->se3CfromW.get_rotation().get_matrix()[0][0];
				P(0,1) = (*kf)->se3CfromW.get_rotation().get_matrix()[0][1];
				P(0,2) = (*kf)->se3CfromW.get_rotation().get_matrix()[0][2];
				P(1,0) = (*kf)->se3CfromW.get_rotation().get_matrix()[1][0];
				P(1,1) = (*kf)->se3CfromW.get_rotation().get_matrix()[1][1];
				P(1,2) = (*kf)->se3CfromW.get_rotation().get_matrix()[1][2];
				P(2,0) = (*kf)->se3CfromW.get_rotation().get_matrix()[2][0];
				P(2,1) = (*kf)->se3CfromW.get_rotation().get_matrix()[2][1];
				P(2,2) = (*kf)->se3CfromW.get_rotation().get_matrix()[2][2];
				if(count==1)
				{
					std::cout<<P(0,3)<<","<<P(1,3)<<","<<P(2,3)<<","<<P(0,0)<<","<<P(0,1)<<","<<P(0,2)<<","<<P(1,0)<<","<<P(1,1)<<","<<P(1,2)<<","<<P(2,0)<<","<<P(2,1)<<","<<P(2,2)<<std::endl;
				}
				P(3,0) = 0;
				P(3,1) = 0;
				P(3,2) = 0;
				P(3,3) = 1;
				all_cameras.push_back(P);
				count++;
			}

			std::vector<Wml::Vector3d> corners;
			CRobust3DRecovery r3dr;
			for (int i = 0; i < 4; ++ i)
			{
				std::vector<Wml::Vector2d> points_2d;
				std::vector<Wml::Matrix4d> key_cameras;
				std::vector<Wml::Matrix3d> key_Ks;

				for (int f = 0; f < all_points_2d.size(); ++ f)
				{
					if (all_points_2d[f].size() > i)
					{
						points_2d.push_back(all_points_2d[f][i]);
						key_cameras.push_back(all_cameras[f]);
						key_Ks.push_back(all_Ks[f]);
					}
				}

				if (points_2d.size() > 2)
				{
					Wml::Vector3d point_3d(0, 0, 0);
					r3dr.generate(points_2d, key_cameras, key_Ks, point_3d, 3, 10.0, 10);

					std::cout<<"3dpoint"<<i<<" = ["<<point_3d[0] << ", "<<point_3d[1] << ", "<<point_3d[2] << "]\n";

					corners.push_back(point_3d);
				}
			}

			if (corners.size() == 4)
			{
				Wml::Vector3d center(0.0, 0.0, 0.0);
				for (int i = 0; i < corners.size(); ++ i)
				{
					center += corners[i];
				}
				center /= corners.size();

				std::cout<<"center = ["<<center[0]<<", "<<center[1]<<", "<<center[2]<<","<<center[3]<<"]"<<std::endl;
				//////////////////////////////////////////////////////////////////////////

				Wml::Vector3d xaxis = corners[3] - corners[0];
				Wml::Vector3d zaxis = corners[1] - corners[0];
				Wml::Vector3d axis12 = corners[1] - corners[2];
				Wml::Vector3d axis32 = corners[3] - corners[2];

				// 坐标系尺度。
				//double scale = 4.0/(xaxis.Length() + zaxis.Length() + axis12.Length() + axis32.Length());
				double scale = (xaxis.Length() + zaxis.Length() + axis12.Length() + axis32.Length())/4.0;
				//新坐标
				Wml::Vector3d yaxis = zaxis.Cross(xaxis);
				zaxis = xaxis.Cross(yaxis);
				xaxis.Normalize();
				yaxis.Normalize();
				zaxis.Normalize();

				//注意要scale的，不知道为什么，但是如果scale是用1代替的话，就会出现PTAMM地图大得话，导入的modle就会很大
				//反之则会很小
				Wml::Vector3d u(scale,0,0);
				Wml::Vector3d v(0,scale,0);
				Wml::Vector3d n(0,0,scale);
				mP(0,0) = xaxis.Dot(u);
				mP(0,1) = xaxis.Dot(v);
				mP(0,2) = xaxis.Dot(n);
				mP(1,0) = yaxis.Dot(u);
				mP(1,1) = yaxis.Dot(v);
				mP(1,2) = yaxis.Dot(n);
				mP(2,0) = zaxis.Dot(u);
				mP(2,1) = zaxis.Dot(v);
				mP(2,2) = zaxis.Dot(n);
				mP(0,3) = center[0];
				mP(1,3) = center[1];
				mP(2,3) = center[2];
				mP(3,0) = 0;
				mP(3,1) = 0;
				mP(3,2) = 0;
				mP(3,3) = 1;
			}
			isInitMap = true;
		}
	}

	void VrmlGame::GeneratrShadowFBO()
	{
		int shadowMapWidth = RENDER_WIDTH * SHADOW_WAP_RATIO;
		int shadowMapHeight = RENDER_HEIGHT * SHADOW_WAP_RATIO;

		//GLfloat borderColor[4] = {0,0,0,0};

		GLenum FBOstatus;

		// Try to use a texture depth component
		glGenTextures(1, &depthTextureId);
		glBindTexture(GL_TEXTURE_2D, depthTextureId);

		// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


		// Remove artefact on the edges of the shadowmap
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

		// This is to allow usage of shadow2DProj function in the shader
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); 


		// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// create a framebuffer object
		glGenFramebuffersEXT(1, &fboShadowId);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboShadowId);

		// Instruct openGL that we won't bind a color texture with the currently binded FBO
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		// attach the texture to FBO depth attachment point
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0);

		// check FBO status
		FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
			printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

		// switch back to window-system-provided framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		std::cout<< "Generate Shadow FBO" << std::endl;
	}


	void VrmlGame::SetTextureMatrix()
	{
		static double modelView[16];
		static double projection[16];

		// This is matrix transform every coordinate x,y,z
		// x = x* 0.5 + 0.5 
		// y = y* 0.5 + 0.5 
		// z = z* 0.5 + 0.5 
		// Moving from unit cube [-1,1] to [0,1]  
		const GLdouble bias[16] = {	
			0.5, 0.0, 0.0, 0.0, 
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0};

		// Grab modelview and transformation matrices
		glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);


		glMatrixMode(GL_TEXTURE);
		glActiveTextureARB(GL_TEXTURE7);

		glLoadIdentity();	
		glLoadMatrixd(bias);

		// concatating all matrice into one.
		glMultMatrixd (projection);
		glMultMatrixd (modelView);

		// Go back to normal matrix mode
		glMatrixMode(GL_MODELVIEW);
	}

	void VrmlGame::Draw3D( const GLWindow2 &glWindow, Map &map, SE3<> se3CfromW, GLuint fboId, GLenum *buffers, ATANCamera &mCamera, int statusFlag)
	{
		if(!mbInitialised) {
			Init();
		}
		GetMatrixMP(map);
		GLfloat temp_trans[] = {mP(0,0), mP(0,1), mP(0,2), mP(0,3), mP(1,0), mP(1,1), mP(1,2), mP(1,3), mP(2,0), mP(2,1), mP(2,2), mP(2,3), mP(3,0), mP(3,1), mP(3,2), mP(3,3)};
		
		//add at 2.23
		//读取阴影明暗变量
		std::string filename = "variable.txt";
		std::ifstream variableFile;
		float shadowvariable;
		int light_count;
		variableFile.open(filename.c_str());
		if (!variableFile)
		{
			cout << "can't open the variable file" <<endl;
		}
		variableFile>>shadowvariable;
		variableFile>>light_count;

		if (statusFlag != 2 && statusFlag != 0)
		{
			//shadow
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboShadowId);
			CheckFramebufferStatus();
			glUseProgramObjectARB(0);
			glViewport(0,0,RENDER_WIDTH*SHADOW_WAP_RATIO,RENDER_HEIGHT*SHADOW_WAP_RATIO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			//setup matrices
			glEnable(GL_DEPTH_TEST);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45,RENDER_WIDTH/RENDER_HEIGHT,3,40000);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_NORMALIZE);
			glEnable(GL_COLOR_MATERIAL);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(0.0, 10.0, 0.0, -10.0, -100.0, 0.0, 0.0, 1.0, 0.0);
			//消除自我阴影
			glEnable(GL_CULL_FACE);  //之前没有加这句话，所以Front没有被剔除
			glCullFace(GL_FRONT);
			DrawMediatorAndObject(statusFlag);  //需要设置texture7
			SetTextureMatrix();  //设置texture7
		}
 		
		

		//add 12.8  draw virtual object
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);
		//ATTACHMENT0---TEX_BUFF1
		CheckFramebufferStatus();
		glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);  //
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		// Set up 3D projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMultMatrix(mCamera.MakeUFBLinearFrustumMatrix(0.005, 100));
		glMultMatrix(se3CfromW);
		
	    //opengl staff
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
 		glEnable(GL_NORMALIZE);
 		glEnable(GL_COLOR_MATERIAL);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
 		glMultMatrix(SE3<>());
 		glMultMatrixf(temp_trans);


		
		phongShadow.UseShader(true);
		glEnable(GL_TEXTURE_2D);
		if (statusFlag != 2 && statusFlag != 0)
		{
			phongShadow.SetUniVar("xPixelOffset", 1.0f/ (640.0f* 2));
			phongShadow.SetUniVar("yPixelOffset", 1.0f/ (480.0f* 2));
			phongShadow.SetSampler("ShadowMap",7);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, depthTextureId);
		}
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		DrawMediatorAndObject(statusFlag);
		if (statusFlag != 2 && statusFlag != 0)
		{
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		phongShadow.UseShader(false);

		////test shadow mapping
		//貌似这段程序没法画出深度图，不过印象中之前可以的啊  ---2.23
		/*glUseProgramObjectARB(0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-RENDER_WIDTH/2,RENDER_WIDTH/2,-RENDER_HEIGHT/2,RENDER_HEIGHT/2,1,20);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor4f(1,1,1,1);
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,depthTextureId);
		glEnable(GL_TEXTURE_2D);
		glTranslated(0,0,-1);
		glBegin(GL_QUADS);
		glTexCoord2d(0,0);glVertex3f(0,0,0);
		glTexCoord2d(1,0);glVertex3f(RENDER_WIDTH/2,0,0);
		glTexCoord2d(1,1);glVertex3f(RENDER_WIDTH/2,RENDER_HEIGHT/2,0);
		glTexCoord2d(0,1);glVertex3f(0,RENDER_HEIGHT/2,0);
		glEnd();
		glDisable(GL_TEXTURE_2D);*/

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
	}

	void VrmlGame::Draw3DFromVRML(openvrml::node *obj)
	{
		try
		{
			//可能抛出openvrml::unsupported_interface异常
			const openvrml::field_value &fv_appearance = obj->field("appearance");
			if (fv_appearance.type() == openvrml::field_value::sfnode_id)
			{
				//std::cout<<"appearance"<<std::endl;
				const openvrml::sfnode &sfn = dynamic_cast<const openvrml::sfnode&>(fv_appearance);
				openvrml::vrml97_node::appearance_node *vrml_app =
					static_cast<openvrml::vrml97_node::appearance_node*>(sfn.value.get()->to_appearance());
				const openvrml::node_ptr &vrml_material_node = vrml_app->material();
				const openvrml::node_ptr &vrml_texture_node = vrml_app->texture();

				const openvrml::vrml97_node::material_node *vrml_material =
					dynamic_cast<const openvrml::vrml97_node::material_node *>(vrml_material_node.get());
				if (vrml_material != NULL)
				{
					GLfloat temp_material_mat[4];
					temp_material_mat[0] = vrml_material->ambient_intensity();
					temp_material_mat[1] = vrml_material->ambient_intensity();
					temp_material_mat[2] = vrml_material->ambient_intensity();
					temp_material_mat[3] = 1.0;
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, temp_material_mat);
					temp_material_mat[0] = vrml_material->diffuse_color().r();
					temp_material_mat[1] = vrml_material->diffuse_color().g();
					temp_material_mat[2] = vrml_material->diffuse_color().b();
					temp_material_mat[3] = 1.0;
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, temp_material_mat);
					temp_material_mat[0] = vrml_material->emissive_color().r();
					temp_material_mat[1] = vrml_material->emissive_color().g();
					temp_material_mat[2] = vrml_material->emissive_color().b();
					temp_material_mat[3] = 1.0;
					glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, temp_material_mat);
					temp_material_mat[0] =vrml_material->specular_color().r();
					temp_material_mat[1] =vrml_material->specular_color().g();
					temp_material_mat[2] =vrml_material->specular_color().b();
					temp_material_mat[3] = 1.0;
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, temp_material_mat);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, vrml_material->shininess());

					if (vrml_material->transparency() > 0.0f)
					{
						//设置透明性
					}
					else
					{

					}
					//std::cout<<"material"<<std::endl;
				}

				//bind texture
				const openvrml::vrml97_node::image_texture_node *vrml_texture = 
					dynamic_cast<const openvrml::vrml97_node::image_texture_node *>(vrml_texture_node.get());
				if (vrml_texture != 0)
				{
					const openvrml::field_value &texture_url_fv = vrml_texture->field("url");
					const openvrml::mfstring &mfs = dynamic_cast<const openvrml::mfstring &>(texture_url_fv);

					std::pair<set<std::string>::iterator, bool> ret;

					//修改后缀为bmp
					std::string url = mfs.value[0];
					std::string dot(".");
					std::size_t found;
					found = url.find(dot);
					if (found != std::string::npos)
					{
						std::string bmp("bmp");
						url.replace(found+1,3,bmp);
						ret = textureUrls.insert(url);
						//std::cout<< url.c_str() << std::endl;
					}
					if (ret.second == true)  //如果url插入成功了，即来了一个新的url，则设置isBindTexture，使得之后生成新的纹理
					{
						isBindTexture = false;
					}

					//即查找现在的url为第一个纹理
					int count = 0;
					for (std::set<std::string>::iterator it=textureUrls.begin(); it!=textureUrls.end(); it++,count++)
					{
						if (*it == *ret.first)
						{
							textureNum = count;
							//std::cout<< count << std::endl;
							break;
						}
					}
					//绑定纹理
					if (!isBindTexture)
					{
						glBindTexture(GL_TEXTURE_2D, texture[textureNum]);
						AUX_RGBImageRec *texImage;
						texImage = auxDIBImageLoad(url.c_str());

						if (texImage == NULL)
						{
							std::cout << "no texture file,please convert rgb file to bmp file"<< std::cout;
							return;
						}
						glTexImage2D(GL_TEXTURE_2D, 0, 3,texImage->sizeX, texImage->sizeY,
							0, GL_RGB, GL_UNSIGNED_BYTE, texImage->data);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

						if(texImage)
						{
							if(texImage->data)
							{
								free(texImage->data);
							}
							free(texImage);
						}
						//std::cout<< texture << std::endl;
						//std::cout<<"bind texture ok"<<std::endl;
						isBindTexture = true;
					}
					
					//读rgb文件有问题，暂时用bmp文件代替 
					//用read_texture函数返回的结果有问题，都是空格
					//int width = 180;
					//int height = 180;
					//int components = 4;
					//GLubyte *texels = (GLubyte *)read_texture(url.c_str(),&width,&height,&components);
					//std::cout<< url.c_str() << std::endl;
					//std::cout<< *read_texture(url.c_str(),&width,&height,&components) << std::endl;
				}
			}
		}
		catch(openvrml::unsupported_interface&)
		{

		}
		

		std::string name = obj->id();
		if (obj->type.id == "Group")
		{
			//std::cout<<"Group"<<std::endl;
			openvrml::vrml97_node::group_node *vrml_group;
			vrml_group = dynamic_cast<openvrml::vrml97_node::group_node *>(obj);

			try
			{
				const openvrml::field_value &fv = obj->field("children");

				if (fv.type() == openvrml::field_value::mfnode_id)
				{
					const openvrml::mfnode &mfn = dynamic_cast<const openvrml::mfnode &>(fv);
					for (unsigned int i=0; i<mfn.value.size(); i++)
					{
						openvrml::node *node = mfn.value[i].get();
						Draw3DFromVRML(node);
					}
				}
			}
			catch (openvrml::unsupported_interface&)
			{
				//no children
			}
		}
		else if (obj->type.id == "Transform")
		{
			//std::cout<<"Transform"<<std::endl;
			openvrml::vrml97_node::transform_node* vrml_transform;
			vrml_transform = dynamic_cast<openvrml::vrml97_node::transform_node*>(obj);

			openvrml::mat4f vrml_m = vrml_transform->transform();

			//opengl 添加转换
			glPushMatrix();
			GLfloat temp_trans[] = {vrml_m[0][0], vrml_m[0][1], vrml_m[0][2], vrml_m[0][3], vrml_m[1][0], vrml_m[1][1], vrml_m[1][2], vrml_m[1][3], vrml_m[2][0], vrml_m[2][1], vrml_m[2][2], vrml_m[2][3], vrml_m[3][0], vrml_m[3][1], vrml_m[3][2], vrml_m[3][3]};
			//glMultTransposeMatrixf(temp_trans);  
			glMultMatrixf(temp_trans);	//不需要转制，列优先

			//add 12.9
			glMatrixMode(GL_TEXTURE);
			glActiveTextureARB(GL_TEXTURE7);
			glPushMatrix();
			glMultMatrixf(temp_trans);
			try
			{
				const openvrml::field_value &fv = obj->field("children");

				if (fv.type() == openvrml::field_value::mfnode_id)
				{
					const openvrml::mfnode &mfn = dynamic_cast<const openvrml::mfnode &>(fv);
					for (unsigned int i=0; i<mfn.value.size(); i++)
					{
						openvrml::node *node = mfn.value[i].get();
						Draw3DFromVRML(node);
					}
				}
			}
			catch(openvrml::unsupported_interface&)
			{
				//no children
			}
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
		else if(obj->type.id == "Shape")
		{
			const openvrml::field_value &fv = obj->field("geometry");
			if (fv.type() == openvrml::field_value::sfnode_id)
			{
				const openvrml::sfnode &sfn = dynamic_cast<const openvrml::sfnode&>(fv);

				openvrml::vrml97_node::abstract_geometry_node* vrml_geom =
					static_cast<openvrml::vrml97_node::abstract_geometry_node*>(sfn.value.get()->to_geometry());

				if (openvrml::vrml97_node::indexed_face_set_node* vrml_ifs = dynamic_cast<openvrml::vrml97_node::indexed_face_set_node *>(vrml_geom))
				{
					DrawIndexedFaceSet(vrml_ifs);
				}
				else if (openvrml::vrml97_node::box_node* vrml_box = dynamic_cast<openvrml::vrml97_node::box_node*>(vrml_geom))
				{
					//opengl box
					//const openvrml::vec3f& size = static_cast<const openvrml::sfvec3f&>(vrml_box->field("size")).value;
					std::cout<<"box"<<std::endl;
					DrawBox(vrml_box);
				}
				else if (openvrml::vrml97_node::sphere_node* vrml_sphere = dynamic_cast<openvrml::vrml97_node::sphere_node*>(vrml_geom))
				{
					//sphere
					std::cout<<"sphere"<<std::endl;
					DrawSphere(vrml_sphere);
				}
				else if (openvrml::vrml97_node::cone_node* vrml_cone = dynamic_cast<openvrml::vrml97_node::cone_node*>(vrml_geom))
				{
					//cone
				}
				else if (openvrml::vrml97_node::cylinder_node* vrml_cylinder = dynamic_cast<openvrml::vrml97_node::cylinder_node*>(vrml_geom))
				{
					//cylinder
				}
				else
				{

				}
			}
		}
	}

	void VrmlGame::DrawBox(openvrml::vrml97_node::box_node* vrml_box) const
	{
		const openvrml::vec3f& size = static_cast<const openvrml::sfvec3f&>(vrml_box->field("size")).value;
		std::cout << size[0]<< ","<< size[1]<<","<< size[2]<<std::endl;
		GLfloat a,b,c;
		a = size[0]*0.5;
		b = size[1]*0.5;
		c = size[2]*0.5;
		static GLfloat vertex_list[][3] = {
			-a,-b,-c,
			 a,-b,-c,
		    -a, b,-c,
			 a, b,-c,
		    -a,-b, c,
			 a,-b, c,
		    -a, b, c,
			 a, b, c,
		};
		static GLint index_list[][4] = {
			0,2,3,1,
			0,4,6,2,
			0,1,5,4,
			4,5,7,6,
			1,3,7,5,
			2,6,7,3,
		};
		int i,j;
		glColor3f(0.0,0.0,0.0);
		glBegin(GL_QUADS);
			for(i=0; i<6; ++i)
				for(j=0; j<6; ++j)
					glVertex3fv(vertex_list[index_list[i][j]]);
		glEnd();
	}

	void VrmlGame::DrawSphere(openvrml::vrml97_node::sphere_node* vrml_sphere) const
	{
		float radius = static_cast<const openvrml::sffloat&>(vrml_sphere->field("radius")).value;
		GLUquadricObj* quadric = NULL;
		quadric = gluNewQuadric();
		gluSphere(quadric, radius, 36, 36);
		gluDeleteQuadric(quadric);
	}

	//一般情况下都不会用到color和normal参数
	void VrmlGame::DrawIndexedFaceSet(openvrml::vrml97_node::indexed_face_set_node* vrml_ifs) const
	{
		//get array of coordinate_nodes
		{
			//coord value
			const openvrml::field_value& fv_coord = vrml_ifs->field("coord");
			const openvrml::sfnode& sfn_coord = dynamic_cast<const openvrml::sfnode&>(fv_coord);
			openvrml::vrml97_node::coordinate_node* vrml_coord_node = 
				dynamic_cast<openvrml::vrml97_node::coordinate_node*>(sfn_coord.value.get());
			//texture value
			const openvrml::field_value &fv_texCoord = vrml_ifs->field("texCoord");
			const openvrml::sfnode &sfn = dynamic_cast<const openvrml::sfnode &>(fv_texCoord);
			openvrml::vrml97_node::texture_coordinate_node *vrml_tex_coord_node =
				dynamic_cast<openvrml::vrml97_node::texture_coordinate_node *>(sfn.value.get());
			//color value
			const openvrml::field_value &fv_color = vrml_ifs->field("color");
			const openvrml::sfnode &sfn_color = dynamic_cast<const openvrml::sfnode &>(fv_color);
			openvrml::vrml97_node::color_node *vrml_color_node =
				dynamic_cast<openvrml::vrml97_node::color_node *>(sfn_color.value.get());
			//normal value
			const openvrml::field_value &fv_normal = vrml_ifs->field("normal");
			const openvrml::sfnode &sfn_normal = dynamic_cast<const openvrml::sfnode&>(fv_normal);
			openvrml::vrml97_node::normal_node* vrml_normal_node =
				dynamic_cast<openvrml::vrml97_node::normal_node*>(sfn_normal.value.get());

			//coord index
			const std::vector<openvrml::vec3f>& vrml_coord = vrml_coord_node->point();
			const openvrml::field_value &fv2_coord = vrml_ifs->field("coordIndex");
			const openvrml::mfint32 &vrml_coord_index = dynamic_cast<const openvrml::mfint32&>(fv2_coord);

			//texture index
			std::vector<openvrml::vec2f> vrml_tex_coord;
			std::vector<int> tex_coord_index;
			if (vrml_tex_coord_node != 0)
			{
				vrml_tex_coord = vrml_tex_coord_node->point();
				const openvrml::field_value &fv2_texCoord = vrml_ifs->field("texCoordIndex");
				const openvrml::mfint32 &vrml_tex_coord_index = dynamic_cast<const openvrml::mfint32 &>(fv2_texCoord);
				if (vrml_tex_coord_index.value.size() > 0)  //一般是这种情况
				{
					for (unsigned int i=0; i<vrml_tex_coord_index.value.size(); i++)
					{
						int index = vrml_tex_coord_index.value[i];
						if (index != -1)
						{
							tex_coord_index.push_back(index);
						}
					}
				}
				else
				{
					for (unsigned int i=0; i<vrml_tex_coord_index.value.size(); i++)
					{
						tex_coord_index.push_back(i);
					}
				}
			}
 
			//color index
			//这里有错误
			//引用初始化问题不知道如何解决
			//const std::vector<openvrml::color>& vrml_colors = vrml_color_node->color();
			std::vector<openvrml::color> vrml_colors;
			std::vector<int> color_index;
			bool is_vrml_color_per_vertex;
			if (vrml_color_node != 0)
			{
				vrml_colors = vrml_color_node->color();
				const openvrml::field_value &fv2_color = vrml_ifs->field("colorIndex");
				const openvrml::mfint32 &vrml_color_index = dynamic_cast<const openvrml::mfint32 &>(fv2_color);
				if (vrml_color_index.value.size() > 0)
				{
					for (unsigned int i=0; i<vrml_color_index.value.size(); i++)
					{
						int index = vrml_color_index.value[i];
						if (index != -1)
						{
							color_index.push_back(index);
						}
					}
				} 
				else
				{
					for (unsigned int i=0; i<vrml_coord_index.value.size(); i++)
					{
						color_index.push_back(i);
					}
				}
				const openvrml::field_value &fv3_color = vrml_ifs->field("colorPerVertex");
				const openvrml::sfbool &vrml_color_per_vertex = dynamic_cast<const openvrml::sfbool &>(fv3_color);
				is_vrml_color_per_vertex = vrml_color_per_vertex.value;
			}

			//normal index
			std::vector<openvrml::vec3f> vrml_normal_coord;
			std::vector<int> normal_index;
			bool is_vrml_normal_per_vertex;
			if (vrml_normal_node != 0)
			{
				vrml_normal_coord = vrml_normal_node->vector();
				const openvrml::field_value &fv2_normal = vrml_ifs->field("normalIndex");
				const openvrml::mfint32 &vrml_normal_index = dynamic_cast<const openvrml::mfint32&>(fv2_normal);
				if (vrml_normal_index.value.size() > 0)
				{
					for (unsigned int i=0; i<vrml_normal_index.value.size(); i++)
					{
						int index = vrml_normal_index.value[i];
						if (index != -1)
						{
							normal_index.push_back(index);
						}
					}
				}
				else
				{
					for (unsigned int i=0; i<vrml_coord_index.value.size(); i++)
					{
						normal_index.push_back(i);
					}
				}
				const openvrml::field_value &fv3_normal = vrml_ifs->field("normalPerVertex");
				const openvrml::sfbool &vrml_norm_per_vertex = dynamic_cast<const openvrml::sfbool &>(fv3_normal);
				is_vrml_normal_per_vertex = vrml_norm_per_vertex.value;
			}


			std::vector<int> vert_index;
			int num_vert = 0;  //标记总共画了一个vertex
			int num_polygon = 0;
			for (unsigned int i=0; i<vrml_coord_index.value.size(); i++)
			{
				int index = vrml_coord_index.value[i];
				if (index == -1)
				{
					GLfloat normal[3];
					GLfloat vc1[3],vc2[3];
					GLfloat a,b,c;
					GLdouble r;
					vc1[0] = vrml_coord[vert_index[1]][0] - vrml_coord[vert_index[0]][0];
					vc1[1] = vrml_coord[vert_index[1]][1] - vrml_coord[vert_index[0]][1];
					vc1[2] = vrml_coord[vert_index[1]][2] - vrml_coord[vert_index[0]][2];
					
					vc2[0] = vrml_coord[vert_index[2]][0] - vrml_coord[vert_index[0]][0];
					vc2[1] = vrml_coord[vert_index[2]][1] - vrml_coord[vert_index[0]][1];
					vc2[2] = vrml_coord[vert_index[2]][2] - vrml_coord[vert_index[0]][2];
					a = vc1[1] * vc2[2] - vc2[1] * vc1[2];
					b = vc2[0] * vc1[2] - vc1[0] * vc2[2];
					c = vc1[0] * vc2[1] - vc2[0] * vc1[1];
					r = sqrt( a * a + b* b + c * c);
					normal[0] = a / r;
					normal[1] = b / r;
					normal[2] = c / r;
					//set color if have color
					if (vrml_color_node != 0 && !is_vrml_color_per_vertex)
					{
						glColor4f(vrml_colors[color_index[num_polygon]][0],vrml_colors[color_index[num_polygon]][1],vrml_colors[color_index[num_polygon]][2], 1.0f);
					}
					else if(vrml_tex_coord_node == 0) //如果没有颜色属性，而且没有问题，则默认为白色
					{
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						//glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
					}
					//set normal if polygon
					if (vrml_normal_node !=0 && !is_vrml_normal_per_vertex)
					{
						glNormal3f(vrml_normal_coord[normal_index[num_polygon]][0],vrml_normal_coord[normal_index[num_polygon]][1],vrml_normal_coord[normal_index[num_polygon]][2]);
					}

					if (vrml_tex_coord_node != 0)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, texture[textureNum]);
					}
					glBegin(GL_POLYGON);
					for (std::vector<int>::iterator it = vert_index.begin(); it!=vert_index.end(); ++it)
					{
						//set color if per vertex
						if (vrml_color_node != 0 && is_vrml_color_per_vertex)
						{
							glColor4f(vrml_colors[color_index[num_vert]][0],vrml_colors[color_index[num_vert]][1],vrml_colors[color_index[num_vert]][2], 1.0f);
						}

						//set normal if per vertex
						if (vrml_normal_node !=0 && is_vrml_normal_per_vertex)
						{
							glNormal3f(vrml_normal_coord[normal_index[num_vert]][0],vrml_normal_coord[normal_index[num_vert]][1],vrml_normal_coord[normal_index[num_vert]][2]);
						}
						else if (vrml_normal_node == 0) //default
						{
							//glNormal3f(vrml_coord[*it][0],vrml_coord[*it][1],vrml_coord[*it][2]);
							glNormal3f(normal[0], normal[1], normal[2]);
						}
						
						//set texture coord
						if (vrml_tex_coord_node != 0)
						{
							glTexCoord2f(vrml_tex_coord[tex_coord_index[num_vert]][0],vrml_tex_coord[tex_coord_index[num_vert]][1]);
						}
						glVertex3f(vrml_coord[*it][0],vrml_coord[*it][1],vrml_coord[*it][2]);
						num_vert++;
					}
					glEnd();
					if (vrml_tex_coord_node != 0)
					{
						glDisable(GL_TEXTURE_2D);
					}
					num_polygon++;
					vert_index.clear();
				} 
				else
				{
					vert_index.push_back(index);
				} 
			}
		}
	}

	/*char * VrmlGame::textFileRead(char *fn)
	{
		FILE *fp;
		char *content = NULL;

		int count=0;

		if (fn != NULL) {
			fp = fopen(fn,"rt");

			if (fp != NULL) {

				fseek(fp, 0, SEEK_END);
				count = ftell(fp);
				rewind(fp);

				if (count > 0) {
					content = (char *)malloc(sizeof(char) * (count+1));
					count = fread(content,sizeof(char),count,fp);
					content[count] = '\0';
				}
				fclose(fp);
			}
		}
		return content;
	}

	int VrmlGame::textFileWrite(char *fn, char *s)
	{
		FILE *fp;
		int status = 0;

		if (fn != NULL) {
			fp = fopen(fn,"w");

			if (fp != NULL) {

				if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
					status = 1;
				fclose(fp);
			}
		}
		return(status);
	}*/

	//AUX_RGBImageRec *VrmlGame::LoadBMP(std::string filename) const
	//{
	//	std::fstream file;       // 文件流，用于图像文件操作
	//	if(filename=="")                       // 防止文件名为空
	//	{
	//		return NULL;
	//	}
	//	file.open(filename.c_str(), std::ios::in);  // 以只读方式打开文件
	//	if (file)                              // 若打开成功
	//		file.close();                          // 关闭文件流
	//	// 返回图像文件的指针
	//	return auxDIBImageLoad(filename.c_str());  
	//	return NULL;                           // 如果前面操作失败返回NULL
	//}

	void VrmlGame::detect_corners(std::string &markerdata, std::vector<std::vector<Wml::Vector2d> > &corners)
	{
		bool ret = g_markerkeybase.SetFile(markerdata);

		if (!ret)
		{
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		/// \brief Generated features.
		//////////////////////////////////////////////////////////////////////////
		std::vector<FeatureSpace::ArtificialFeature> _features;

		//////////////////////////////////////////////////////////////////////////
		/// \brief Found, <feature index, key index>
		//////////////////////////////////////////////////////////////////////////
		std::map<int, int> _featurekey;

		corners.resize(mpMap->vpKeyFrames.size());
		int i = 0;
		vector<KeyFrame *>::iterator kf;
		for( kf = mpMap->vpKeyFrames.begin(); kf != mpMap->vpKeyFrames.end(); kf++ )
		{
			corners[i].clear();

			CSimpleImageb simage;

			CVD::Image<CVD::byte> tempimage = (*kf)->aLevels[0].im;
			int height,width;
			height = tempimage.size().y;
			width = tempimage.size().x;
			//convert coordinate
			unsigned char* tempdata = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
			for (int j=0; j<height; j++)
			{
				memcpy(tempdata+width*(height-1-j),(CVD::byte*)tempimage.data()+width*j,width*sizeof(unsigned char));
			}

			simage.set_data(tempdata, width, height, 1);
			free(tempdata);
			Pattern::CSimpleMarkerDetector smd;
			std::vector<pattern_parameter<MARKER_FLOAT> > patterns;

			bool ret = smd.Fit(simage, patterns);


			//////////////////////////////////////////////////////////////////////////
			std::vector<Pattern::RectHomography<MARKER_FLOAT> > recthomos;
			recthomos.resize(patterns.size());


			// use the pattern to extract the image features.
			std::vector<pattern_parameter<MARKER_FLOAT> >::iterator it = patterns.begin();

			for (int p = 0; it != patterns.end(); ++ it, ++ p)
			{
				Pattern::RectHomography<MARKER_FLOAT> &obj = recthomos[p];

				obj.corners = it->corners;
				assert(obj.corners.size() == 4);  
			}

			//////////////////////////////////////////////////////////////////////////
			CSimpleImagef grey_image;
			SimpleImage::grayscale(simage, grey_image);

			SimpleImage::flip(grey_image);


			// convert the marker region image to feature description.
			CArtificialFeatureMatch::Pattern2Feature(grey_image, recthomos, _features);

			//////////////////////////////////////////////////////////////////////////
			CArtificialFeatureMatch::SearchForFeature(_features, g_markerkeybase, _featurekey);


			//////////////////////////////////////////////////////////////////////////
			for(int c = 0; c < _features.size(); ++c)
			{
				if (_features[c]._state == FeatureSpace::CALIBRATED)
				{
					assert (_features[c]._corners.size() == 4);

					for(int p = 0; p < _features[c]._corners.size(); ++p)
					{
						corners[i].push_back(Wml::Vector2d(_features[c]._corners[p].x2, _features[c]._corners[p].y2));
						std::cout<<_features[c]._corners[p].x2<<","<<_features[c]._corners[p].y2<<std::endl;
					}
					std::cout<<i<<" image marker found\n";
				}
				break;
			}
			i++;
		}
	}

	void VrmlGame::Init()
	{
		if(mbInitialised) return;
		mbInitialised = true;

		Reset();
		std::cout<<"VrmlGame Init"<<std::endl;
	}

	void VrmlGame::Reset()
	{

	}
}