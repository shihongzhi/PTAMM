#include "GLShader.h"

namespace PTAMM {

#define printOpenGLError() printOglError(__FILE__, __LINE__)

Shader::Shader()
{
	m_VertexShader = NULL;
	m_FragmentShader = NULL;
	m_Program = NULL;
	m_Frag = NULL;
	m_Vert = NULL;
}

Shader::~Shader()
{
	CleanUp();
}
void Shader::CleanUp()
{
	if(NULL != m_VertexShader)
	{
		delete[] m_VertexShader; 
		m_VertexShader = NULL;
	}
	if(NULL != m_FragmentShader)
	{
		delete[] m_FragmentShader;
		m_FragmentShader = NULL;
	}

	//解除shader与program的绑定关系
	glDetachShader(m_Program,m_Vert);
	glDetachShader(m_Program,m_Frag);

	//删除shader和program
	glDeleteObjectARB(m_Vert);
	glDeleteObjectARB(m_Frag);
	glDeleteProgram(m_Program);	
	m_Vert = NULL;	
	m_Frag = NULL;
	m_Program = NULL;
}

bool Shader::TextFileRead(char *_fn, GLchar *&_shader)
{
	FILE *fp;
	int count = 0;

	fp = fopen(_fn,"rt");
	if(NULL == fp)
		return false;

	// 将文件指针指向文件流末尾
	fseek(fp,0,SEEK_END);
	// 计算文件尾相对文件头的偏移量（文件长度）
	count = ftell(fp);
	// 把文件指针重新指向文件流头部
	rewind(fp);

	if(count<=0)
		return false;

	_shader = new GLchar[count + 8];	//+8防止越界
	count = fread(_shader,sizeof(GLchar),count,fp);
	_shader[count] = '\0';
	fclose(fp);
	return true;
}

bool Shader::ReadVertextShader(char *_fn)
{
	if(TextFileRead(_fn,m_VertexShader))
		return true;
	else
		return false;
}

bool Shader::ReadFragmentShader(char *_fn)
{
	if(TextFileRead(_fn,m_FragmentShader))
		return true;
	else
		return false;
}

bool Shader::SetShaderFile(char* sVSFileName, char* sFSFileName)
{
	if (glIsProgram(m_Program))
	{
		CleanUp();
	}
	if (!ReadVertextShader(sVSFileName) || !ReadFragmentShader(sFSFileName))
	{
		return false;
	}
	GLint vertCompiled, fragCompiled;			//状态值
	GLint linked;
	//创建shader对象
	m_Vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	m_Frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	const GLcharARB *vv = m_VertexShader;
	const GLcharARB *ff = m_FragmentShader;
	//添加shader
	glShaderSourceARB(m_Vert,1,&vv,NULL);
	glShaderSourceARB(m_Frag,1,&ff,NULL);

	//编译shader
	glCompileShaderARB(m_Vert);
	printOpenGLError(); //检查OpenGL错误
	//glGetObjectParameterivARB(m_Vert, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
	glGetShaderiv(m_Vert, GL_COMPILE_STATUS, &vertCompiled);
	printInfoLog(m_Vert);
	glCompileShaderARB(m_Frag);
	printOpenGLError(); //检查OpenGL错误
	//glGetObjectParameterivARB(m_Frag, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
	glGetShaderiv(m_Frag, GL_COMPILE_STATUS, &fragCompiled);
	printInfoLog(m_Frag); 

	if (!vertCompiled || !fragCompiled)
		return false;

	//创建程序对象
	m_Program = glCreateProgramObjectARB();

	//绑定shader到程序对象
	glAttachObjectARB(m_Program,m_Vert);
	glAttachObjectARB(m_Program,m_Frag);	

	//链接程序
	glLinkProgramARB(m_Program);
	printOpenGLError(); //检查OpenGL错误
	//glGetObjectParameterivARB(m_Program, GL_OBJECT_COMPILE_STATUS_ARB, &linked);
	glGetProgramiv(m_Program, GL_LINK_STATUS, &linked);  
	printInfoLog(m_Program);

	if (!linked)
		return false; 
	UseShader(true);				
	return true;
}

void Shader::UseShader(bool bOn)
{
	if (!bOn)
	{
		glUseProgramObjectARB(0);
	}
	else
	{
		glUseProgramObjectARB(m_Program);		
	}
}

GLint Shader::getUniLoc(GLuint program, const GLchar *name)
{
	GLint loc;
	loc = glGetUniformLocation(program, name);
	if (loc == -1)
		printf("uniform variates %s undefined! \n", name);
	printOpenGLError(); // Check for OpenGL errors
	return loc;
} 
void Shader::SetUniVar(char* sVarName, float fValue0, float fValue1, float fValue2)
{
	//设置初始一致变量值
	glUniform3fARB(getUniLoc(m_Program, sVarName), fValue0, fValue1, fValue2); 

}
void Shader::SetUniVar(char* sVarName, float fValue0, float fValue1)
{
	//设置初始一致变量值
	glUniform2fARB(getUniLoc(m_Program, sVarName), fValue0, fValue1);
}
void Shader::SetUniVar(char* sVarName, float fValue0)
{
	//设置初始一致变量值
	glUniform1fARB(getUniLoc(m_Program, sVarName), fValue0);
}
void Shader::SetUniVar(char* sVarName, int nValue0)
{
	//设置初始一致变量值
	glUniform1iARB(getUniLoc(m_Program, sVarName), nValue0);
}
void Shader::SetSampler(char* sVarName, int tex)
{
	GLint texSampler;
	texSampler = glGetUniformLocation(m_Program, sVarName);
	glUniform1i(texSampler, tex);
}
//---------------------------------------------------------------------------
/**
* @brief 打印OpenGL错误信息
* @param  file 错误所在的文件
* @param  line 错误所在的行
* @return 1 OpenGL error
* @return 0 other  error
**/
int Shader::printOglError(char *file, int line)
{
	GLenum glErr;
	int retCode = 0;
	glErr = glGetError();///获取错误
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();///获取下一个错误
	}
	return retCode;
}

/*** @brief 打印日志 ***/
void Shader::printInfoLog(GLhandleARB obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;
	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

}
