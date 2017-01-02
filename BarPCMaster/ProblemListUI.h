#pragma once

const TCHAR szProblemListUIInterFace[]		= _T("ProblemList");

const TCHAR szProblemListItemUIInferFace[]	= _T("ProblemListItem");

const TCHAR szProblemListGroupUIInferFace[]	= _T("ProblemListGroup");

class CProblemListItemUI : public CTreeNodeUI
{
public:
protected:
private:
};

class CProblemListUI;
class CProblemListGroupUI : public CTreeNodeUI
{
public:
	CProblemListGroupUI();

	void SetProblemList(CProblemListUI* pProblemList);

private:
	CProblemListUI*		m_pProblemList;
};

class CProblemListUI : public CTreeViewUI, public IDialogBuilderCallback
{
public:
	enum { SCROLL_TIMERID = 10 };

	CProblemListUI(CPaintManagerUI& PaintManager);
	~CProblemListUI();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	bool AddGroup(LPCTSTR szGroupName, int nIndex = -1);
	bool AddItem(LPCTSTR szGroupName, bool bAutoRefreshIndex = true, int nIndex = -1);

private:
	CPaintManagerUI&	m_PaintManager;
	CDialogBuilder		m_dlgGroupBuilder;
	CDialogBuilder		m_dlgItemBuilder;
};

