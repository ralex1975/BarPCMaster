#pragma once

#include "CppSQLite3U.h"
#include <vector>

/*
数据库相关宏
*/
#define SQLITE_DB_FOLDER		_T("Data")
#define SQLITE_DB_FILENAME		_T("BarPCMaster.dat")
#define SQLITE_DB_TABLE			_T("EXAMINATION")
#define SQLITE_SQL_CREATE_TABLE	_T( \
"CREATE TABLE EXAMINATION(" \
"ID				INTEGER			PRIMARY KEY AUTOINCREMENT, " \
"DESCRIPTION	VARCHAR(50)		NOT NULL, " \
"STATUS			BOOL			NOT NULL, " \
"TYPE			INTEGER			NOT NULL, " \
"OPERATION		INTEGER			NOT NULL, " \
"PATH			VARCHAR(512)	NOT NULL, " \
"KEY			VARCHAR(256), " \
"KEYTYPE		VARCHAR(32), " \
"COMPARETYPE	INTEGER" \
"VALUE			VARCHAR(256), " \
"COMPARISON		INTEGER, " \
"NOTES			VARCHAR(256))")

/*
	DESCRIPTION		问题描述，显示在界面上
	STATUS			问题的状态，已经解决还是已经忽略
	TYPE			问题类型，清理还是加速还是其他
	OPERATION		相关操作，清理目录，优化注册表等
	PATH			问题关联的路径
	KEY				问题关联的文件信息或者注册表 Key 信息
	KEYTYPE			问题关联的注册表值类型信息
	VALUE			问题关联的注册表值信息，也是一个正常的参考值
	COMPARISON		大于还是小于还是等于参考值
	NOTES			备注信息，不显示，暂时无用
*/

typedef struct _tag_PROBLEMITEM
{
	int				nID;
	CString		strDescription;
	BOOL			bStatus;
	int				nType;
	int				nOperation;
	CString		strPath;
	CString		strKey;
	int				nKeyType;
	int				nCompareType;
	CString		strValue;
	int				nComparison;
	CString		strNotes;
	void*			pControl;
} PROBLEMITEM, *PPROBLEMITEM;

class CDBDataMgr
{
public:
	CDBDataMgr(LPCTSTR lpPath);
	~CDBDataMgr();

	void	Init();
	UINT	GetRowCount();
	BOOL	GetAllData(std::vector<PROBLEMITEM>& vecExaminationList);
	BOOL	InsertItem(const PROBLEMITEM& pProblemItem);

private:
	CppSQLite3DB*	m_pDB;
	CString		m_strPath;
};

