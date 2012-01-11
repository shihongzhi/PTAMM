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

	//���shader��program�İ󶨹�ϵ
	glDetachShader(m_Program,m_Vert);
	glDetachShader(m_Program,m_Frag);

	//ɾ��shader��program
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

	// ���ļ�ָ��ָ���ļ���ĩβ
	fseek(fp,0,SEEK_END);
	// �����ļ�β����ļ�ͷ��ƫ�������ļ����ȣ�
	count = ftell(fp);
	// ���ļ�ָ������ָ���ļ���ͷ��
	rewind(fp);

	if(count<=0)
		return false;

	_shader = new GLchar[count + 8];	//+8��ֹԽ��
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
	GLint vertCompiled, fragCompiled;			//״ֵ̬
	GLint linked;
	//����shader����
	m_Vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	m_Frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	const GLcharARB *vv = m_VertexShader;
	const GLcharARB *ff = m_FragmentShader;
	//���shader
	glShaderSourceARB(m_Vert,1,&vv,NULL);
	glShaderSourceARB(m_Frag,1,&ff,NULL);

	//����shader
	glCompileShaderARB(m_Vert);
	printOpenGLError(); //���OpenGL����
	//glGetObjectParameterivARB(m_Vert, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
	glGetShaderiv(m_Vert, GL_COMPILE_STATUS, &vertCompiled);
	printInfoLog(m_Vert);
	glCompileShaderARB(m_Frag);
	printOpenGLError(); //���OpenGL����
	//glGetObjectParameterivARB(m_Frag, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
	glGetShaderiv(m_Frag, GL_COMPILE_STATUS, &fragCompiled);
	printInfoLog(m_Frag); 

	if (!vertCompiled || !fragCompiled)
		return false;

	//�����������
	m_Program = glCreateProgramObjectARB();

	//��shader���������
	glAttachObjectARB(m_Program,m_Vert);
	glAttachObjectARB(m_Program,m_Frag);	

	//���ӳ���
	glLinkProgramARB(m_Program);
	printOpenGLError(); //���OpenGL����
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
	//���ó�ʼһ�±���ֵ
	glUniform3fARB(getUniLoc(m_Program, sVarName), fValue0, fValue1, fValue2); 

}
void Shader::SetUniVar(char* sVarName, float fValue0, float fValue1)
{
	//���ó�ʼһ�±���ֵ
	glUniform2fARB(getUniLoc(m_Program, sVarName), fValue0, fValue1);
}
void Shader::SetUniVar(char* sVarName, float fValue0)
{
	//���ó�ʼһ�±���ֵ
	glUniform1fARB(getUniLoc(m_Program, sVarName), fValue0);
}
void Shader::SetUniVar(char* sVarName, int nValue0)
{
	//���ó�ʼһ�±���ֵ
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
* @brief ��ӡOpenGL������Ϣ
* @param  file �������ڵ��ļ�
* @param  line �������ڵ���
* @return 1 OpenGL error
* @return 0 other  error
**/
int Shader::printOglError(char *file, int line)
{
	GLenum glErr;
	int retCode = 0;
	glErr = glGetError();///��ȡ����
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();///��ȡ��һ������
	}
	return retCode;
}

/*** @brief ��ӡ��־ ***/
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
