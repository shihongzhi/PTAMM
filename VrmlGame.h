#ifndef __VRMLGAME_H
#define __VRMLGAME_H

#include <TooN/TooN.h>
#include "OpenGL.h"
#include "Game.h"
#include <openvrml/vrml97node.h>
#include <openvrml/common.h>
#include <openvrml/browser.h>
#include <openvrml/node.h>
#include <openvrml/node_ptr.h>
#include <openvrml/field.h>
#include "Math/Basic/WmlMath/include/WmlQuaternion.h"
// library
#include "Math/Basic/WmlMath/lib/wmlmathlink.h"

//////////////////////////////////////////////////////////////////////////
// header
#include "motionsolver/Robust3DRecovery.h"

#include "vision/image/simpleimage/include/simpleimage.h"
#include "vision/image/simpleimage/include/simpleimagecolor.h"
#include "vision/image/simpleimage/include/simpleimageoperation.h"

#include "simplemarkerdetector.h"
#include "artificialfeaturematch.h"

#include "utility/floattype.h"
#include "GLShader.h"
#include <string.h>
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#pragma comment( lib, "marker100-win32-vc8-mtd-d.lib" )
#else if
#pragma comment( lib, "marker100-win32-vc8-mtd-r.lib" )
#endif

namespace PTAMM {

	using namespace TooN;

	class Map;
	class KeyFrame;


	class VrmlGame : public Game
	{
	public:
   		VrmlGame( );
		void Draw3D( const GLWindow2 &glWindow, Map &map, SE3<> se3CfromW, GLuint fboId, GLenum *buffers, ATANCamera &mCamera, int statusFlag);
		void DrawMediatorAndObject(int statusFlag);
		void GetMatrixMP(Map &map);
		void GeneratrShadowFBO();
		void SetTextureMatrix();
		void Reset();
		void Init();
		void Draw3DFromVRML(openvrml::node *obj);
		void DrawBox(openvrml::vrml97_node::box_node* vrml_box) const;
		void DrawSphere(openvrml::vrml97_node::sphere_node* vrml_sphere) const;
		void DrawIndexedFaceSet(openvrml::vrml97_node::indexed_face_set_node* vrml_ifs) const;
		void detect_corners(std::string &markerdata, std::vector<std::vector<Wml::Vector2d> > &corners);
		void InitMedMaskTexture();
	private:
		openvrml::browser b;
		Map * mpMap;                    // The associated map
		bool isInitMap;
		bool isBindTexture;
		Wml::Matrix4d mP;    //相机矩阵
		GLuint texture[20];  //最多支持20个纹理
		int textureNum;   //表示现在为第几个纹理
		std::set<std::string> textureUrls;

		Shader phongShadow;   
		GLuint fboShadowId;  //shadow fbo
		GLuint depthTextureId;  //shadow depth texutre
		GLuint medMashTextureId;
	};

}
#endif