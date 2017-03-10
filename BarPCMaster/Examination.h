#ifndef __EXAMINATION_H__
#define __EXAMINATION_H__

#include <thread>
#include "MainFrameWnd.h"

/*

	看到 Effective C++ 建议使用全局 const 替代 #define
	但考虑全局变量可能会很多，且如果使用宏出错的几率也不是很大
	宏比全局 const 更容易识别，暂时使用宏处理
	const DWORD bpcdwStopColor = 0xFFE7614E;
	const DWORD bpcdwStartColor = 0xFFED8033;
*/

#define STOP_BTN_BKCOLOR	0xFFE7614E
#define START_BTN_BKCOLOR	0xFFED8033

class CMainFrameWnd;
class CExamination
{
public:
	CExamination(CMainFrameWnd* pMainFrameWnd);
	~CExamination();

	BOOL		Start();
	BOOL		Stop();
	BOOL		IsRunning();

	static VOID BeginExamation(CExamination* pExamination);
	VOID		ExamationThreadInstance();

private:
	BOOL					m_bIsRunning;
	BOOL					m_bIsStop;

	CMainFrameWnd*			m_pMainFrameWnd;

	//CProblemListUI*			m_pProblemListUI;		// 问题列表
	//CButtonUI*				m_pExaminationBtn;		// 立即体检按钮
	//CButtonUI*				m_pReturnBtn;			// 返回按钮
	//CTextUI*				m_pTipsText;			// 提示信息
	//CProgressUI*			m_pProgressFront;		// 进度条

	//CVerticalLayoutUI*		m_pVLayoutMain;			// 主界面
	//CVerticalLayoutUI*		m_pVLayoutExamination;	// 体检界面

	std::thread*			m_pExamationThread;		// 体检线程

};

#endif
