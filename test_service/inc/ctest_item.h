#pragma once
#include <string>

#include <Windows.h>

#include <ct_system.h>

class ctest_item
{
public:
	typedef	std::vector<_ns_tools::ct_warp::cevent::type_ptr_cevent>		type_v_warp_ptr_event;

public:
	//return shareQ name
	// (event or mutex) = ( returned GetNameShareQ+L"_EVENT_", returned GetNameShareQ+L"_MUTEXT_" )
	//created by NDM
	static const std::wstring& GetNameShareQ()
	{
		static std::wstring sNameShareQ(L"_Next_Generation_Share_Queue_20101006_");
		return sNameShareQ;
	}

	//return Device single instance mutex name
	// mutex
	//created by NDM
	static const std::wstring& GetNameNDMInstatnce()
	{
		static std::wstring sNameNDMInstatnce(L"_Next_Generation_NDM_single_instance_20101006_");
		return sNameNDMInstatnce;
	}

	//return Device single instance mutex name
	// mutex
	//created by NDM
	static const std::wstring& GetNameNDMSetupOk()
	{
		static std::wstring sNameNDMSetupOk(L"_Next_Generation_NDM_setup_ok_20101006_");
		return sNameNDMSetupOk;
	}

	//return Next device manager kill event name
	// event
	//created by NDM
	static const std::wstring& GetNameNDMKill()
	{
		static std::wstring sNameNDMKill(L"_Next_Generation_NDM_Kill_20101007_");
		return sNameNDMKill;
	}

	//return Next device start stop service global event.
	// event
	//created by NDM
	static const std::wstring& GetNameSSS_GEvent()
	{
		static std::wstring sName(L"Global\\_Next_Generation_SSS_GEvent_20101119_");
		return sName;
	}

	// called from GetNameDevShareQ
	static const std::wstring& GetNameDevShareQPrefix()
	{
		static std::wstring sNameShareQPreFix(L"_NG_Device_Share_Queue_20101019_");
		return sNameShareQPreFix;
	}

	// (event or mutex) = ( returned sNameDevShareQ+L"_EVENT_", returned sNameDevShareQ+L"_MUTEXT_" )
	//created by NDM
	static void GetNameDevShareQ(std::wstring& sNameDevShareQ, HANDLE hDev)
	{
		sNameDevShareQ = ctest_item::GetNameDevShareQPrefix() + std::to_wstring((unsigned long long)hDev);
	}

	/**
	* added for testing
	* 
	*/
	static const std::wstring& get_test_ns_local_named()
	{
		static std::wstring s_name(L"Local\\092A3705-D55E-4A39-8BE4-E6AE19A2D44A");
		return s_name;
	}
	static const std::wstring& get_test_ns_global_named()
	{
		static std::wstring s_name(L"Global\\4529E682-9876-4160-8E9D-0E38B8A770C3");
		return s_name;
	}
	static const std::wstring& get_test_ns_none_global_named()
	{
		static std::wstring s_name(L"7E8047F4-98B0-440C-B6C0-F6D9D81AD0F3");
		return s_name;
	}

	static const std::wstring& get_test_ns_auto_local_named()
	{
		static std::wstring s_name(L"Local\\AUTO-092A3705-D55E-4A39-8BE4-E6AE19A2D44A");
		return s_name;
	}
	static const std::wstring& get_test_ns_auto_global_named()
	{
		static std::wstring s_name(L"Global\\AUTO-4529E682-9876-4160-8E9D-0E38B8A770C3");
		return s_name;
	}
	static const std::wstring& get_test_ns_auto_none_global_named()
	{
		static std::wstring s_name(L"AUTO-7E8047F4-98B0-440C-B6C0-F6D9D81AD0F3");
		return s_name;
	}

};

