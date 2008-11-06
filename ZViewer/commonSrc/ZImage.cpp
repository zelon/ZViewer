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

void ZImage::GetExifMap(std::map < tstring, tstring > & exifMap)
{
	int iEnumIndex = FIMD_NODATA + 1;
	int iIndex = 0;
	for ( iEnumIndex = FIMD_NODATA + 1; iEnumIndex <= FIMD_CUSTOM; ++iEnumIndex )
	{
		/*
		unsigned iCount = m_image.getMetadataCount(iEnumIndex);

		for ( iIndex = 0; i<iCount; ++i )
		{
			m_image.getMetadata(iEnumIndex, )
		}
		*/
	}
}
