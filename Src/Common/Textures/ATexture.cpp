/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ATexture.cpp
 *   DATE: Mon Jun 11 15:09:54 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Textures/ATexture.h"
#include "freeimage/include/FreeImage.h"
#include <iostream>
/***********************************************************************
 *  Method: ATexture::ATexture
 *  Params: 
 * Effects: 
 ***********************************************************************/
ATexture::ATexture()
 : m_mipMapsGenerated( false ){}


/***********************************************************************
 *  Method: ATexture::~ATexture
 *  Params: 
 * Effects: 
 ***********************************************************************/
ATexture::~ATexture()
{
}



/***********************************************************************
 *  Method: ATexture::Load
 *  Params: string path, bool generateMipMaps
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool ATexture::Load(std::string path, bool generateMipMaps)
{
  	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(path.c_str(), 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(path.c_str());
	
	if(fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, path.c_str());

	if(!dib) {
		return false;
	}

	char* pData = (char*)FreeImage_GetBits(dib); // Retrieve the image data

	// If somehow one of these failed (they shouldn't), return failure
	if (pData == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;
	
	CreateFromData(pData, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), generateMipMaps);
	
	FreeImage_Unload(dib);

	m_path = path;

	return true; // Success
}


/***********************************************************************
 *  Method: ATexture::GetWidth
 *  Params: 
 * Returns: int
 * Effects: 
 ***********************************************************************/
int ATexture::GetWidth()
{
	return m_width;
}


/***********************************************************************
 *  Method: ATexture::GetHeight
 *  Params: 
 * Returns: int
 * Effects: 
 ***********************************************************************/
int ATexture::GetHeight()
{
  return m_height;
}


/***********************************************************************
 *  Method: ATexture::GetBPP
 *  Params: 
 * Returns: int
 * Effects: 
 ***********************************************************************/
int ATexture::GetBPP()
{
  return m_bpp;
}


