/*

	Copyright 2011 Etay Meiri

	// Modified by Dr Greg Slabaugh to work with OpenGL template
	// Modified by Stavros Bizelis for his Dissertation with the name "Network Distributed Deferred Shading" in City, University of London

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <map>
#include <vector>
#include "gl/include/glew.h"
#include <assimp/include/Importer.hpp>      // C++ importer interface
#include <assimp/include/scene.h>       // Output data structure
#include <assimp/include/PostProcess.h> // Post processing flags

#include <string.h>
#include "Common/Textures/ITexture.h"
#include <memory>
#define INVALID_OGL_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#include "Common/Shapes/IMesh.h"
#include "Common/Textures/ITexture.h"



class AOpenAssetImportMesh : public IMesh
{
public:
  virtual bool Load(const std::string& Filename) = 0;
  virtual void Create();
  
	virtual std::shared_ptr<ITexture> GetTexture() = 0;

};



