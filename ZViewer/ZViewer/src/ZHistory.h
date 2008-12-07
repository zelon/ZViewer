/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
*
*   2005. 5. 7
*	ZHistory.h
*
*                                       http://www.wimy.com
*********************************************************************/

/*

 이 클래스는 사용자가 키를 잘못 조작하여 갑자기 엉뚱한 이미지 파일로 넘어갔을 때를 위해
 이미지 파일을 볼 때마다 기록을 하여 다시 인덱스를 되돌리게 하기 위한 클래스이다.
*/

#pragma once

#include <list>

class ZHistory
{
public:

	ZHistory();

	std::deque < int > m_indexVector;

	void push_lastImageIndex(int iIndex);

	int Undo();
	int Redo();

	bool CanUndo() const
	{
		if ( m_indexVector.size() <= 0 ) return false;
		if ( m_iCurrentIndex <= 0 ) return false;

		return true;
	}

	bool CanRedo() const
	{
		if ( m_indexVector.size() <= 0 ) return false;
		if ( m_iCurrentIndex >= m_indexVector.size() ) return false;

		return true;
	}

protected:
	size_t m_iCurrentIndex;			// 현재 Undo/Redo 의 위치
};
