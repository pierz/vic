# Microsoft Developer Studio Project File - Name="common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=common - Win32 Debug IPv6
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "common.mak" CFG="common - Win32 Debug IPv6"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "common - Win32 Debug IPv6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "\src\tcl-8.0\generic" /I "\src\tk-8.0\generic" /I "\src\tk-8.0\xlib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /D "DEBUG_MEM" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\uclmm.lib"

!ELSEIF  "$(CFG)" == "common - Win32 Debug IPv6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "common___Win32_Debug_IPv6"
# PROP BASE Intermediate_Dir "common___Win32_Debug_IPv6"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "common___Win32_Debug_IPv6"
# PROP Intermediate_Dir "common___Win32_Debug_IPv6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "\src\tcl-8.0\generic" /I "\src\tk-8.0\generic" /I "\src\tk-8.0\xlib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /D "DEBUG_MEM" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "\src\tcl-8.0\generic" /I "\src\tk-8.0\generic" /I "\src\tk-8.0\xlib" /I "\DDK\inc" /I "\src\msripv6\inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /D "DEBUG_MEM" /D "HAVE_IPv6" /D "BUILD_tcl" /D "BUILD_tk" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\uclmm.lib"
# ADD LIB32 /nologo /out:"Debug\uclmm.lib"

!ENDIF 

# Begin Target

# Name "common - Win32 Release"
# Name "common - Win32 Debug"
# Name "common - Win32 Debug IPv6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\base64.c
# End Source File
# Begin Source File

SOURCE=.\crypt_random.c
# End Source File
# Begin Source File

SOURCE=.\debug.c
# End Source File
# Begin Source File

SOURCE=.\hmac.c
# End Source File
# Begin Source File

SOURCE=.\mbus.c
# End Source File
# Begin Source File

SOURCE=.\md5.c
# End Source File
# Begin Source File

SOURCE=.\memory.c
# End Source File
# Begin Source File

SOURCE=.\net_udp.c
# End Source File
# Begin Source File

SOURCE=.\qfDES.c
# End Source File
# Begin Source File

SOURCE=.\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\config_unix.h
# End Source File
# Begin Source File

SOURCE=.\config_win32.h
# End Source File
# Begin Source File

SOURCE=.\crypt_random.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\hmac.h
# End Source File
# Begin Source File

SOURCE=.\mbus.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\memory.h
# End Source File
# Begin Source File

SOURCE=.\net_udp.h
# End Source File
# Begin Source File

SOURCE=.\qfDES.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# End Target
# End Project