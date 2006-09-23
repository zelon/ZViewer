/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MessageManager.cpp
 *
 * 2006.9.23 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "MessageManager.h"
#include "UnicodeFile.h"
#include "CommonFunc.h"

CMessageManager::CMessageManager()
{
}


CMessageManager::~CMessageManager()
{
}


/// 어떤 언어를 쓸건지 선택한다.
void CMessageManager::SetLanguage(const std::string & lang)
{
	tstring strFileName = GetProgramFolder();
	strFileName += TEXT("\\language\\");


	if ( lang == "korean" )
	{
		strFileName = TEXT("korean.txt");
	}
	else /// 기본값은 영어이다.
	{
		strFileName = TEXT("english.txt");
	}

}