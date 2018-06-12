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


#include "Common/Shapes/AOpenAssetImportMesh.h"
#include "OpenGL/Textures/GLTexture.h"


class GLOpenAssetImportMesh : public AOpenAssetImportMesh
{
public:

  ~GLOpenAssetImportMesh(){ Clear(); }  
  virtual bool Load(const std::string& Filename);
  

	virtual std::shared_ptr<ITexture> GetTexture();

  virtual void Create();
  virtual void Render();
	virtual void Release();
  void Clear();
  
private:
  bool InitFromScene(const aiScene* pScene, const std::string& Filename);
  void InitMesh(unsigned int Index, const aiMesh* paiMesh);
  bool InitMaterials(const aiScene* pScene, const std::string& Filename);



#define INVALID_MATERIAL 0xFFFFFFFF

  struct MeshEntry {
    MeshEntry();

    ~MeshEntry();

    void Init(const std::vector<Vertex>& Vertices,
              const std::vector<unsigned int>& Indices);
    unsigned int vbo;
    unsigned int ibo;
    unsigned int NumIndices;
    unsigned int MaterialIndex;
  };

  std::vector<MeshEntry> m_Entries;
  std::vector<std::shared_ptr<GLTexture> > m_Textures;
  unsigned int m_vao;
};



