#include "stdafx.h"
#include "ZHistory.h"

ZHistory::ZHistory()
: m_iCurrentIndex(0)
{

}

void ZHistory::push_lastImageIndex(int iIndex)
{
	// 현재 위치 이후의 Redo 는 지운다.
	m_indexVector.resize(m_iCurrentIndex);

	// 마지막에 넣는다.
	m_indexVector.push_back(iIndex);

	// index 위치를 조정한다.
	m_iCurrentIndex = m_indexVector.size();
}

int ZHistory::Undo()
{
	assert(CanUndo());

	// Undo/Redo 위치를 조정한다.
	--m_iCurrentIndex;

	// 마지막으로 봤던 것을 돌려준다.
	int iRet = m_indexVector[m_iCurrentIndex];


	return iRet;
}

int ZHistory::Redo()
{
	assert(CanRedo());

	// Undo 를 되돌린다.
	++m_iCurrentIndex;

	return m_indexVector[m_iCurrentIndex];
}
