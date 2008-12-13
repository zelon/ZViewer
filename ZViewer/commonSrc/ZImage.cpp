/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
*
*   2005. 5. 7
*	ZImage.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "ZImage.h"
#include "CommonDefine.h"

#include <string>

using namespace std;

ZImage & ZImage::operator=(const ZImage & image)
{
	if ( this == &image) return *this;

	m_image = image.m_image;
	m_originalWidth = image.m_originalWidth;
	m_originalHeight = image.m_originalHeight;
	m_originalSize = image.m_originalSize;

	return (*this);
}

void ZImage::GetExifList(std::list < TagData > & exifList)
{
#ifdef _DEBUG
	DWORD dwStart = GetTickCount();
#endif
	FREE_IMAGE_MDMODEL iEnumIndex = (FREE_IMAGE_MDMODEL)(FIMD_NODATA + 1);

	fipTag tag;
	fipMetadataFind finder;
	TagData tagData;

	for ( ; iEnumIndex <= FIMD_CUSTOM; iEnumIndex=(FREE_IMAGE_MDMODEL)(iEnumIndex+1) )
	{
		if( finder.findFirstMetadata(iEnumIndex, m_image, tag) )
		{
			do
			{
#ifndef _DEBUG
				try
				{
#endif
					tagData.m_strKey = tag.getKey();
					tagData.m_strValue = tag.toString(iEnumIndex);
					exifList.push_back(tagData);
#ifndef _DEBUG
				}
				catch ( ... )
				{
				}
#endif

			} while( finder.findNextMetadata(tag) );
		}
	}

#ifdef _DEBUG
	DWORD dwEnd = GetTickCount();
	DebugPrintf(TEXT("Get exifdata time span : %d"), (dwEnd - dwStart));
#endif
}

void ZImage::AutoRotate()
{
	fipTag tag;
	fipMetadataFind finder;

	if( finder.findFirstMetadata(FIMD_EXIF_MAIN, m_image, tag) )
	{
		do
		{
			string strValue = tag.toString(FIMD_EXIF_MAIN);

			if ( strcmp(tag.getKey(), "Orientation") == 0 )
			{
				if ( strValue == EXIF_ROTATION_90 )
				{
					Rotate(-90);
				}
				else if ( strValue == EXIF_ROTATION_180 )
				{
					Rotate(-180);
				}
				else if ( strValue == EXIF_ROTATION_270 )
				{
					Rotate(-270);
				}
				break;
			}
		} while( finder.findNextMetadata(tag) );
	}
}