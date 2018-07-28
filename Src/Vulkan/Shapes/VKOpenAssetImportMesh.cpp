/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKOpenAssetImportMesh.cpp
 *   DATE: Fri Jul 13 19:30:00 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Shapes/VKOpenAssetImportMesh.h"
#include <assert.h>


#include <windows.h>
VKOpenAssetImportMesh::MeshEntry::MeshEntry(std::shared_ptr<VulkanMemory> a_memory) : m_memory(a_memory)
{   
  
  NumIndices  = 0;
  MaterialIndex = INVALID_MATERIAL;
};

VKOpenAssetImportMesh::MeshEntry::~MeshEntry()
{
    // // // // // // // // // if (vbo != INVALID_OVK_VALUE)
        // // // // // // // // // VKDeleteBuffers(1, &vbo);

    // // // // // // // // // if (ibo != INVALID_OVK_VALUE)
        // // // // // // // // // VKDeleteBuffers(1, &ibo);
    
}

void VKOpenAssetImportMesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                          const std::vector<unsigned int>& Indices)
{
  NumIndices = Indices.size();

  // upload data to the gpu
  m_vkVertices = m_memory->CreateVertexBuffer( (char*)Vertices.data(), Vertices.size() * sizeof(Vertex) );
  m_vkIndices = m_memory->CreateIndexBuffer( (char*)Indices.data(), Indices.size() * sizeof(unsigned int) );
  
	// VKGenBuffers(1, &vbo);
  	// VKBindBuffer(VK_ARRAY_BUFFER, vbo);
	// VKBufferData(VK_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], VK_STATIC_DRAW);

  // VKGenBuffers(1, &ibo);
  // VKBindBuffer(VK_ELEMENT_ARRAY_BUFFER, ibo);
  // VKBufferData(VK_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], VK_STATIC_DRAW);
}





VKOpenAssetImportMesh::VKOpenAssetImportMesh( const std::shared_ptr<VulkanMemory>& a_memory ) : AOpenAssetImportMesh(), m_memory(a_memory), m_vulkanShape( nullptr ){}

/*
 *  Method: VKOpenAssetImportMesh::Load
 *  Params: const std::string &Filename
 * Returns: bool
 * Effects: 
 */
bool
VKOpenAssetImportMesh::Load(const std::string &Filename)
{
  // Release the previously loaded mesh (if it exists)
  Release();
  
  bool Ret = false;
  Assimp::Importer Importer;

  const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (pScene) {
    Ret = InitFromScene(pScene, Filename);
  }
  else {
    MessageBox(NULL, Importer.GetErrorString(), "Error loading mesh model", MB_ICONHAND);
  }

  return Ret;
}

std::shared_ptr<ITexture> VKOpenAssetImportMesh::GetTexture()
{
  if (1 < m_Textures.size() && m_Textures[1]) 
		return  m_Textures[1];

	return nullptr;
}
void VKOpenAssetImportMesh::Clear()
{
    //for (unsigned int i = 0 ; i < m_Textures.size() ; i++) {
    //   SAFE_DELETE(m_Textures[i]);
    //}
	// // // // // // // // // VKDeleteVertexArrays(1, &m_vao);
  m_Entries.clear();
  m_Textures.clear();
}

/*
 *  Method: VKOpenAssetImportMesh::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKOpenAssetImportMesh::Create()
{
}


/*
 *  Method: VKOpenAssetImportMesh::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKOpenAssetImportMesh::Render()
{
  // // // // // // // // // // // // // // // // VKBindVertexArray(m_vao);

  // // // // // // // // // // // // // // // // for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
    // // // // // // // // // // // // // // // // VKEnableVertexAttribArray(0);
    // // // // // // // // // // // // // // // // VKEnableVertexAttribArray(1);
    // // // // // // // // // // // // // // // // VKEnableVertexAttribArray(2);
    // // // // // // // // // // // // // // // // VKEnableVertexAttribArray(3);
    // // // // // // // // // // // // // // // // VKBindBuffer(VK_ARRAY_BUFFER, m_Entries[i].vbo);
    // // // // // // // // // // // // // // // // VKVertexAttribPointer(0, 3, VK_FLOAT, VK_FALSE, sizeof(Vertex), 0);
    // // // // // // // // // // // // // // // // VKVertexAttribPointer(1, 2, VK_FLOAT, VK_FALSE, sizeof(Vertex), (const VKvoid*)12);
    // // // // // // // // // // // // // // // // VKVertexAttribPointer(2, 3, VK_FLOAT, VK_FALSE, sizeof(Vertex), (const VKvoid*)20);
    // // // // // // // // // // // // // // // // VKVertexAttribPointer(3, 3, VK_FLOAT, VK_FALSE, sizeof(Vertex), (const VKvoid*)32);

    // // // // // // // // // // // // // // // // VKBindBuffer(VK_ELEMENT_ARRAY_BUFFER, m_Entries[i].ibo);


    // // // // // // // // // // // // // // // // VKDrawElements(VK_TRIANVKES, m_Entries[i].NumIndices, VK_UNSIGNED_INT, 0);
    // // // // // // // // // // // // // // // // VKDisableVertexAttribArray(0);
    // // // // // // // // // // // // // // // // VKDisableVertexAttribArray(1);
    // // // // // // // // // // // // // // // // VKDisableVertexAttribArray(2);
    // // // // // // // // // // // // // // // // VKDisableVertexAttribArray(3);
  // // // // // // // // // // // // // // // // }

}


/*
 *  Method: VKOpenAssetImportMesh::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKOpenAssetImportMesh::Release()
{
  // // // // // // // // // // // // // // // // if( m_vao != 0)
  // // // // // // // // // // // // // // // // {
    // // // // // // // // // // // // // // // // VKDeleteVertexArrays(1, &m_vao);
    // // // // // // // // // // // // // // // // m_vao = 0;
  // // // // // // // // // // // // // // // // }
  if( m_vulkanShape )
    delete m_vulkanShape;
  
}

bool VKOpenAssetImportMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
  m_Entries.resize(pScene->mNumMeshes, {m_memory});
  m_Textures.resize(pScene->mNumMaterials);

  // // // // // // // // // // // // // // // // // // VKGenVertexArrays(1, &m_vao); 
  // // // // // // // // // // // // // // // // // // VKBindVertexArray(m_vao);


  // Initialize the meshes in the scene one by one
  for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
    const aiMesh* paiMesh = pScene->mMeshes[i];
    InitMesh(i, paiMesh);
  }
  // init shape
  
  std::vector<std::shared_ptr<VulkanMemoryChunk> > l_vkVertices;
  std::vector<std::shared_ptr<VulkanMemoryChunk> > l_vkIndices;
  std::vector<unsigned int> l_indicesCount;
  for (unsigned int i = 0 ; i < m_Entries.size() ; i++)
  {
    l_vkVertices.push_back(m_Entries[i].m_vkVertices);
    l_vkIndices.push_back(m_Entries[i].m_vkIndices);
    l_indicesCount.push_back(m_Entries[i].NumIndices);
  }
  m_vulkanShape = new VKShape(m_memory, l_vkVertices, l_vkIndices, l_indicesCount);

  return InitMaterials(pScene, Filename);
}
void VKOpenAssetImportMesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
  m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

  std::vector<Vertex> Vertices;
  std::vector<unsigned int> Indices;

  const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

  for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
    const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
    const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
    const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
    const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;

    Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
             glm::vec2(pTexCoord->x, 1.0f-pTexCoord->y),
             glm::vec3(pNormal->x, pNormal->y, pNormal->z),
     glm::vec3(pTangent->x, pTangent->y, pTangent->z)
    );

    Vertices.push_back(v);
  }

  for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
    const aiFace& Face = paiMesh->mFaces[i];
    assert(Face.mNumIndices == 3);
    Indices.push_back(Face.mIndices[0]);
    Indices.push_back(Face.mIndices[1]);
    Indices.push_back(Face.mIndices[2]);
  }

  m_Entries[Index].Init(Vertices, Indices);
}

bool VKOpenAssetImportMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
  // Extract the directory part from the file name
  std::string::size_type SlashIndex = Filename.find_last_of("\\");
  std::string Dir;

  if (SlashIndex == std::string::npos) {
      Dir = ".";
  }
  else if (SlashIndex == 0) {
      Dir = "\\";
  }
  else {
      Dir = Filename.substr(0, SlashIndex);
  }

  bool Ret = true;

  // Initialize the materials
  for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
      const aiMaterial* pMaterial = pScene->mMaterials[i];

      m_Textures[i] = NULL;

      if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
          aiString Path;

        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
        {
          std::string FullPath = Dir + "\\" + Path.data;
          m_Textures[i] = std::make_shared<VKTexture>(m_memory);
          if (!m_Textures[i]->Load(FullPath, true)) {
            MessageBox(NULL, FullPath.c_str(), "Error loading mesh texture", MB_ICONHAND);
            // delete m_Textures[i];
            m_Textures[i] = NULL;
            Ret = false;
          }
          else {
              printf("Loaded texture '%s'\n", FullPath.c_str());
          }
        }
      }

      // Load a sinVKe colour texture matching the diffuse colour if no texture added
      if (!m_Textures[i]) {
  
        aiColor3D color (0.f,0.f,0.f);
        pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE,color);

        m_Textures[i] = std::make_shared<VKTexture>(m_memory);
        char data[4];
        data[0] = (char) (color[2]*255);
        data[1] = (char) (color[1]*255);
        data[2] = (char) (color[0]*255);
        data[3] = (char)254;
        m_Textures[i]->CreateFromData(data, 1, 1, 32, false);

      }
  }

  return Ret;
}

