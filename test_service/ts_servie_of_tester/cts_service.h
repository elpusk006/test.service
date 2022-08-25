#pragma once

#include <mutex>
#include <Windows.h>
#include <winsvc.h>
#include <process.h>
#include <TlHelp32.h>
#include <WtsApi32.h>
#include <UserEnv.h>

#include <ct_log.h>
#include <ct_system.h>
#include <ct_warp.h>
#include <ct_file.h>
#include <ct_xml_list.h>

#include <ctest_item.h>

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Wtsapi32.lib")


class cts_service
{

private:
	enum { const_size_table = 2 };

	struct _process_para {

		std::wstring s_command_line;
		std::wstring s_working_directory;
		UINT n_pause_start;
		UINT n_pause_end;
		bool b_user_interface;
		bool b_restart;
		std::wstring s_user_name;
		std::wstring s_domain;
		std::wstring s_password;
	};

public:
	static cts_service& get_instance()
	{
		static cts_service obj;
		return obj;
	}

	/**
	* ****************************************************
	* called functions in main().
	* ****************************************************
	*/
	bool is_ini() const
	{
		return m_b_ini;
	}

	bool kill_service()
	{
		bool b_result(false);

		do {
			_ns_tools::ct_warp::cservice_handle sch_scmanager(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
			if (sch_scmanager.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : OpenSCManager().\n", __WFUNCTION__);
				continue;
			}
			_ns_tools::ct_warp::cservice_handle sch_service(::OpenService(sch_scmanager.get_handle(), m_s_service_name.c_str(), SERVICE_ALL_ACCESS));
			if (sch_service.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : OpenService().\n", __WFUNCTION__);
				continue;
			}
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if (!::ControlService(sch_service.get_handle(), SERVICE_CONTROL_STOP, &status)) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : ControlService().\n", __WFUNCTION__);
				continue;
			}
			//
			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s\n", __WFUNCTION__);
			b_result = true;
		} while (false);
		return b_result;
	}
	bool run_service()
	{
		bool b_result(false);

		do {
			_ns_tools::ct_warp::cservice_handle sch_scmanager(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
			if (sch_scmanager.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : OpenSCManager().\n", __WFUNCTION__);
				continue;
			}
			_ns_tools::ct_warp::cservice_handle sch_service(::OpenService(sch_scmanager.get_handle(), m_s_service_name.c_str(), SERVICE_ALL_ACCESS));
			if (sch_service.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : OpenService().\n", __WFUNCTION__);
				continue;
			}
			// call StartService to run the service
			if (!::StartService(sch_service.get_handle(), 0, NULL)) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : StartService().\n", __WFUNCTION__);
			}
			//
			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s\n", __WFUNCTION__);
			b_result = true;
		} while (false);
		return b_result;
	}
	void uninstall()
	{
		do {
			_ns_tools::ct_warp::cservice_handle sch_scmanager(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
			if (sch_scmanager.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : OpenSCManager().\n", __WFUNCTION__);
				continue;
			}
			_ns_tools::ct_warp::cservice_handle sch_service(::OpenService(sch_scmanager.get_handle(), m_s_service_name.c_str(), SERVICE_ALL_ACCESS));
			if (sch_service.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : OpenService().\n", __WFUNCTION__);
				continue;
			}
			if (!DeleteService(sch_service.get_handle())) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : DeleteService().\n", __WFUNCTION__);
				continue;
			}
			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s\n", __WFUNCTION__);
		} while (false);
	}
	void install()
	{
		do {
			SERVICE_DESCRIPTION sd;
			wchar_t s_description[] = L"application that is running th system account for testing.";

			_ns_tools::ct_warp::cservice_handle sch_scmanager(::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE));
			if (sch_scmanager.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : OpenSCManager().\n", __WFUNCTION__);
				continue;
			}

			_ns_tools::ct_warp::cservice_handle sch_service(::CreateService
			(
				sch_scmanager.get_handle(),	/* SCManager database      */
				m_s_service_name.c_str(),			/* name of service         */
				m_s_service_name.c_str(),			/* service name to display */
				SERVICE_ALL_ACCESS,        /* desired access          */
				//SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
				SERVICE_WIN32_OWN_PROCESS, /* service type            */
				SERVICE_AUTO_START,      /* start type              */
				SERVICE_ERROR_NORMAL,      /* error control type      */
				m_s_exe_file_name.c_str(),			/* service's binary        */
				NULL,                      /* no load ordering group  */
				NULL,                      /* no tag identifier       */
				NULL,                      /* no dependencies         */
				NULL,                      /* LocalSystem account     */
				NULL
			));                   /* no password             */
			if (sch_service.get_handle() == NULL) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : CreateService().\n", __WFUNCTION__);
				continue;
			}
			sd.lpDescription = s_description;

			if (!ChangeServiceConfig2(sch_service.get_handle(), SERVICE_CONFIG_DESCRIPTION, &sd)) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : ChangeServiceConfig2().\n", __WFUNCTION__);
			}
			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s\n", __WFUNCTION__);

		} while (false);

	}

	unsigned long launch_service()
	{
		unsigned long dw_result(0);

		do {
			m_dispatch_table[0].lpServiceName = &m_v_dup_service_name[0];
			m_dispatch_table[0].lpServiceProc = cts_service::_service_main;

			m_dispatch_table[1].lpServiceName = NULL;
			m_dispatch_table[1].lpServiceProc = NULL;

			if (!StartServiceCtrlDispatcher(m_dispatch_table)) {
				dw_result = ::GetLastError();	//return error code 
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : StartServiceCtrlDispatcher() : %u.\n", __WFUNCTION__, dw_result);
				continue;
			}

			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s\n", __WFUNCTION__);
		} while (false);
		return dw_result;
	}

private:
	cts_service() : 
		m_b_ini(false),
		m_h_service_status(NULL),
		m_service_status{ 0, },
		m_v_dup_service_name(_MAX_PATH, 0),
		m_proc_info{0,},
		m_dispatch_table{ 0, },
		m_h_kill_event_user_app(NULL),
		m_s_service_name(L"ts_service_of_tester")
	{
		do {
			m_process.s_command_line.clear();
			m_process.s_working_directory.clear();
			m_process.n_pause_start = 0;
			m_process.n_pause_end = 0;
			m_process.b_user_interface = true;
			m_process.b_restart = false;
			m_process.s_user_name.clear();
			m_process.s_domain.clear();
			m_process.s_password.clear();

			_ns_tools::ct_log& log(_ns_tools::ct_log::get_instance());

			if (!log.config(L"Elpusk", 00000006, L"cts_service"))
				continue;

			log.enable_time();
			log.enable();

			if (!_load_xml()) {
				log.log_fmt(L" : E : %s : _load_xml()\n", __WFUNCTION__);
				continue;
			}


			//
			log.log_fmt(L" : S : %s\n", __WFUNCTION__);
			m_b_ini = true;
		} while (false);

	}

	bool _load_xml()
	{
		bool b_result(false);

		do {
			std::wstring s_module_path;
			_ns_tools::type_v_ws_buffer s_path(_MAX_PATH + 1, 0);

			if (!::GetModuleFileName(NULL, &s_path[0], (unsigned long)(s_path.size() - 1))) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : GetModuleFileName()\n", __WFUNCTION__);
				continue;
			}
			//
			m_s_exe_file_name = s_module_path = (WCHAR*)&s_path[0];

			std::wstring s_drive, s_dir, s_name, s_ext;

			if (!_ns_tools::ct_file::split_path(s_module_path, s_drive, s_dir, s_name, s_ext)) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : split_path()\n", __WFUNCTION__);
				continue;
			}

			std::wstring s_xml_path;
			_ns_tools::ct_file::make_path(s_xml_path, s_drive, s_dir, s_name, std::wstring(L".xml"));
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : xml path = %s\n", __WFUNCTION__,s_xml_path.c_str());

			if (!_load_file_xml_structure()) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : _load_file_xml_structure()\n", __WFUNCTION__);
				continue;
			}

			m_map_s_list_obj_element.clear();
			m_xml_list_obj.reset_contents();

			if (!m_xml_list_obj.load_xml(s_xml_path)) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : load_xml()\n", __WFUNCTION__);
				continue;
			}

			if (!_build_map_obj_to_element()) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : _build_map_obj_to_element()\n", __WFUNCTION__);
				continue;
			}
			if (!_load_to_parameter()) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : _load_to_parameter()\n", __WFUNCTION__);
				continue;
			}
			//
			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s\n", __WFUNCTION__);
			b_result = true;
		} while (false);
		return b_result;
	}

	static VOID WINAPI _service_main(DWORD dwArgc, LPTSTR* lpszArgv)
	{
		DWORD   status = 0;
		DWORD   specificError = 0xfffffff;

		cts_service& svr(cts_service::get_instance());

		svr.m_service_status.dwServiceType = SERVICE_WIN32;
		svr.m_service_status.dwCurrentState = SERVICE_START_PENDING;
		svr.m_service_status.dwControlsAccepted =
			SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SESSIONCHANGE;
		svr.m_service_status.dwWin32ExitCode = 0;
		svr.m_service_status.dwServiceSpecificExitCode = 0;
		svr.m_service_status.dwCheckPoint = 0;
		svr.m_service_status.dwWaitHint = 0;

		svr.m_h_service_status = ::RegisterServiceCtrlHandlerEx(svr.m_s_service_name.c_str(), _service_handler_ex, NULL);
		if (svr.m_h_service_status == NULL)
		{
			_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : RegisterServiceCtrlHandlerEx().\n", __WFUNCTION__);
			return;
		}

		// Initialization complete - report running status 
		svr.m_service_status.dwCurrentState = SERVICE_RUNNING;
		svr.m_service_status.dwCheckPoint = 0;
		svr.m_service_status.dwWaitHint = 0;
		if (!SetServiceStatus(svr.m_h_service_status, &(svr.m_service_status)))
		{
			_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : SetServiceStatus().\n", __WFUNCTION__);
		}

		svr.m_proc_info.hProcess = NULL;

		_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : StartProcessesDifferentSession().\n", __WFUNCTION__);
		svr._start_processes_different_session();
		svr._run_as_interactive_user_account();
		
		//cts_service::_test_in_service_start();
	}

	static DWORD WINAPI _service_handler_ex(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
	{
		DWORD dwResult = ERROR_CALL_NOT_IMPLEMENTED;
		std::wstring s_msg;
		int i;

		cts_service& svr(cts_service::get_instance());

		switch (dwControl) {
		case SERVICE_CONTROL_PAUSE:
			svr.m_service_status.dwCurrentState = SERVICE_PAUSED;
			dwResult = NO_ERROR;
			break;
		case SERVICE_CONTROL_CONTINUE:
			svr.m_service_status.dwCurrentState = SERVICE_RUNNING;
			dwResult = NO_ERROR;
			break;
		case SERVICE_CONTROL_STOP:
			svr.m_service_status.dwWin32ExitCode = 0;
			svr.m_service_status.dwCurrentState = SERVICE_STOPPED;
			svr.m_service_status.dwCheckPoint = 0;
			svr.m_service_status.dwWaitHint = 0;

			// terminate process
			svr._end_process();
			dwResult = NO_ERROR;
			break;
		case SERVICE_CONTROL_SHUTDOWN:
			svr.m_service_status.dwWin32ExitCode = 0;
			svr.m_service_status.dwCurrentState = SERVICE_STOPPED;
			svr.m_service_status.dwCheckPoint = 0;
			svr.m_service_status.dwWaitHint = 0;

			// terminate process
			svr._end_process();
			dwResult = NO_ERROR;
			break;
		case SERVICE_CONTROL_SESSIONCHANGE:
			WTSSESSION_NOTIFICATION wtsno;
			CopyMemory(&wtsno, lpEventData, sizeof(WTSSESSION_NOTIFICATION));
			switch (dwEventType)
			{
			case WTS_SESSION_LOGON:
				/*
				OSVERSIONINFO osi;
				osi.dwOSVersionInfoSize = sizeof(osi);
				if (GetVersionEx(&osi))
				{
					//XP에서는새로운세션로그인시바로프로그램을실행시키면
					//CreateProcessAsUser 에서233번오류가발생한다.
					//XP 자체의 오류로 Vista에서는 해결되었다.
					//그래서, 로그온화면이사라질때까지기다린후프로그램들을실행시킨다.
					if ((osi.dwMajorVersion <= 5) && (wtsno.dwSessionId > 0))
					{
						Sleep (3000);
					}
				}
				*/

				svr._end_process();
				
				if (!_ns_tools::ct_system::is_windows_vista_or_greater() && (wtsno.dwSessionId > 0)) {
					// GetVersionEx 는 windows 8 부터 더 이상 사용 금지.
					Sleep(3000);
				}


				//프로그램을실행시킨다.
				s_msg = L"StartProcessesDifferentSession : dwSessionId = ";
				s_msg += std::to_wstring((long long)wtsno.dwSessionId);

				_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : %s.\n", __WFUNCTION__, s_msg.c_str());

				for (i = 0; i < 50; i++) {
					Sleep(100);
					if (svr._start_processes_different_session(wtsno.dwSessionId))
						break;//exit for
				}//end for
				if (i == 50) {
					_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : start use application.\n", __WFUNCTION__);
				}
				break;
			default:
				break;
			};

			dwResult = NO_ERROR;
			break;
		default:
			dwResult = ERROR_CALL_NOT_IMPLEMENTED;
			break;
		}//end switch

		if (!SetServiceStatus(svr.m_h_service_status, &(svr.m_service_status))) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : SetServiceStatus().\n", __WFUNCTION__);
		}

		return dwResult;
	}

	static void _test_in_service_start()
	{
		std::wstring s_temp, s_mutex, s_event;

		s_temp = ctest_item::GetNameShareQ();
		s_mutex = s_temp + L"_MUTEXT_";
		s_event = s_temp + L"_EVENT_";

		if (_ns_tools::ct_system::is_exist_named_mutex(s_mutex)) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : GetNameShareQ()-mutex.\n");//user,system
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : GetNameShareQ()-mutex.\n");
		}

		if (_ns_tools::ct_system::is_exist_named_event(s_event)) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : GetNameShareQ()-event.\n");//user,system
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : GetNameShareQ()-event.\n");
		}


		if (_ns_tools::ct_system::is_exist_named_mutex(ctest_item::GetNameNDMInstatnce())) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : GetNameNDMInstatnce().\n");//user,system
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : GetNameNDMInstatnce().\n");
		}
		//
		if (_ns_tools::ct_system::is_exist_named_mutex(ctest_item::GetNameNDMSetupOk())) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : GetNameNDMSetupOk().\n");//user,system
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : GetNameNDMSetupOk().\n");
		}
		//
		if (_ns_tools::ct_system::is_exist_named_event(ctest_item::GetNameNDMKill())) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : GetNameNDMKill().\n");//user,system
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : GetNameNDMKill().\n");
		}
		//
		if (_ns_tools::ct_system::is_exist_named_event(ctest_item::GetNameSSS_GEvent())) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : GetNameSSS_GEvent().\n");//system
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : GetNameSSS_GEvent().\n");//user
		}
		//////////////////////////////////////////////////////////////////////

		//cts_service::_create_test_named_object();
		cts_service::_check_test_named_object();

	}


	void _end_process()
	{
		// end a program started by the service
		HANDLE h_event = ::OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, m_s_instance_event_name.c_str());
		if (h_event) {
			::SetEvent(h_event);
			::CloseHandle(h_event);
			::Sleep(100);
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : send Kill-signal to user application.\n", __WFUNCTION__);
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : user application is already killed..\n", __WFUNCTION__);
		}

		try // close handles to avoid ERROR_NO_SYSTEM_RESOURCES
		{
			::CloseHandle(m_proc_info.hThread);
			::CloseHandle(m_proc_info.hProcess);
		}
		catch (...) {}
	}

	bool _start_processes_different_session()
	{

		DWORD dw_session_id = ::WTSGetActiveConsoleSessionId();
		_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : ActiveConsoleSessionId() = %u.\n", __WFUNCTION__, dw_session_id);
		return _start_processes_different_session(dw_session_id);
	}

	bool _start_processes_different_session(DWORD dw_session_id)
	{
		bool b_result(false);
		DWORD dw_result(0);

		do {
			if (_ns_tools::ct_system::is_exist_named_event(m_s_instance_event_name.c_str())) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : user application is executed already..\n", __WFUNCTION__);
				continue;
			}

			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			BOOL bResult = FALSE;
			DWORD winlogonPid;
			DWORD dwCreationFlags;

			// Find the winlogon process
			PROCESSENTRY32 procEntry;
			HANDLE h_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (h_snap == INVALID_HANDLE_VALUE){
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : CreateToolhelp32Snapshot().\n", __WFUNCTION__);
				continue;
			}

			_ns_tools::ct_warp::chandle handle_snap(h_snap);
			procEntry.dwSize = sizeof(PROCESSENTRY32);

			if (!Process32First(handle_snap.get_handle(), &procEntry)){
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : Process32First().\n", __WFUNCTION__);
				continue;
			}

			std::wstring s_winlogon_exe(L"winlogon.exe");

			do {
				if (s_winlogon_exe.compare(procEntry.szExeFile) == 0) {
					// We found a winlogon process...make sure it's running in the console session
					DWORD dw_win_log_on_session_id = 0;
					if (ProcessIdToSessionId(procEntry.th32ProcessID, &dw_win_log_on_session_id) && dw_win_log_on_session_id == dw_session_id)
					{
						winlogonPid = procEntry.th32ProcessID;
						break;
					}
				}
			} while (Process32Next(h_snap, &procEntry));

			static wchar_t s_desktop[] = L"winsta0\\default";

			dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.lpDesktop = s_desktop;
			ZeroMemory(&pi, sizeof(pi));
			TOKEN_PRIVILEGES tp;
			LUID luid;

			_ns_tools::ct_warp::chandle process_handle(::OpenProcess(MAXIMUM_ALLOWED, FALSE, winlogonPid));
			HANDLE h_ptoken;

			if (!::OpenProcessToken(
				process_handle.get_handle(),
				TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID | TOKEN_READ | TOKEN_WRITE,
				&h_ptoken)
				)
			{
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : OpenProcessToken().\n", __WFUNCTION__);
			}
			_ns_tools::ct_warp::chandle ptoken_handle(h_ptoken);
			if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
			{
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : LookupPrivilegeValue().\n", __WFUNCTION__);
			}
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			HANDLE h_user_token_dup(NULL);
			if (!::DuplicateTokenEx(
				ptoken_handle.get_handle(),
				MAXIMUM_ALLOWED,
				NULL,
				SecurityIdentification,
				TokenPrimary,
				&h_user_token_dup
			)) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : DuplicateTokenEx().\n", __WFUNCTION__);
			}
			_ns_tools::ct_warp::chandle user_token_dup_handle(h_user_token_dup);

			//Adjust Token privilege
			::SetTokenInformation(
				user_token_dup_handle.get_handle(),
				TokenSessionId,
				(void*)dw_session_id,
				sizeof(DWORD)
			);

			if (!AdjustTokenPrivileges(user_token_dup_handle.get_handle(), FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL))
			{
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : AdjustTokenPrivileges().\n", __WFUNCTION__);
			}

			if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
			{
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : AdjustTokenPrivileges() : Token does not have the provilege.\n", __WFUNCTION__);
			}

			LPVOID p_env = NULL;

			if (::CreateEnvironmentBlock(&p_env, user_token_dup_handle.get_handle(), TRUE))
			{
				dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
			}
			else {
				p_env = NULL;
				_ns_tools::ct_log::get_instance().log_fmt(L" : I : %s : CreateEnvironmentBlock().\n", __WFUNCTION__);
			}

			_ns_tools::ct_warp::cenvironment_block env_block(p_env);

			// Launch the process in the client's logon session.
			_ns_tools::type_v_ws_buffer v(_MAX_PATH);
			m_process.s_command_line.copy(&v[0], m_process.s_command_line.size() + 1);

			if (!::CreateProcessAsUser(
				user_token_dup_handle.get_handle(),            // client's access token
				&v[0],              // file to execute
				NULL,     // command line
				NULL,              // pointer to process SECURITY_ATTRIBUTES
				NULL,              // pointer to thread SECURITY_ATTRIBUTES
				FALSE,             // handles are not inheritable
				dwCreationFlags,  // creation flags
				env_block.get_block(),              // pointer to new environment block 
				m_process.s_working_directory.size() == 0 ? NULL : m_process.s_working_directory.c_str(),              // name of current directory 
				&si,               // pointer to STARTUPINFO structure
				&m_proc_info		// receives information about new process
			)) {
				dw_result = GetLastError();
				_ns_tools::ct_log::get_instance().log_fmt(L" : E : %s : CreateProcessAsUser()=%u.\n", __WFUNCTION__, dw_result);
				_ns_tools::ct_log::get_instance().log_fmt(L" : I : user_token_dup_handle = 0x%x.\n", user_token_dup_handle.get_handle());
				_ns_tools::ct_log::get_instance().log_fmt(L" : I : lpApplicationName = %s.\n", &v[0]);
				_ns_tools::ct_log::get_instance().log_fmt(L" : I : dwCreationFlags = 0x%x\n", dwCreationFlags);
				_ns_tools::ct_log::get_instance().log_fmt(L" : I : s_working_directory = %s\n", m_process.s_working_directory.c_str());

				continue;
			}

			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s : starting user application.\n", __WFUNCTION__);

			std::wstring s_msg(L"Process ID : ");

			s_msg += std::to_wstring((unsigned long long)m_proc_info.dwProcessId);
			s_msg += L"\n";

			s_msg += L"Thread ID : ";
			s_msg += std::to_wstring((unsigned long long)m_proc_info.dwThreadId);
			s_msg += L"\n";

			s_msg += L"Process Handle : ";
			s_msg += std::to_wstring((unsigned long long)m_proc_info.hProcess);
			s_msg += L"\n";

			s_msg += L"Thread Handle : ";
			s_msg += std::to_wstring((unsigned long long)m_proc_info.hThread);
			s_msg += L"\n";

			_ns_tools::ct_log::get_instance().log_fmt(L" : I : Process Information\n %s", s_msg.c_str());

			b_result = true;

		} while (false);

		return b_result;
	}

	bool _load_file_xml_structure()
	{
		bool b_result(false);

		do {
			m_xml_list_obj.reset_contents();

			_ns_tools::ct_xml_list::celement_format xml_format;

			xml_format.set_name(L"process");
			xml_format.add_attribute(L"version", _ns_tools::ct_convert::type_value_type::value_type_string);
			xml_format.add_attribute(L"date", _ns_tools::ct_convert::type_value_type::value_type_string);
			//
			_ns_tools::ct_xml_list::type_ptr_element_format ptr_element_device;
			ptr_element_device = xml_format.add_sub_element(L"info", _ns_tools::ct_convert::type_value_type::value_type_string);
			ptr_element_device->add_attribute(L"s_instance_event_name", _ns_tools::ct_convert::type_value_type::value_type_string);
			ptr_element_device->add_attribute(L"s_command_line", _ns_tools::ct_convert::type_value_type::value_type_string);
			ptr_element_device->add_attribute(L"s_working_directory", _ns_tools::ct_convert::type_value_type::value_type_string);

			ptr_element_device->add_attribute(L"n_pause_start", _ns_tools::ct_convert::type_value_type::value_type_int);
			ptr_element_device->add_attribute(L"n_pause_end", _ns_tools::ct_convert::type_value_type::value_type_int);

			ptr_element_device->add_attribute(L"b_user_interface", _ns_tools::ct_convert::type_value_type::value_type_bool);
			ptr_element_device->add_attribute(L"b_restart", _ns_tools::ct_convert::type_value_type::value_type_bool);

			ptr_element_device->add_attribute(L"s_user_name", _ns_tools::ct_convert::type_value_type::value_type_string);
			ptr_element_device->add_attribute(L"s_domain", _ns_tools::ct_convert::type_value_type::value_type_string);
			ptr_element_device->add_attribute(L"s_password", _ns_tools::ct_convert::type_value_type::value_type_string);

			m_xml_list_obj.load_format(xml_format);

			b_result = true;
		} while (false);
		return b_result;
	}
	bool _build_map_obj_to_element()
	{
		/*
		* In this function, get_attribute is used for checking existence of the attribute's value.
		* therefore It is no problem that the attribute's value is converted to string without considering it's type.
		*/
		bool b_result(false);

		_ns_tools::ct_xml_list::celement el_emv_sw = m_xml_list_obj.get_root_element();

		bool b_first(true);
		_ns_tools::ct_xml_list::type_ptr_element el_device;
		std::wstring s_data, s_raw_data;
		_ns_tools::type_v_buffer v_data;
		do {
			el_device = el_emv_sw.find_in_sub_element(L"info", b_first);
			if (el_device == nullptr)
				continue;
			if (!el_device->get_attribute(s_data, L"s_instance_event_name")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"s_command_line")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"s_working_directory")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"n_pause_start")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"n_pause_end")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"b_user_interface")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"b_restart")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"s_user_name")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"s_domain")) {
				continue;
			}
			if (!el_device->get_attribute(s_data, L"s_password")) {
				continue;
			}

			_add_element(L"info", el_device);
			//
			b_result = true;
		} while (false);

		return b_result;
	}
	void _add_element(const std::wstring& s_element_name, const _ns_tools::ct_xml_list::type_ptr_element& ptr_element)
	{
		do {
			if (s_element_name.empty())
				continue;
			auto it = m_map_s_list_obj_element.find(s_element_name);
			if (it == std::end(m_map_s_list_obj_element)) {
				auto result = m_map_s_list_obj_element.insert(std::make_pair(s_element_name, _ns_tools::ct_xml_list::type_list_ptr_element()));
				if (!result.second)
					continue;
				it = result.first;
			}

			if (ptr_element)
				it->second.push_back(ptr_element);
		} while (false);
	}

	bool _load_to_parameter()
	{
		bool b_result(false);

		bool b_value(false);
		int n_value(0);
		unsigned char c_value(0);
		std::wstring s_value;
		size_t n_size(0);

		do {
			m_s_instance_event_name.clear();
			m_process.s_command_line.clear();
			m_process.s_working_directory.clear();
			m_process.n_pause_start = 0;
			m_process.n_pause_end = 0;
			m_process.b_user_interface = true;
			m_process.b_restart = false;
			m_process.s_user_name.clear();
			m_process.s_domain.clear();
			m_process.s_password.clear();

			_ns_tools::type_v_ws_buffer v(_MAX_PATH,0);

			if (!_get_front_value(s_value, L"info", L"s_instance_event_name"))
				continue;
			m_s_instance_event_name = s_value;

			if (!_get_front_value(s_value, L"info", L"s_command_line"))
				continue;
			n_size = ExpandEnvironmentStrings(s_value.c_str(), &v[0], static_cast<DWORD>(v.size()));
			m_process.s_command_line.assign(&v[0], n_size);

			if (!_get_front_value(s_value, L"info", L"s_working_directory"))
				continue;
			n_size = ExpandEnvironmentStrings(s_value.c_str(), &v[0], static_cast<DWORD>(v.size()));
			m_process.s_working_directory.assign(&v[0], n_size);

			if (!_get_front_value(n_value, L"info", L"n_pause_start"))
				continue;
			m_process.n_pause_start = n_value;

			if (!_get_front_value(n_value, L"info", L"n_pause_end"))
				continue;
			m_process.n_pause_end = n_value;

			if (!_get_front_value(b_value, L"info", L"b_user_interface"))
				continue;
			m_process.b_user_interface = b_value;

			if (!_get_front_value(b_value, L"info", L"b_restart"))
				continue;
			m_process.b_restart = b_value;


			if (!_get_front_value(s_value, L"info", L"s_user_name"))
				continue;
			m_process.s_user_name = s_value;

			if (!_get_front_value(s_value, L"info", L"s_domain"))
				continue;
			m_process.s_domain = s_value;

			if (!_get_front_value(s_value, L"info", L"s_password"))
				continue;
			m_process.s_password = s_value;

			//
			b_result = true;

			_ns_tools::ct_log::get_instance().log_fmt(L" : I : s_instance_event_name = %s.\n", m_s_instance_event_name.c_str());
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : s_command_line = %s.\n", m_process.s_command_line.c_str());
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : s_working_directory = %s.\n", m_process.s_working_directory.c_str());
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : n_pause_start = %u.\n", m_process.n_pause_start);
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : n_pause_end = %u.\n", m_process.n_pause_end);
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : b_user_interface = %s.\n", m_process.b_user_interface==true ? L"true":L"false");
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : b_restart = %s.\n", m_process.b_restart == true ? L"true" : L"false");

			_ns_tools::ct_log::get_instance().log_fmt(L" : I : s_user_name = %s.\n", m_process.s_user_name.c_str());
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : s_domain = %s.\n", m_process.s_domain.c_str());
			_ns_tools::ct_log::get_instance().log_fmt(L" : I : s_password = %s.\n", m_process.s_password.c_str());

		} while (false);
		return b_result;
	}

	bool _get_front_value(bool& b_enable, const std::wstring& s_element, const std::wstring& s_attribute)
	{
		bool b_result(false);

		do {
			auto it = m_map_s_list_obj_element.find(s_element);
			if (it == std::end(m_map_s_list_obj_element))
				continue;
			if (it->second.empty())
				continue;
			if (!it->second.front()->get_attribute(b_enable, s_attribute))
				continue;
			b_result = true;
		} while (false);
		return b_result;
	}
	bool _get_front_value(std::wstring& s_value, const std::wstring& s_element, const std::wstring& s_attribute)
	{
		bool b_result(false);

		do {
			auto it = m_map_s_list_obj_element.find(s_element);
			if (it == std::end(m_map_s_list_obj_element))
				continue;
			if (it->second.empty())
				continue;
			if (!it->second.front()->get_attribute(s_value, s_attribute))
				continue;
			b_result = true;
		} while (false);
		return b_result;
	}

	bool _get_front_value(int& n_value, const std::wstring& s_element, const std::wstring& s_attribute)
	{
		bool b_result(false);

		do {
			auto it = m_map_s_list_obj_element.find(s_element);
			if (it == std::end(m_map_s_list_obj_element))
				continue;
			if (it->second.empty())
				continue;
			if (!it->second.front()->get_attribute(n_value, s_attribute))
				continue;
			b_result = true;
		} while (false);
		return b_result;
	}

	static void _create_test_named_object()
	{
		std::wstring s_name;
		int n_index(0);

		s_name = ctest_item::get_test_ns_local_named();
		cts_service::_get_v_warp_ptr_event().emplace_back(
			_ns_tools::ct_warp::cevent::type_ptr_cevent(
				new _ns_tools::ct_warp::cevent(
					_ns_tools::ct_system::create_event_for_ipc(true, false, s_name)
				)
			)
		);

		if (cts_service::_get_v_warp_ptr_event()[n_index]) {
			if (cts_service::_get_v_warp_ptr_event()[n_index]->get_handle()) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : OK : create event get_test_ns_local_named() .\n");
			}
			else {
				_ns_tools::ct_log::get_instance().log_fmt(L" : ER : create event get_test_ns_local_named() #1.\n");
			}

		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : ER : create event get_test_ns_local_named() #2.\n");
		}
		///////
		++n_index;
		s_name = ctest_item::get_test_ns_global_named();
		cts_service::_get_v_warp_ptr_event().emplace_back(
			_ns_tools::ct_warp::cevent::type_ptr_cevent(
				new _ns_tools::ct_warp::cevent(
					_ns_tools::ct_system::create_event_for_ipc(true, false, s_name)
				)
			)
		);

		if (cts_service::_get_v_warp_ptr_event()[n_index]) {
			if (cts_service::_get_v_warp_ptr_event()[n_index]->get_handle()) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : OK : create event get_test_ns_global_named() .\n");
			}
			else {
				_ns_tools::ct_log::get_instance().log_fmt(L" : ER : create event get_test_ns_global_named() #1.\n");
			}

		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : ER : create event get_test_ns_global_named() #2.\n");
		}
		///////
		++n_index;
		s_name = ctest_item::get_test_ns_none_global_named();
		cts_service::_get_v_warp_ptr_event().emplace_back(
			_ns_tools::ct_warp::cevent::type_ptr_cevent(
				new _ns_tools::ct_warp::cevent(
					_ns_tools::ct_system::create_event_for_ipc(true, false, s_name)
				)
			)
		);
		if (cts_service::_get_v_warp_ptr_event()[n_index]) {
			if (cts_service::_get_v_warp_ptr_event()[n_index]->get_handle()) {
				_ns_tools::ct_log::get_instance().log_fmt(L" : OK : create event get_test_ns_none_global_named() .\n");
			}
			else {
				_ns_tools::ct_log::get_instance().log_fmt(L" : ER : create event get_test_ns_none_global_named() #1.\n");
			}

		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : ER : create event get_test_ns_none_global_named() #2.\n");
		}
		///////

	}

	static void _check_test_named_object()
	{
		if (_ns_tools::ct_system::is_exist_named_event(ctest_item::get_test_ns_local_named())) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : get_test_ns_local_named().\n");
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : get_test_ns_local_named().\n");
		}
		//
		if (_ns_tools::ct_system::is_exist_named_event(ctest_item::get_test_ns_global_named())) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : get_test_ns_global_named().\n");
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : get_test_ns_global_named().\n");
		}
		//
		if (_ns_tools::ct_system::is_exist_named_event(ctest_item::get_test_ns_none_global_named())) {
			_ns_tools::ct_log::get_instance().log_fmt(L" : OK : get_test_ns_none_global_named().\n");
		}
		else {
			_ns_tools::ct_log::get_instance().log_fmt(L" : NO : get_test_ns_none_global_named().\n");
		}
	
	}

	static ctest_item::type_v_warp_ptr_event& _get_v_warp_ptr_event()
	{
		static ctest_item::type_v_warp_ptr_event _v_warp_ptr_event;
		return _v_warp_ptr_event;
	}

	bool _run_as_interactive_user_account()
	{
		bool b_result(false);

		do {
			HRESULT hr(0);
			HANDLE h_process_token(NULL);

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &h_process_token)) {
				continue;
			}
			_ns_tools::ct_warp::chandle handle_process_token(h_process_token);
			//
			LUID luid = { 0, };
			if (!LookupPrivilegeValue(NULL, L"SeTcbPrivilege", &luid)) {
				continue;
			}

			TOKEN_PRIVILEGES adj_token_privileges = { 0 }, old_token_privileges = { 0 };
			adj_token_privileges.PrivilegeCount = 1;
			adj_token_privileges.Privileges[0].Luid = luid;
			adj_token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			DWORD dw_old_tp_len(0);
			if (!AdjustTokenPrivileges(
				handle_process_token.get_handle(),
				FALSE,
				&adj_token_privileges,
				sizeof(TOKEN_PRIVILEGES),
				&old_token_privileges,
				&dw_old_tp_len
			)) {
				continue;
			}

			DWORD dw_console_session_id = WTSGetActiveConsoleSessionId();
			if (dw_console_session_id == 0xFFFFFFFF) {
				continue;
			}

			HANDLE h_impersonation_token(0);
			if (!WTSQueryUserToken(
				dw_console_session_id,
				&h_impersonation_token
			)) {
				continue;
			}
			_ns_tools::ct_warp::chandle handle_impersonation_token(h_impersonation_token);

			HANDLE h_user_token(NULL);
			if (!DuplicateTokenEx(
				handle_impersonation_token.get_handle(),
				MAXIMUM_ALLOWED,
				NULL,
				SecurityIdentification,
				TokenPrimary,
				&h_user_token
			)) {
				continue;
			}
			_ns_tools::ct_warp::chandle handle_user_token(h_user_token);

			static wchar_t s_desk_top[] = L"winsta0\\default";
			STARTUPINFO si = { 0 };
			si.cb = sizeof(STARTUPINFO);
			si.lpDesktop = s_desk_top;

			void* lp_enviroment(NULL);
			if (!CreateEnvironmentBlock(
				&lp_enviroment,
				handle_user_token.get_handle(),
				TRUE
			)) {
				continue;
			}
			_ns_tools::ct_warp::cenvironment_block env_block(lp_enviroment);

			_ns_tools::type_v_ws_buffer v(_MAX_PATH);
			m_process.s_command_line.copy(&v[0], m_process.s_command_line.size() + 1);
			PROCESS_INFORMATION process_info = { 0 };
			if (!CreateProcessAsUser(
				handle_user_token.get_handle(),
				&v[0],
				NULL,
				NULL,
				NULL,
				FALSE,
				CREATE_UNICODE_ENVIRONMENT,
				env_block.get_block(),
				m_process.s_working_directory.size() == 0 ? NULL : m_process.s_working_directory.c_str(),
				&si,
				&m_proc_info
			)) {
				continue;
			}
			_ns_tools::ct_log::get_instance().log_fmt(L" : S : %s : starting user application.\n", __WFUNCTION__);
			//
			b_result = true;
		} while (false);
		return b_result;
	}

private:
	bool m_b_ini;

	std::mutex m_mutex;

	SERVICE_STATUS m_service_status;
	SERVICE_STATUS_HANDLE m_h_service_status;

	_ns_tools::type_v_ws_buffer m_v_dup_service_name;

	std::wstring m_s_service_name;
	std::wstring m_s_exe_file_name;

	cts_service::_process_para m_process;

	PROCESS_INFORMATION m_proc_info;

	SERVICE_TABLE_ENTRY m_dispatch_table[cts_service::const_size_table];

	HANDLE m_h_kill_event_user_app;

	std::wstring m_s_instance_event_name;

	_ns_tools::ct_xml_list m_xml_list_obj;
	_ns_tools::ct_xml_list::type_map_string_list_ptr_element m_map_s_list_obj_element;

private: //don't call these methods
	cts_service(const cts_service&);
	cts_service& operator=(const cts_service&);

};

