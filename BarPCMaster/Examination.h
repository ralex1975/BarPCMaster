#pragma once
#include "UIProblemList.h"
#include <thread>

class CExamination
{
public:
	CExamination(CPaintManagerUI& PaintManagerUI);
	~CExamination();

	VOID		Init();
	VOID		Back();

	BOOL		Start();
	BOOL		Stop();
	BOOL		IsRunning();

	static VOID BeginExamation(CExamination* pExamination);
	VOID		ExamationThreadInstance();

private:
	BOOL					m_bIsRunning;
	BOOL					m_bIsStop;
	CPaintManagerUI&		m_PaintManager;

	CProblemListUI*			m_pProblemListUI;		// 问题列表
	CButtonUI*				m_pExaminationBtn;		// 立即体检按钮
	CButtonUI*				m_pReturnBtn;			// 返回按钮
	CTextUI*				m_pTipsText;			// 提示信息
	CProgressUI*			m_pProgressFront;		// 进度条

	CVerticalLayoutUI*		m_pVLayoutMain;			// 主界面
	CVerticalLayoutUI*		m_pVLayoutExamination;	// 体检界面

	std::thread*			m_pExamationThread;		// 体检线程

};

