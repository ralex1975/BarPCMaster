#ifndef __MAINFRAMEWND_H__
#define __MAINFRAMEWND_H__

#include "UIProblemList.h"
#include "Examination.h"

class CExamination;
class CMainFrameWnd : public WindowImplBase
{
public:
	CMainFrameWnd(CDuiString strXMLPath);
	~CMainFrameWnd();

protected:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName(void) const;
	virtual CControlUI* CreateControl(LPCTSTR pstrClassName);

	virtual void		Notify(TNotifyUI& msg);
	virtual void		InitWindow();

	virtual LRESULT		MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	virtual LRESULT		OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BOOL AddGroupToList(LPCTSTR lpGroupName);
	BOOL AddItemToList(LPCTSTR lpGroupName, LPCTSTR lpItemValue);
	void SetProgressValue(int value);
	void SetBtnText(LPCTSTR lpText);
	void SetBtnBkColor(DWORD dwColor);
	void SetTipsText(LPCTSTR lpTipsText);
	void ShowReturnButton(bool bIsShow = TRUE);

	void ShowMainLayout();
	void ShowExaminationLayout();

private:
	// CShadowUI*			m_pShadowUI;
	CDuiString				m_strXMLPath;
	CExamination*			m_Examination;

	CProblemListUI*			m_pProblemListUI;		// 问题列表
	CButtonUI*				m_pExaminationBtn;		// 立即体检按钮
	CButtonUI*				m_pReturnBtn;			// 返回按钮
	CTextUI*				m_pTipsText;			// 提示信息
	CProgressUI*			m_pProgressFront;		// 进度条

	CVerticalLayoutUI*		m_pVLayoutMain;			// 主界面
	CVerticalLayoutUI*		m_pVLayoutExamination;	// 体检界面
};

#endif
