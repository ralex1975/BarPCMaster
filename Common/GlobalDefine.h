#pragma once

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
	EXT_OPT_FILES,					// 指定文件
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
