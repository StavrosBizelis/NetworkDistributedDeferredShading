import sys
import os

# if you work on a x86 machine you may want to erase "TARGET_ARCH='x86'"
env=Environment( ENV = {'PATH' : os.environ['PATH']}, TARGET_ARCH='x86' )

# here we add the include folders
env.Append(CCFLAGS='/DWIN32 /Zi /Gm /GR /EHsc /DNDEBUG /MT /O2 -D_WIN32_WINNT=0x0601 /I. /I"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include" /I"C:\\VulkanSDK\\1.0.57.0\\Include" /I"../Include" /I"../Impt/"')
# remember to change this for the various projects to the name you want
env.Append(LINKFLAGS=' /PDB:NDDSClient.pdb')
# in here we add the library folders
env.Append(LIBPATH='C:\Program Files (x86)\Microsoft SDKs\Windows\\v7.0A\lib;C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib;C:\\VulkanSDK\\1.0.57.0\\Lib32;../Bin/Debug;../Impt/assimp/lib;../Impt/freeimage/lib;../Impt/gl/lib;')
# in this list we add all the libraries we want to link to our code seperated by space
list1=Split(' kernel32.lib User32.lib gdi32.lib glew32.lib opengl32.lib assimp.lib FreeImage.lib vulkan-1.lib')
env.Append(LIBS=list1)



# in this list we add all the cpp files we need to compile seperated by space
list=Split('Common/Core/MatrixStack.cpp Common/Core/HighResolutionTimer.cpp Common/Core/MyUtilities.cpp Common/Core/GameWindow.cpp \
Common/Network/NetworkMsg.cpp \
Common/Shapes/ACone.cpp Common/Shapes/ACube.cpp Common/Shapes/ARect.cpp Common/Shapes/ASkybox.cpp Common/Shapes/ASphere.cpp Common/Shapes/AOpenAssetImportMesh.cpp \
Common/Textures/ACubemap.cpp Common/Textures/ATexture.cpp \
Common/RenderControl/ADeferredShadingPass.cpp Common/RenderControl/ACompositionPass.cpp Common/RenderControl/Camera.cpp Common/RenderControl/RenderPassPipeline.cpp \
Common/SceneControl/SceneManager.cpp Common/SceneControl/SceneNode.cpp Common/SceneControl/CameraSceneNode.cpp Common/SceneControl/MeshSceneNode.cpp Common/SceneControl/DirectionalLightSceneNode.cpp Common/SceneControl/PointLightSceneNode.cpp Common/SceneControl/SpotLightSceneNode.cpp \
\
OpenGL/Core/VertexBufferObject.cpp OpenGL/Core/VertexBufferObjectIndexed.cpp \
OpenGL/MaterialControl/GLMaterialManager.cpp OpenGL/MaterialControl/GLShaderProgram.cpp \
OpenGL/RenderControl/GLDeferredShadingPass.cpp OpenGL/RenderControl/GLCompositionPass.cpp \
OpenGL/Textures/GLTextureFactory.cpp OpenGL/Textures/GLTexture.cpp OpenGL/Textures/GLCubemap.cpp \
OpenGL/Shapes/GLShapeFactory.cpp OpenGL/Shapes/GLRect.cpp OpenGL/Shapes/GLSphere.cpp OpenGL/Shapes/GLCone.cpp OpenGL/Shapes/GLCube.cpp OpenGL/Shapes/GLSkybox.cpp OpenGL/Shapes/GLOpenAssetImportMesh.cpp \
OpenGL/GLGraphicsEngine.cpp \
\
Vulkan/Core/VulkanDriver.cpp Vulkan/Core/VulkanMemory.cpp Vulkan/Core/VulkanLogicalDeviceManager.cpp  Vulkan/Core/VulkanSwapChainDetails.cpp Vulkan/Core/VulkanPhysicalDeviceSelector.cpp Vulkan/Core/VulkanUtilities.cpp \
Vulkan/RenderControl/VKCompositionPass.cpp Vulkan/RenderControl/VKDeferredShadingPass.cpp \
Vulkan/RenderControl/Pipelines/VKPipeline.cpp Vulkan/RenderControl/Pipelines/VKDeferredPipelines.cpp Vulkan/RenderControl/Pipelines/VulkanPrimaryCommandBuffer.cpp Vulkan/RenderControl/Pipelines/VulkanSecondaryCommandBuffer.cpp \
Vulkan/RenderControl/Pipelines/VKCompositionPipeline.cpp \
Vulkan/Textures/VKTextureFactory.cpp Vulkan/Textures/VKTexture.cpp Vulkan/Textures/VKCubemap.cpp \
Vulkan/Shapes/VKShapeFactory.cpp Vulkan/Shapes/VKRect.cpp Vulkan/Shapes/VKSphere.cpp Vulkan/Shapes/VKCone.cpp Vulkan/Shapes/VKCube.cpp Vulkan/Shapes/VKSkybox.cpp Vulkan/Shapes/VKOpenAssetImportMesh.cpp \
Vulkan/SceneControl/VKSceneManager.cpp Vulkan/SceneControl/VKPointLightSceneNode.cpp Vulkan/SceneControl/VKMeshSceneNode.cpp Vulkan/SceneControl/VKDirectionalLightSceneNode.cpp \
Vulkan/VKGraphicsEngine.cpp \
\
Client/main.cpp Client/ClientApp.cpp Client/ClientControl.cpp  \
\
../Impt/lz4/include/lz4.c')

# this creates a Library(lib)
#could use env.SharedLibrary(...) to create shared library(dll+lib) - when use this option remember to have exported all the symbols
#could use env.Program(...) to create executable(exe)
ExampleLib = env.Program( 'NDDSClient',source=list)


# install the example folder to the PathToInstall
PathToInstall = '..\\Bin\\Release'
env.Install(PathToInstall, ExampleLib )

env.Alias('install', PathToInstall)

env['ENV']['TMP'] = os.environ['TMP']