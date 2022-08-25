// ts_servie_of_tester.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <ct_type.h>

#include "cts_service.h"


int _tmain(int argc, wchar_t* argv[])
{
	std::wstring stemp;
	cts_service& svr = cts_service::get_instance();
	if (!svr.is_ini())
	{
		std::wcout << L"Fail Get Service object." << std::endl;
		return 0;
	}

	_ns_tools::type_v_wstring v_arg;

	for (int i = 0; i < argc; i++) {
		v_arg.push_back(std::wstring(argv[i]));
	}//end for


	// uninstall service if switch is "-u"
	if (v_arg.size() == 2 && v_arg[v_arg.size()-1].compare(L"-u") == 0)
		svr.uninstall();

	// install service if switch is "-i"
	else if (v_arg.size() == 2 && v_arg[v_arg.size() - 1].compare(L"-i") == 0)
		svr.install();

	// bounce service if switch is "-b"
	else if (v_arg.size() == 2 && v_arg[v_arg.size() - 1].compare(L"-b") == 0)
	{
		svr.kill_service();
		svr.run_service();
	}
	// kill a service
	else if (v_arg.size() == 2 && v_arg[v_arg.size() - 1].compare(L"-k") == 0)
	{
		if (!svr.kill_service())
		{
			std::wcout << L"Failed to kill service " << std::endl;
		}
	}
	// run a service
	else if (v_arg.size() == 2 && v_arg[v_arg.size() - 1].compare(L"-r") == 0)
	{
		if (!svr.run_service())
			std::wcout << L"Failed to run service " << std::endl;
	}
	// assume user is starting this service 
	else
	{
		// pass dispatch table to service controller
		unsigned long dw_error = svr.launch_service();
		if (dw_error) {
			std::wcout << L"StartServiceCtrlDispatcher failed, error code = " << std::to_wstring((unsigned long long)dw_error) << std::endl;
		}
		// you don't get here unless the service is shutdown
	}

	return 0;
}

