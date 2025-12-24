# Microsoft Developer Studio Project File - Name="XMWinExe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=XMWinExe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XMWinExe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XMWinExe.mak" CFG="XMWinExe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XMWinExe - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "XMWinExe - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XMWinExe - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "_VISUALC_" /D "NODISP" /D M7XMVERSION=\"6.1\" /D "AFLIB" /D "USEXML" /FR /YX /FD "c_plusplus" /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 mpeg2declib.lib XMLibB.lib XMLibC.lib XMLibAP.lib XMLibA.lib XMLibV.lib XMLibM.lib kltlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib cv.lib cxcore.lib ..\lib\qhull.lib ..\..\afsp\MSVC\lib\libtsp.lib ..\..\afsp\MSVC\lib\libAO.lib ..\..\ImageMagick\lib\Imagick.lib ..\..\xml\lib\xerces-c_1.lib /nologo /subsystem:console /machine:I386 /out:"..\XMWinExe.exe" /libpath:"../lib" /libpath:"C:\Program Files\OpenCV\lib"

!ELSEIF  "$(CFG)" == "XMWinExe - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "_DEBUG" /D "DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D M7XMVERSION=\"6.1\" /D "AFLIB" /D "USEXML" /FR /YX /FD /c
# SUBTRACT CPP /X /u
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mpeg2declibDB.lib XMLibBDB.lib XMLibCDB.lib XMLibAPDB.lib XMLibADB.lib XMLibVDB.lib XMLibMDB.lib kltlibDB.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib cv.lib cxcore.lib ..\lib\qhull.lib ..\..\afsp\MSVC\lib\libtsp.lib ..\..\afsp\MSVC\lib\libAO.lib ..\..\ImageMagick\lib\Imagick.lib ..\..\xml\lib\xerces-c_1.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\XMWinExe_dbg.exe" /pdbtype:sept /libpath:"../lib" /libpath:"C:\Program Files\OpenCV\lib"

!ENDIF 

# Begin Target

# Name "XMWinExe - Win32 Release"
# Name "XMWinExe - Win32 Debug"
# Begin Source File

SOURCE=..\XMManager\XMMain.cpp
# End Source File
# End Target
# End Project
