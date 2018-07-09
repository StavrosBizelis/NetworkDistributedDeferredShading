import sys
import os

# if you work on a x86 machine you may want to erase "TARGET_ARCH='x86'"
env=Environment( ENV = {'PATH' : os.environ['PATH']}, TARGET_ARCH='x86' )

# here we add the include folders
env.Append(CCFLAGS='/DWIN32 /Zi /Od /Gm /EHsc /MTd /W4 /DDEBUG -D_WIN32_WINNT=0x0601 /I. /I"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include" /I"../Include" /I"../Impt/"')
# remember to change this for the various projects to the name you want
env.Append(LINKFLAGS=' /DEBUG  /PDB:UpdateNetworkMsgTest.pdb')
# in here we add the library folders
env.Append(LIBPATH='C:\Program Files (x86)\Microsoft SDKs\Windows\\v7.0A\lib;C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib;../Bin/Debug;../Impt/assimp/lib;../Impt/freeimage/lib;../Impt/gl/lib')
# in this list we add all the libraries we want to link to our code seperated by space
list1=Split(' kernel32.lib user32.lib gdi32.lib glew32.lib opengl32.lib assimp.lib FreeImage.lib FreeImage.lib glew32.lib')
env.Append(LIBS=list1)



# in this list we add all the cpp files we need to compile seperated by space
list=Split('../Src/Common/Network/NetworkMsg.cpp ../Src/Common/Core/MyUtilities.cpp UpdateNetworkMsgTest.cpp \
../Impt/lz4/include/lz4.c')

# this creates a Library(lib)
#could use env.SharedLibrary(...) to create shared library(dll+lib) - when use this option remember to have exported all the symbols
#could use env.Program(...) to create executable(exe)
ExampleLib = env.Program( 'UpdateNetworkMsgTest',source=list)


# install the example folder to the PathToInstall
PathToInstall = '..\\Bin\\Debug'
env.Install(PathToInstall, ExampleLib )

env.Alias('install', PathToInstall)

env['ENV']['TMP'] = os.environ['TMP']