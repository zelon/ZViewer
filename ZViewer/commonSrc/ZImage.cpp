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

void ZImage::GetExifMap(std::map < string, string > & exifMap)
{
	FREE_IMAGE_MDMODEL iEnumIndex = (FREE_IMAGE_MDMODEL)(FIMD_NODATA + 1);
	unsigned int iIndex = 0;

	fipTag tag;
	fipMetadataFind finder;

	for ( ; iEnumIndex <= FIMD_CUSTOM; iEnumIndex=(FREE_IMAGE_MDMODEL)(iEnumIndex+1) )
	{
		unsigned iCount = m_image.getMetadataCount(iEnumIndex);

		for ( iIndex = 0; iIndex<iCount; ++iIndex )
		{
			if( finder.findFirstMetadata(iEnumIndex, m_image, tag) )
			{
				do
				{
					// process the tag
					//cout << "Key : " << tag.getKey() << "\n";
					//cout << "toString : " << tag.toString(FIMD_EXIF_MAIN) << endl;
					exifMap[tag.getKey()] = tag.toString(iEnumIndex);

				} while( finder.findNextMetadata(tag) );
			}
		}
	}
}
