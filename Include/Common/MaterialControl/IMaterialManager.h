#pragma once
#include "Common/MaterialControl/IShaderProgram.h"
#include <map>
#include <string>
#include <memory>
namespace MaterialControl
{ 
	/// managing factory class that allocates and returns shader materials
	class IMaterialManager
	{
  protected:
		std::map<std::string, std::shared_ptr<IShaderProgram> > m_materials;

		// helper function
		virtual std::shared_ptr<IShaderProgram> GetMaterial(const std::string& a_programName, const std::string& a_vertShPath, const std::string& a_fragShPath,
			const std::string& a_geometryShader = "" ) = 0;

	public:

		// utility materials
		virtual std::shared_ptr<IShaderProgram> GetSingleTextureMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		virtual std::shared_ptr<IShaderProgram> GetAddBlendMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;

		// deferred shading - geometry passes
		virtual std::shared_ptr<IShaderProgram> GetGeometryPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		virtual std::shared_ptr<IShaderProgram> GetColourGeometryPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		virtual std::shared_ptr<IShaderProgram> GetEmissiveGeometryPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		virtual std::shared_ptr<IShaderProgram> GetColourNormalGeometryPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		virtual std::shared_ptr<IShaderProgram> GetColourNormalSpecGeometryPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		virtual std::shared_ptr<IShaderProgram> GetColourNormalSpecHardnessGeometryPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		virtual std::shared_ptr<IShaderProgram> GetColourNormalSpecHardnessEmissionGeometryPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;

		// sky cube
		virtual std::shared_ptr<IShaderProgram> GetSkyCubeMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;


		/// used for mesh lights ( point/spot lights etc...)
		virtual std::shared_ptr<IShaderProgram> GetStencilLightPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		/// used for mesh light ( point lights etc...)
		virtual std::shared_ptr<IShaderProgram> GetPointLightPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;
		/// used for mesh light ( point lights etc...)
		virtual std::shared_ptr<IShaderProgram> GetSpotLightPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;

		/// used for directional lights
		virtual std::shared_ptr<IShaderProgram> GetFullScreenLightPassMaterial(const std::string& a_vertShPath, const std::string& a_fragShPath) = 0;

	};

}
