# Microsoft Developer Studio Project File - Name="syx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=syx - Win32 Release_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "syx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "syx.mak" CFG="syx - Win32 Release_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "syx - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "syx - Win32 Release_NTS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "syx - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "syx - Win32 Debug_NTS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "syx - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\" /I "..\.." /I "..\..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_SYX" /D ZTS=1 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYX_EXPORTS" /D "COMPILE_DL_SYX" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_SYX=1 /D "LIBZEND_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_syx.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "syx - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\" /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_SYX" /D ZTS=1 /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYX_EXPORTS" /D "COMPILE_DL_SYX" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_SYX=1 /D "LIBZEND_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts_debug.lib /nologo /dll /machine:I386 /out:"..\..\Debug_TS/php_syx.dll" /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "syx - Win32 Release_NTS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_NTS"
# PROP BASE Intermediate_Dir "Release_NTS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_NTS"
# PROP Intermediate_Dir "Release_NTS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\" /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_SYX" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYX_EXPORTS" /D "COMPILE_DL_SYX" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_SYX=1 /D "LIBZEND_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5.lib /nologo /dll /machine:I386 /out:"..\..\Release_NTS/php_syx.dll" /libpath:"..\..\Release_NTS"

!ELSEIF  "$(CFG)" == "syx - Win32 Debug_NTS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_NTS"
# PROP BASE Intermediate_Dir "Debug_NTS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_NTS"
# PROP Intermediate_Dir "Debug_NTS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\" /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_SYX" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYX_EXPORTS" /D "COMPILE_DL_SYX" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_SYX=1 /D "LIBZEND_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5_debug.lib /nologo /dll /machine:I386 /out:"..\..\Debug_NTS/php_syx.dll" /libpath:"..\..\Debug_NTS"

!ENDIF 

# Begin Target

# Name "syx - Win32 Release_TS"
# Name "syx - Win32 Debug_TS"
# Name "syx - Win32 Release_NTS"
# Name "syx - Win32 Debug_NTS"
# Begin Group "Syx Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\syx.c
# End Source File
# Begin Source File

SOURCE=.\syx_application.c
# End Source File
# Begin Source File

SOURCE=.\syx_loader.c
# End Source File
# Begin Source File

SOURCE=.\syx_dispatcher.c
# End Source File
# Begin Source File

SOURCE=.\syx_bootstrap.c
# End Source File
# Begin Source File

SOURCE=.\syx_config.c
# End Source File
# Begin Source File

SOURCE=.\configs\syx_config_ini.c
# End Source File
# Begin Source File

SOURCE=.\configs\syx_config_simple.c
# End Source File
# Begin Source File

SOURCE=.\syx_registry.c
# End Source File
# Begin Source File

SOURCE=.\syx_controller.c
# End Source File
# Begin Source File

SOURCE=.\syx_action.c
# End Source File
# Begin Source File

SOURCE=.\syx_view.c
# End Source File
# Begin Source File

SOURCE=.\views\syx_view_interface.c
# End Source File
# Begin Source File

SOURCE=.\views\syx_view_simple.c
# End Source File
# Begin Source File

SOURCE=.\syx_request.c
# End Source File
# Begin Source File

SOURCE=.\requests\syx_request_http.c
# End Source File
# Begin Source File

SOURCE=.\requests\syx_request_simple.c
# End Source File
# Begin Source File

SOURCE=.\syx_response.c
# End Source File
# Begin Source File

SOURCE=.\responses\syx_response_cli.c
# End Source File
# Begin Source File

SOURCE=.\responses\syx_response_http.c
# End Source File
# Begin Source File

SOURCE=.\syx_router.c
# End Source File
# Begin Source File

SOURCE=.\routes\syx_route_interface.c
# End Source File
# Begin Source File

SOURCE=.\routes\syx_route_static.c
# End Source File
# Begin Source File

SOURCE=.\routes\syx_route_simple.c
# End Source File
# Begin Source File

SOURCE=.\routes\syx_route_supervar.c
# End Source File
# Begin Source File

SOURCE=.\routes\syx_route_regex.c
# End Source File
# Begin Source File

SOURCE=.\routes\syx_route_rewrite.c
# End Source File
# Begin Source File

SOURCE=.\routes\syx_route_map.c
# End Source File
# Begin Source File

SOURCE=.\syx_exception.c
# End Source File
# Begin Source File

SOURCE=.\syx_plugin.c
# End Source File
# Begin Source File

SOURCE=.\syx_session.c
# End Source File

# End Group
# Begin Group "Syx Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_syx.h
# End Source File
# Begin Source File

SOURCE=.\syx_application.h
# End Source File
# Begin Source File

SOURCE=.\syx_bootstrap.h
# End Source File
# Begin Source File

SOURCE=.\syx_loader.h
# End Source File
# Begin Source File

SOURCE=.\syx_config.h
# End Source File
# Begin Source File

SOURCE=.\syx_registry.h
# End Source File
# Begin Source File

SOURCE=.\syx_controller.h
# End Source File
# Begin Source File

SOURCE=.\syx_view.h
# End Source File
# Begin Source File

SOURCE=.\php_request.h
# End Source File
# Begin Source File

SOURCE=.\syx_router.h
# End Source File
# Begin Source File

SOURCE=.\syx_exception.h
# End Source File
# Begin Source File

SOURCE=.\syx_plugin.h
# End Source File
# Begin Source File

SOURCE=.\syx_session.h
# End Source File
# End Group
# End Target
# End Project
