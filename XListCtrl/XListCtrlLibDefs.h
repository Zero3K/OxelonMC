#ifndef XLISTCTRLLIBDEFS_H
#define XLISTCTRLLIBDEFS_H

#define XLISTCTRLLIB_STATIC

#if defined XLISTCTRLLIB_STATIC
	#define XLISTCTRLLIBDLLEXPORT
#else
	#ifdef XLISTCTRLLIB_EXPORTS
		#define XLISTCTRLLIBDLLEXPORT _declspec(dllexport)
	#else
		#define XLISTCTRLLIBDLLEXPORT _declspec(dllimport)
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// The first two letters of the library name specify the MFC/XListCtrl
// build configuration as follows:
//       MFC     MFC      XListCtrl    XListCtrl
//       DLL    Static       DLL        Static
// DD     x                   x                
// DS     x                               x    
// SS             x                       x    
// SD    -----------  not provided  -----------  
//
// The third letter specifies Release/Debug.
// The fourth letter specifies ANSI/UNICODE.
///////////////////////////////////////////////////////////////////////////////

#ifndef XLISTCTRLLIB_NOAUTOLIB
	#if defined _AFXDLL && !defined XLISTCTRLLIB_STATIC
		// MFC shared DLL with XListCtrl shared DLL
		#ifdef _UNICODE	
			#ifdef _DEBUG
				#pragma comment(lib,"XListCtrlDDDU.lib")
				#pragma message("Automatically linking with XListCtrlDDDU.lib")
			#else
				#pragma comment(lib,"XListCtrlDDRU.lib")
				#pragma message("Automatically linking with XListCtrlDDRU.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib,"XListCtrlDDDA.lib")
				#pragma message("Automatically linking with XListCtrlDDDA.lib")
			#else
				#pragma comment(lib,"XListCtrlDDRA.lib")
				#pragma message("Automatically linking with XListCtrlDDRA.lib")
			#endif
		#endif
	#elif defined _AFXDLL && defined XLISTCTRLLIB_STATIC
		// MFC shared DLL with XListCtrl static lib
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma comment(lib,"XListCtrlDSDU.lib")
				#pragma message("Automatically linking with XListCtrlDSDU.lib")
			#else
				#pragma comment(lib,"XListCtrlDSRU.lib")
				#pragma message("Automatically linking with XListCtrlDSRU.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib,"XListCtrlDSDA.lib")
				#pragma message("Automatically linking with XListCtrlDSDA.lib")
			#else
				#pragma comment(lib,"XListCtrlDSRA.lib")
				#pragma message("Automatically linking with XListCtrlDSRA.lib")
			#endif
		#endif
	#elif !defined _AFXDLL && defined XLISTCTRLLIB_STATIC
		// MFC static lib with XListCtrl static lib
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma comment(lib,"XListCtrlSSDU.lib")
				#pragma message("Automatically linking with XListCtrlSSDU.lib")
			#else
				#pragma comment(lib,"XListCtrlSSRU.lib")
				#pragma message("Automatically linking with XListCtrlSSRU.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib,"XListCtrlSSDA.lib")
				#pragma message("Automatically linking with XListCtrlSSDA.lib")
			#else
				#pragma comment(lib,"XListCtrlSSRA.lib")
				#pragma message("Automatically linking with XListCtrlSSRA.lib")
			#endif
		#endif
	#else
		#pragma message(" ")
		#pragma message("--------------------------------------------------------------------------------")
		#pragma message(" The SD build configuration (MFC static, XListCtrl DLL) is not available. ")
		#pragma message("--------------------------------------------------------------------------------")
		#pragma message(" ")
		#error This build configuration is not available.
	#endif
#endif


#endif //XLISTCTRLLIBDEFS_H
