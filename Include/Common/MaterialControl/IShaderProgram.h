#pragma once

#include "glm/glm.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

// An interface that provides a wrapper around an shader and allows multiple implementations
class IShader
{
public:


	virtual bool LoadShader(std::string sFile, int iType) = 0;
	virtual void DeleteShader() = 0;

	virtual bool GetLinesFromFile(std::string sFile, bool bIncludePart, std::vector<std::string>* vResult) = 0;

	virtual bool IsLoaded() = 0;
	virtual unsigned int GetShaderID() = 0;


private:
	unsigned int m_uiShader; // ID of shader
	int m_iType; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool m_bLoaded; // Whether shader was loaded and compiled
};


// A class the provides a wrapper around an OpenGL shader program
class IShaderProgram
{
public:
	virtual void CreateProgram() = 0;
	virtual void DeleteProgram() = 0;

	virtual bool AddShaderToProgram(IShader* shShader) = 0;
	virtual bool LinkProgram() = 0;

	virtual void UseProgram() = 0;

	virtual unsigned int GetProgramID() = 0;

	// Setting vectors
	virtual void SetUniform(std::string sName, glm::vec2* vVectors, int iCount = 1) = 0;
	virtual void SetUniform(std::string sName, const glm::vec2 vVector) = 0;
	virtual void SetUniform(std::string sName, glm::vec3* vVectors, int iCount = 1) = 0;
	virtual void SetUniform(std::string sName, const glm::vec3 vVector) = 0;
	virtual void SetUniform(std::string sName, glm::vec4* vVectors, int iCount = 1) = 0;
	virtual void SetUniform(std::string sName, const glm::vec4 vVector) = 0;

	// Setting floats
	virtual void SetUniform(std::string sName, float* fValues, int iCount = 1) = 0;
	virtual void SetUniform(std::string sName, const float fValue) = 0;

	// Setting 3x3 matrices
	virtual void SetUniform(std::string sName, glm::mat3* mMatrices, int iCount = 1) = 0;
	virtual void SetUniform(std::string sName, const glm::mat3 mMatrix) = 0;

	// Setting 4x4 matrices
	virtual void SetUniform(std::string sName, glm::mat4* mMatrices, int iCount = 1) = 0;
	virtual void SetUniform(std::string sName, const glm::mat4 mMatrix) = 0;

	// Setting integers
	virtual void SetUniform(std::string sName, int* iValues, int iCount = 1) = 0;
	virtual void SetUniform(std::string sName, const int iValue) = 0;

	void SetOnUse(const std::string& a_funcName, const std::function<void(IShaderProgram*)>& a_func) { m_onUse[a_funcName] = a_func; }
	void RemoveOnUse(const std::string& a_funcName) { m_onUse.erase(a_funcName); }
	bool OnUseExists(const std::string& a_funcName) { return m_onUse.count(a_funcName) == 1; }

protected:
  std::map<std::string, std::function<void(IShaderProgram*)> > m_onUse;
};