import sys
import os

# if you work on a x86 machine you may want to erase "TARGET_ARCH='x86'"
env=Environment( ENV = {'PATH' : os.environ['PATH']}, TARGET_ARCH='x86' )

# here we add the include folders
env.Append(CCFLAGS='/DWIN32 /Zi /Od /Gm /EHsc /MTd /W4 /DDEBUG -D_WIN32_WINNT=0x0601 /I. /I"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include" /I"../Include" /I"../Impt/"')
# remember to change this for the various projects to the name you want
env.Append(LINKFLAGS=' /DEBUG  /PDB:ServerClientTest.pdb')
# in here we add the library folders
env.Append(LIBPATH='C:\Program Files (x86)\Microsoft SDKs\Windows\\v7.0A\lib;C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib;../Bin/Debug;../Impt/assimp/lib;../Impt/freeimage/lib;../Impt/gl/lib')
# in this list we add all the libraries we want to link to our code seperated by space
list1=Split(' kernel32.lib user32.lib gdi32.lib glew32.lib opengl32.lib assimp.lib FreeImage.lib FreeImage.lib glew32.lib')
env.Append(LIBS=list1)



# in this list we add all the cpp files we need to compile seperated by space
list=Split('../Src/Common/Core/MatrixStack.cpp ../Src/Common/Core/HighResolutionTimer.cpp ../Src/Common/Core/MyUtilities.cpp \
../Src/Common/Network/NetworkMsg.cpp \
../Src/Common/Shapes/ACone.cpp ../Src/Common/Shapes/ACube.cpp ../Src/Common/Shapes/ARect.cpp ../Src/Common/Shapes/ASkybox.cpp ../Src/Common/Shapes/ASphere.cpp ../Src/Common/Shapes/AOpenAssetImportMesh.cpp \
../Src/Common/Textures/ACubemap.cpp ../Src/Common/Textures/ATexture.cpp \
../Src/Common/RenderControl/ADeferredShadingPass.cpp ../Src/Common/RenderControl/ACompositionPass.cpp ../Src/Common/RenderControl/Camera.cpp ../Src/Common/RenderControl/RenderPassPipeline.cpp \
../Src/Common/SceneControl/SceneManager.cpp ../Src/Common/SceneControl/SceneNode.cpp ../Src/Common/SceneControl/CameraSceneNode.cpp ../Src/Common/SceneControl/MeshSceneNode.cpp ../Src/Common/SceneControl/DirectionalLightSceneNode.cpp ../Src/Common/SceneControl/PointLightSceneNode.cpp ../Src/Common/SceneControl/SpotLightSceneNode.cpp \
\
../Src/OpenGL/Core/VertexBufferObject.cpp ../Src/OpenGL/Core/VertexBufferObjectIndexed.cpp \
../Src/OpenGL/MaterialControl/GLMaterialManager.cpp ../Src/OpenGL/MaterialControl/GLShaderProgram.cpp \
../Src/OpenGL/RenderControl/GLDeferredShadingPass.cpp ../Src/OpenGL/RenderControl/GLCompositionPass.cpp \
../Src/OpenGL/Textures/GLTextureFactory.cpp ../Src/OpenGL/Textures/GLTexture.cpp ../Src/OpenGL/Textures/GLCubemap.cpp \
../Src/OpenGL/Shapes/GLShapeFactory.cpp ../Src/OpenGL/Shapes/GLRect.cpp ../Src/OpenGL/Shapes/GLSphere.cpp ../Src/OpenGL/Shapes/GLCone.cpp ../Src/OpenGL/Shapes/GLCube.cpp ../Src/OpenGL/Shapes/GLSkybox.cpp ../Src/OpenGL/Shapes/GLOpenAssetImportMesh.cpp \
../Src/OpenGL/GLGraphicsEngine.cpp \
\
ServerClientTest.cpp ../Src/Server/ServerControl.cpp ../Src/Client/ClientControl.cpp')

# this creates a Library(lib)
#could use env.SharedLibrary(...) to create shared library(dll+lib) - when use this option remember to have exported all the symbols
#could use env.Program(...) to create executable(exe)
ExampleLib = env.Program( 'ServerClientTest',source=list)


# install the example folder to the PathToInstall
PathToInstall = '..\\Bin\\Debug'
env.Install(PathToInstall, ExampleLib )

env.Alias('install', PathToInstall)

env['ENV']['TMP'] = os.environ['TMP']