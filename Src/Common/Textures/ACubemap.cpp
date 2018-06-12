/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ACubemap.cpp
 *   DATE: Mon Jun 11 15:09:49 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Textures/ACubemap.h"
#include "freeimage\include\FreeImage.h"
/***********************************************************************
 *  Method: ACubemap::ACubemap
 *  Params: 
 * Effects: 
 ***********************************************************************/
ACubemap::ACubemap()
{
}


/***********************************************************************
 *  Method: ACubemap::~ACubemap
 *  Params: 
 * Effects: 
 ***********************************************************************/
ACubemap::~ACubemap()
{
}

/***********************************************************************
 *  Method: ACubemap::Release
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void ACubemap::Release()
{
}


/***********************************************************************
 *  Method: ACubemap::LoadTexture
 *  Params: string filename, char **bmpBytes, int &iWidth, int &iHeight
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool ACubemap::LoadTexture(std::string filename, char **bmpBytes, int &iWidth, int &iHeight)
{
  FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(filename.c_str(), 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(filename.c_str());
	
	if(fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, filename.c_str());

	if(!dib) {
		return false;
	}

	iWidth = FreeImage_GetWidth(dib);
	iHeight = FreeImage_GetWidth(dib);
	int bpp = FreeImage_GetBPP(dib);


	char* bDataPointer = (char*)FreeImage_GetBits(dib); // Retrieve the image data


	// If somehow one of these failed (they shouldn't), return failure
	if(bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0) 
		return false;
	int test = FreeImage_GetDIBSize(dib);
	*bmpBytes = new char [iWidth*iHeight*bpp/8];
	memcpy(*bmpBytes, bDataPointer, iWidth*iHeight*bpp/8);
	
	/*
	GLenum format;
	FreeImage_GetBPP(dib);
	if(FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
	if(FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	if(FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
	*/
	
	FreeImage_Unload(dib);
	return true; // Success
}


