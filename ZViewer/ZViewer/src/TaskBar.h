/********************************************************************
*
* Created by zelon(Kim, Jinwook - Korea)
* 
*   2008. 6. 2 created
*	FileName : TaskBar.h
*
*                                       http://zviewer.wimy.com
*********************************************************************/

/// 작업 표시줄을 관리하는 클래스
class TaskBar
{
public:
	/// 윈도우의 작업 표시줄을 보이게 해준다.
	static void ShellTrayShow();

	/// 윈도우의 작업 표시줄을 숨긴다.
	static void ShellTrayHide();

};