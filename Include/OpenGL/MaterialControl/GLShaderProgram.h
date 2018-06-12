#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include "Common/MaterialControl/IShaderProgram.h"

// An interface that provides a wrapper around an shader and allows multiple implementations
class GLShader : public IShader
{
public:
  GLShader();

	virtual bool LoadShader(std::string sFile, int iType);
	virtual void DeleteShader();

	virtual bool GetLinesFromFile(std::string sFile, bool bIncludePart, std::vector<std::string>* vResult);

	virtual bool IsLoaded();
	virtual unsigned int GetShaderID();


private:
	unsigned int m_uiShader; // ID of shader
	int m_iType; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool m_bLoaded; // Whether shader was loaded and compiled
};


// A class the provides a wrapper around an OpenGL shader program
class GLShaderProgram : public IShaderProgram
{
public:
  GLShaderProgram();
	virtual void CreateProgram();
	virtual void DeleteProgram();

	virtual bool AddShaderToProgram(IShader* shShader);
	virtual bool LinkProgram();

	virtual void UseProgram();

	virtual unsigned int GetProgramID();

	// Setting vectors
	virtual void SetUniform(std::string sName, glm::vec2* vVectors, int iCount = 1);
	virtual void SetUniform(std::string sName, const glm::vec2 vVector);
	virtual void SetUniform(std::string sName, glm::vec3* vVectors, int iCount = 1);
	virtual void SetUniform(std::string sName, const glm::vec3 vVector);
	virtual void SetUniform(std::string sName, glm::vec4* vVectors, int iCount = 1);
	virtual void SetUniform(std::string sName, const glm::vec4 vVector);

	// Setting floats
	virtual void SetUniform(std::string sName, float* fValues, int iCount = 1);
	virtual void SetUniform(std::string sName, const float fValue);

	// Setting 3x3 matrices
	virtual void SetUniform(std::string sName, glm::mat3* mMatrices, int iCount = 1);
	virtual void SetUniform(std::string sName, const glm::mat3 mMatrix);

	// Setting 4x4 matrices
	virtual void SetUniform(std::string sName, glm::mat4* mMatrices, int iCount = 1);
	virtual void SetUniform(std::string sName, const glm::mat4 mMatrix);

	// Setting integers
	virtual void SetUniform(std::string sName, int* iValues, int iCount = 1);
	virtual void SetUniform(std::string sName, const int iValue);

private:
  unsigned int m_uiProgram; // ID of program
	bool m_bLinked; // Whether program was linked and is ready to use
};