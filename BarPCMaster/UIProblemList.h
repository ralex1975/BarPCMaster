#ifndef __UIPROBLEMLIST_H__
#define __UIPROBLEMLIST_H__

const TCHAR bpcProblemListUIInterFace[]			= _T("ProblemList");
const TCHAR bpcProblemListGroupUIInferFace[]	= _T("ProblemListGroup");
const TCHAR bpcProblemListItemUIInferFace[]		= _T("ProblemListItem");

// --------------------------- CProblemListItemUI

class CProblemListItemUI : public CTreeNodeUI
{
public:
	CProblemListItemUI();
	~CProblemListItemUI();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR pstrName);

protected:
private:
};

// --------------------------- CProblemListGroupUI

class CProblemListUI;
class CProblemListGroupUI : public CTreeNodeUI
{
public:
	CProblemListGroupUI();
	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual void SetItemText(LPCTSTR pstrValue);

	bool Add(CProblemListItemUI* pControl, bool bAutoRefreshIndex = true);
	bool AddAt(CProblemListItemUI* pControl, int iIndex, bool bAutoRefreshIndex = true);
	//bool RemoveAt(CProblemListItemUI* _pTreeNodeUI, bool bAutoRefreshIndex = true);


	void SetProblemList(CProblemListUI* pProblemList);

private:
	CDuiString			m_GroupName;
	CProblemListUI*		m_pProblemList;
};

// --------------------------- CProblemListUI

class CProblemListUI : public CTreeViewUI, public IDialogBuilderCallback
{
public:
	enum { SCROLL_TIMERID = 10 };

	CProblemListUI(CPaintManagerUI& PaintManager);
	~CProblemListUI();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	bool AddGroup(LPCTSTR szGroupName, int nIndex = -1);
	bool AddItem(LPCTSTR szGroupName, LPCTSTR szProblemText, bool bAutoRefreshIndex = true, int nIndex = -1);

private:
	CPaintManagerUI&	m_PaintManager;
	CDialogBuilder		m_dlgGroupBuilder;
	CDialogBuilder		m_dlgItemBuilder;
};

#endif
