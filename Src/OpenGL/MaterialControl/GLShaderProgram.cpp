/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: GLShaderProgram.cpp
 *   DATE: Mon Jun 11 15:35:46 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/MaterialControl/GLShaderProgram.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
#include <windows.h>
#include <sstream>
GLShader::GLShader() : m_bLoaded( false ) {}

/*
 *  Method: GLShader::LoadShader
 *  Params: string sFile, int iType
 * Returns: bool
 * Effects: 
 */
bool
GLShader::LoadShader(std::string sFile, int iType)
{
  	std::vector<std::string> sLines;

	if(!GetLinesFromFile(sFile, false, &sLines)) {
		char message[1024];
		sprintf_s(message, "Cannot load shader\n%s\n", sFile.c_str());
		MessageBox(NULL, message, "Error", MB_ICONERROR);
		return false;
	}

	const char** sProgram = new const char*[(int)sLines.size()];
	for (int i = 0; i < (int)sLines.size(); i++) 
		sProgram[i] = sLines[i].c_str();
	
	m_uiShader = glCreateShader(iType);

	glShaderSource(m_uiShader, (int)sLines.size(), sProgram, NULL);
	glCompileShader(m_uiShader);

	delete[] sProgram;

	int iCompilationStatus;
	glGetShaderiv(m_uiShader, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)
	{
		char sInfoLog[1024];
		char sFinalMessage[1536];
		int iLogLength;
		glGetShaderInfoLog(m_uiShader, 1024, &iLogLength, sInfoLog);
		char sShaderType[64];
		if (iType == GL_VERTEX_SHADER)
			sprintf_s(sShaderType, "vertex shader");
		else if (iType == GL_FRAGMENT_SHADER)
			sprintf_s(sShaderType, "fragment shader");
		else if (iType == GL_GEOMETRY_SHADER)
			sprintf_s(sShaderType, "geometry shader");
		else if (iType == GL_TESS_CONTROL_SHADER)
			sprintf_s(sShaderType, "tesselation control shader");
		else if (iType == GL_TESS_EVALUATION_SHADER)
			sprintf_s(sShaderType, "tesselation evaluation shader");
		else
			sprintf_s(sShaderType, "unknown shader type");

		sprintf_s(sFinalMessage, "Error in %s!\n%s\nShader file not compiled.  The compiler returned:\n\n%s", sShaderType, sFile.c_str(), sInfoLog);

		MessageBox(NULL, sFinalMessage, "Error", MB_ICONERROR);
		return false;
	}
	m_iType = iType;
	m_bLoaded = true;

	return true;
}


/*
 *  Method: GLShader::DeleteShader
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLShader::DeleteShader()
{
  if(!IsLoaded())
		return;
	m_bLoaded = false;
	glDeleteShader(m_uiShader);
}


/*
 *  Method: GLShader::GetLinesFromFile
 *  Params: string sFile, bool bIncludePart, vector<string> *vResult
 * Returns: bool
 * Effects: 
 */
bool
GLShader::GetLinesFromFile(std::string sFile, bool bIncludePart, std::vector<std::string> *vResult)
{
  FILE* fp;
	fopen_s(&fp, sFile.c_str(), "rt");
	if(!fp)return false;

	std::string sDirectory;
	int slashIndex = -1;

	for (int i = (int)sFile.size()-1; i == 0; i--)
	{
		if(sFile[i] == '\\' || sFile[i] == '/')
		{
			slashIndex = i;
			break;
		}
	}

	sDirectory = sFile.substr(0, slashIndex+1);

	// Get all lines from a file

	char sLine[255];

	bool bInIncludePart = false;

	while(fgets(sLine, 255, fp))
	{
		std::stringstream ss(sLine);
		std::string sFirst;
		ss >> sFirst;
		if(sFirst == "#include")
		{
			std::string sFileName;
			ss >> sFileName;
			if((int)sFileName.size() > 0 && sFileName[0] == '\"' && sFileName[(int)sFileName.size()-1] == '\"')
			{
				sFileName = sFileName.substr(1, (int)sFileName.size()-2);
				GetLinesFromFile(sDirectory+sFileName, true, vResult);
			}
		}
		else if(sFirst == "#include_part")
			bInIncludePart = true;
		else if(sFirst == "#definition_part")
			bInIncludePart = false;
		else if(!bIncludePart || (bIncludePart && bInIncludePart))
			vResult->push_back(sLine);
	}
	fclose(fp);

	return true;
}


/*
 *  Method: GLShader::IsLoaded
 *  Params: 
 * Returns: bool
 * Effects: 
 */
bool
GLShader::IsLoaded()
{
  return m_bLoaded;
}


/*
 *  Method: GLShader::GetShaderID
 *  Params: 
 * Returns: unsigned int
 * Effects: 
 */
unsigned int
GLShader::GetShaderID()
{
  return m_uiShader;
}


GLShaderProgram::GLShaderProgram() : m_bLinked(false) {}

/*
 *  Method: GLShaderProgram::CreateProgram
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::CreateProgram()
{
  m_uiProgram = glCreateProgram();
}


/*
 *  Method: GLShaderProgram::DeleteProgram
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::DeleteProgram()
{
  if(!m_bLinked)  
		return;
	m_bLinked = false;
	glDeleteProgram(m_uiProgram);
}


/*
 *  Method: GLShaderProgram::AddShaderToProgram
 *  Params: CShader *shShader
 * Returns: bool
 * Effects: 
 */
bool
GLShaderProgram::AddShaderToProgram(IShader *shShader)
{
  if(!shShader->IsLoaded())
		return false;

	glAttachShader(m_uiProgram, shShader->GetShaderID());

	return true;
}


/*
 *  Method: GLShaderProgram::LinkProgram
 *  Params: 
 * Returns: bool
 * Effects: 
 */
bool
GLShaderProgram::LinkProgram()
{
	glLinkProgram(m_uiProgram);
	int iLinkStatus;
	glGetProgramiv(m_uiProgram, GL_LINK_STATUS, &iLinkStatus);

	if (iLinkStatus == FALSE) 
	{
		char sInfoLog[1024];
		char sFinalMessage[1536];
		int iLogLength;
		glGetProgramInfoLog(m_uiProgram, 1024, &iLogLength, sInfoLog);
		sprintf_s(sFinalMessage, "Error! Shader program wasn't linked! The linker returned:\n\n%s", sInfoLog);
		MessageBox(NULL, sFinalMessage, "Error", MB_ICONERROR);
		return false;
	}

	m_bLinked = iLinkStatus == GL_TRUE;
	return m_bLinked;
}


/*
 *  Method: GLShaderProgram::UseProgram
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::UseProgram()
{
  if(m_bLinked)
	{
		glUseProgram(m_uiProgram);
		typedef std::map<std::string, std::function<void(IShaderProgram*)> >::iterator Iter;
		for (Iter l_iter = m_onUse.begin(); l_iter != m_onUse.end(); ++l_iter)
			(l_iter->second)(this);
	}
}


/*
 *  Method: GLShaderProgram::GetProgramID
 *  Params: 
 * Returns: unsigned int
 * Effects: 
 */
unsigned int
GLShaderProgram::GetProgramID()
{
  return m_uiProgram;
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, glm::vec2 *vVectors, int iCount
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, glm::vec2 *vVectors, int iCount)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform2fv(iLoc, iCount, (GLfloat*)vVectors);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, const glm::vec2 vVector
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, const glm::vec2 vVector)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, glm::vec3 *vVectors, int iCount
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, glm::vec3 *vVectors, int iCount)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, const glm::vec3 vVector
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, const glm::vec3 vVector)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, glm::vec4 *vVectors, int iCount
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, glm::vec4 *vVectors, int iCount)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform4fv(iLoc, iCount, (GLfloat*)vVectors);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, const glm::vec4 vVector
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, const glm::vec4 vVector)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform4fv(iLoc, 1, (GLfloat*)&vVector);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, float *fValues, int iCount
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, float *fValues, int iCount)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1fv(iLoc, iCount, fValues);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, const float fValue
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, const float fValue)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1fv(iLoc, 1, &fValue);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, glm::mat3 *mMatrices, int iCount
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, glm::mat3 *mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix3fv(iLoc, iCount, FALSE, (GLfloat*)mMatrices);  
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, const glm::mat3 mMatrix
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, const glm::mat3 mMatrix)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix3fv(iLoc, 1, FALSE, (GLfloat*)&mMatrix);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, glm::mat4 *mMatrices, int iCount
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, glm::mat4 *mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix4fv(iLoc, iCount, FALSE, (GLfloat*)mMatrices);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, const glm::mat4 mMatrix
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, const glm::mat4 mMatrix)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix4fv(iLoc, 1, FALSE, (GLfloat*)&mMatrix);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, int *iValues, int iCount
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, int *iValues, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1iv(iLoc, iCount, iValues);
}


/*
 *  Method: GLShaderProgram::SetUniform
 *  Params: string sName, const int iValue
 * Returns: void
 * Effects: 
 */
void
GLShaderProgram::SetUniform(std::string sName, const int iValue)
{
  int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
  glUniform1i(iLoc, iValue);
}


