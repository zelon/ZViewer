#include "stdafx.h"
#include "ZImage.h"

#include "CommonDefine.h"

using namespace std;

ZImage::ZImage()
: m_originalWidth(0), m_originalHeight(0), m_originalSize(0) {
}

bool ZImage::LoadFromMemory(fipMemoryIO & memBuffer) {
  if (TRUE == m_image.loadFromMemory(memBuffer))
  {
    _ProcAfterLoad();
    return true;
  }
  return false;
}

bool ZImage::LoadFromFile(const tstring & strFilename) {
  //DebugPrintf("LoadFromFile : %s", strFilename.c_str());

#ifndef _DEBUG
  try
  {
#endif
    if (m_image.loadU(strFilename.c_str()) == TRUE)
    {
      _ProcAfterLoad();
      return true;
    }
#ifndef _DEBUG
  }
  catch (...)
  {
    assert(false);
    return false;
  }
#endif
  return false;
}

void ZImage::GetExifList(std::list < TagData > & exifList) {
#ifdef _DEBUG
  TIMECHECK_START("Get exifdata time span");
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
  TIMECHECK_END();
#endif
}

void ZImage::AutoRotate() {
  fipTag tag;
  fipMetadataFind finder;

  if( finder.findFirstMetadata(FIMD_EXIF_MAIN, m_image, tag) ) {
    do {
      string strValue = tag.toString(FIMD_EXIF_MAIN);

      if ( strcmp(tag.getKey(), "Orientation") == 0 ) {
        if ( strValue == EXIF_ROTATION_90 ) {
          Rotate(-90);
        } else if ( strValue == EXIF_ROTATION_180 ) {
          Rotate(-180);
        } else if ( strValue == EXIF_ROTATION_270 ) {
          Rotate(-270);
        }
        break;
      }
    } while( finder.findNextMetadata(tag) );
  }
}

bool ZImage::PasteFromClipboard() {
  if ( FALSE == m_image.pasteFromClipboard() ) {
    return false;
  }
  if ( FALSE == m_image.convertTo24Bits() ) {
    return false;
  }
  
  return true;
}

/// freeimage 가 투명도가 있는 그림을 jpg 로 저장못하기 때문에 DC 에 그린 후 다시 저장해본다.
bool ZImage::SaveToFileThroughDC(const tstring & /*strFilename*/ ) const {
  //RECT rect = { 0, 0, m_originalWidth, m_originalHeight };

  //m_image.draw(hDC, &rect);

  //m_image.

//	HBIT

  assert(!"not implemented");
  return false;
}

void ZImage::_ProcAfterLoad() {
  m_originalWidth = m_image.getWidth();
  m_originalHeight = m_image.getHeight();
  m_originalSize = m_image.getImageSize();
}

