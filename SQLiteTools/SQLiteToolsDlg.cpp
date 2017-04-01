
// SQLiteToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SQLiteTools.h"
#include "SQLiteToolsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSQLiteToolsDlg 对话框



CSQLiteToolsDlg::CSQLiteToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SQLITETOOLS_DIALOG, pParent)
	, m_editPath(_T(""))
	, m_editKey(_T(""))
	, m_editDefaultValue(_T(""))
	, m_editDescript(_T(""))
	, m_pDataMgr(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSQLiteToolsDlg::~CSQLiteToolsDlg()
{
	if (nullptr != m_pDataMgr)
	{
		delete m_pDataMgr;
		m_pDataMgr = nullptr;
	}
}

void CSQLiteToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_editDescript);
	DDX_Control(pDX, IDC_COMBO1, m_comboboxType);
	DDX_Control(pDX, IDC_COMBO2, m_comboboxOperation);
	DDX_Text(pDX, IDC_EDIT3, m_editPath);
	DDX_Text(pDX, IDC_EDIT4, m_editKey);
	DDX_Control(pDX, IDC_COMBO3, m_comboboxKeyType);
	DDX_Control(pDX, IDC_COMBO4, m_comboboxCompareType);
	DDX_Control(pDX, IDC_COMBO5, m_comboboxComparison);
	DDX_Text(pDX, IDC_EDIT6, m_editDefaultValue);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CSQLiteToolsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSQLiteToolsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSQLiteToolsDlg 消息处理程序

BOOL CSQLiteToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 初始化数据库类指针
	TCHAR szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathRemoveFileSpec(szFilePath);
	PathAppend(szFilePath, SQLITE_DB_FOLDER);
	if (_taccess(szFilePath, 0) == -1)
	{
		CreateDirectory(szFilePath, NULL);
	}
	PathAppend(szFilePath, SQLITE_DB_FILENAME);

	m_pDataMgr = new CDBDataMgr(szFilePath);

	// 初始化 combobox 数据
	m_comboboxType.InsertString(0, _T("清理"));
	m_comboboxType.InsertString(1, _T("加速"));
	m_comboboxType.SetCurSel(0);

	m_comboboxOperation.InsertString(0, _T("文件操作"));
	m_comboboxOperation.InsertString(1, _T("32注册表"));
	m_comboboxOperation.InsertString(2, _T("64注册表"));
	m_comboboxOperation.SetCurSel(0);

	m_comboboxKeyType.InsertString(0, _T("数值类型"));
	m_comboboxKeyType.InsertString(1, _T("字符串类型"));
	m_comboboxKeyType.SetCurSel(0);

	m_comboboxCompareType.InsertString(0, _T("数值类型"));
	m_comboboxCompareType.InsertString(1, _T("字符串类型"));
	m_comboboxCompareType.SetCurSel(0);

	m_comboboxComparison.InsertString(0, _T("<"));
	m_comboboxComparison.InsertString(1, _T("<="));
	m_comboboxComparison.InsertString(2, _T("="));
	m_comboboxComparison.InsertString(3, _T(">="));
	m_comboboxComparison.InsertString(4, _T(">"));
	m_comboboxComparison.SetCurSel(2);

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSQLiteToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSQLiteToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSQLiteToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSQLiteToolsDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	PROBLEMITEM stProblemItem = { 0 };

	int nType = m_comboboxType.GetCurSel();
	int nOperation = m_comboboxOperation.GetCurSel();
	int nKeyType = m_comboboxKeyType.GetCurSel();
	int nCompareType = m_comboboxCompareType.GetCurSel();
	int nComparison = m_comboboxComparison.GetCurSel() - 2;

	stProblemItem.strDescription = m_editDescript.GetString();
	stProblemItem.bStatus = 0;
	stProblemItem.nType = nType;
	stProblemItem.nOperation = nOperation;
	stProblemItem.strPath = m_editPath.GetString();
	stProblemItem.strKey = m_editKey.GetString();
	stProblemItem.nKeyType = nKeyType;
	stProblemItem.nCompareType = nCompareType;
	stProblemItem.strValue = m_editDefaultValue;
	stProblemItem.nComparison = nComparison;

	m_pDataMgr->Init();
	m_pDataMgr->InsertItem(stProblemItem);

	UpdateData(FALSE);
}
