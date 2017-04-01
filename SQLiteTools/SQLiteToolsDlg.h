
// SQLiteToolsDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "DBDataMgr.h"
#include "GlobalDefine.h"


// CSQLiteToolsDlg 对话框
class CSQLiteToolsDlg : public CDialogEx
{
// 构造
public:
	CSQLiteToolsDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSQLiteToolsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SQLITETOOLS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	afx_msg void OnBnClickedOk();


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CString		m_editDescript;
	CComboBox	m_comboboxType;
	CComboBox	m_comboboxOperation;
	CString		m_editPath;
	CString		m_editKey;
	CComboBox	m_comboboxKeyType;
	CComboBox	m_comboboxCompareType;
	CComboBox	m_comboboxComparison;
	CString		m_editDefaultValue;
	CListCtrl	m_listCtrl;

	CDBDataMgr*	m_pDataMgr;
	
};
