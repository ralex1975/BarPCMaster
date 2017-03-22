#ifndef __EXAMINATION_H__
#define __EXAMINATION_H__

#include <thread>
#include "MainFrameWnd.h"
#include "DBDataMgr.h"

/*

	看到 Effective C++ 建议使用全局 const 替代 #define
	但考虑全局变量可能会很多，且如果使用宏出错的几率也不是很大
	宏比全局 const 更容易识别，暂时使用宏处理
	const DWORD bpcdwStopColor = 0xFFE7614E;
	const DWORD bpcdwStartColor = 0xFFED8033;
*/

#define STOP_BTN_BKCOLOR	0xFFE7614E
#define START_BTN_BKCOLOR	0xFFED8033

#define TOTALBYTES			2048

class CMainFrameWnd;
class CExamination
{
	enum EXAMINATION_TYPE
	{
		EXT_TYPE_CLEAN,					// 清理
		EXT_TYPE_SPEEDUP				// 加速
	};

	enum EXAMINATION_KEYTYPE
	{
		EXT_KEYTYPE_DWORD,				// 32位 DWORD 类型
		EXT_KEYTYPE_STRING,				// 字符串类型
	};

	enum EXAMINATION_OPERATION
	{
		EXT_OPT_FILES,					// 清理指定文件
		EXT_OPT_REG32,					// 32位注册表
		EXT_OPT_REG64,					// 64位注册表
	};

	enum EXAMINATION_COMPATISON
	{
		EXT_COMPT_LT = -2,				// 小于
		EXT_COMPT_LTE,					// 小于等于
		EXT_COMPT_EQUAL,				// 等于
		EXT_COMPT_GTE,					// 大于等于
		EXT_COMPT_GT,					// 大于
	};

public:
	CExamination(CMainFrameWnd* pMainFrameWnd);
	~CExamination();

	BOOL		Start();
	BOOL		Stop();
	BOOL		IsRunning();

	static VOID BeginExamation(CExamination* pExamination);
	VOID		ExamationThreadInstance();

	// 功能函数
	DWORD		ScanDirectory(LPCTSTR szDirectory, LPCTSTR szType, LONGLONG& dwFileSize);
	DWORD		ScanRegistry(const PROBLEMITEM& refProblemData);

	CDuiString	GetSizeString(LONGLONG llSize);

private:
	BOOL						m_bIsRunning;
	BOOL						m_bIsStop;
	CDBDataMgr*					m_pDBMgr;
	CMainFrameWnd*				m_pMainFrameWnd;
	std::thread*				m_pExamationThread;	// 体检线程
	std::vector<PROBLEMITEM>	m_vecProblemList;	// 问题列表

};

#endif
