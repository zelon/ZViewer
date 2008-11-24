/* ------------------------------------------------------------------------
 *
 * ZImageManager.h
 *
 * 2008.11.24 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include <map>

typedef unsigned int ZHandle;

#include "ZImage.h"

///
class ZImageManager
{
public:
	~ZImageManager();

protected:
	std::map < ZHandle, ZImage * > m_handleToZImagePtr;
};