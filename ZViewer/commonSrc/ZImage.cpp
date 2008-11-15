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

#include <string>

using namespace std;

void ZImage::GetExifList(std::list < TagData > & exifList)
{
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
				tagData.m_strKey = tag.getKey();
				tagData.m_strValue = tag.toString(iEnumIndex);
				exifList.push_back(tagData);

			} while( finder.findNextMetadata(tag) );
		}
	}
}
