// BarPCMaster.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "BarPCMaster.h"
#include "MainFrameWnd.h"

// 初始化日志模块
INITIALIZE_EASYLOGGINGPP

void InitlizeLogger()
{
	el::Configurations defaultConf;
	defaultConf.setToDefault();

	// 设置全局的属性
	defaultConf.setGlobally(el::ConfigurationType::Enabled, "true");
	defaultConf.setGlobally(el::ConfigurationType::ToFile, "true");
	defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
	defaultConf.setGlobally(el::ConfigurationType::Format, "[%datetime{%Y-%M-%d %H:%m:%s.%g}][%fbase:%line][TD=%thread][%level] - %msg");
	defaultConf.setGlobally(el::ConfigurationType::MaxLogFileSize, "1125899906842624");

	// 设置 INFO 级别的日志属性
	defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "[%datetime{%Y-%M-%d %H:%m:%s.%g}][TD=%thread][%level] - %msg");

	el::Loggers::reconfigureLogger("default", defaultConf);

	LOG(INFO) << "==================== InitlizeLogger =====================";
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	InitlizeLogger();

 	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
	{
		return 0;
	}

	// 设置关联实例
	CPaintManagerUI::SetInstance(hInstance);
	// 设置皮肤路径
	CPaintManagerUI::SetCurrentPath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourcePath(_T("Skin"));
	//CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("Skin"));
	//CPaintManagerUI::SetResourceZip(_T("Skin.zip"));

	CMainFrameWnd* pFrame = new CMainFrameWnd(_T("main_frame.xml"));

	pFrame->Create(NULL, _T("BarPCMaster"), UI_WNDSTYLE_DIALOG, 0);
	pFrame->CenterWindow();
	pFrame->ShowWindow();

	CPaintManagerUI::MessageLoop();

	if (nullptr != pFrame)
	{
		delete pFrame;
	}

	::CoUninitialize();

	return 0;
}

