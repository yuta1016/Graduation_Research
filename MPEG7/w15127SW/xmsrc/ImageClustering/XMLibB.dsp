# Microsoft Developer Studio Project File - Name="XMLibB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XMLibB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XMLibB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XMLibB.mak" CFG="XMLibB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XMLibB - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XMLibB - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XMLibB - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "USEXML" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\XMLibB.lib"

!ELSEIF  "$(CFG)" == "XMLibB - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "USEXML" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\XMLibBDB.lib"

!ENDIF 

# Begin Target

# Name "XMLibB - Win32 Release"
# Name "XMLibB - Win32 Debug"
# Begin Group "BasicModules"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Decoders\DecoderFileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Decoders\DecoderFileIO.h
# End Source File
# Begin Source File

SOURCE=..\Decoders\Decoders.h
# End Source File
# Begin Source File

SOURCE=..\Encoders\EncoderFileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Encoders\EncoderFileIO.h
# End Source File
# Begin Source File

SOURCE=..\Encoders\Encoders.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MatchList.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MatchList.h
# End Source File
# Begin Source File

SOURCE=..\Media\Media.h
# End Source File
# Begin Source File

SOURCE=..\Media\MediaIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Media\MediaIO.h
# End Source File
# Begin Source File

SOURCE=..\DDL\RegisteredDDLs.cpp
# End Source File
# Begin Source File

SOURCE=..\Decoders\RegisteredDecoders.cpp
# End Source File
# Begin Source File

SOURCE=..\Encoders\RegisteredEncoders.cpp
# End Source File
# Begin Source File

SOURCE=..\ParsingUtilities\RegisteredParsers.cpp
# End Source File
# Begin Source File

SOURCE=..\SystemTools\RegisteredSystems.cpp
# End Source File
# Begin Source File

SOURCE=..\Media\WrlFileUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\Media\WrlFileUtil.h
# End Source File
# End Group
# Begin Group "EvaluationTools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\EvaluationTools\EvaluationTools.cpp
# End Source File
# Begin Source File

SOURCE=..\EvaluationTools\EvaluationTools.h
# End Source File
# Begin Source File

SOURCE=..\EvaluationTools\RegisteredEvalTools.cpp
# End Source File
# End Group
# End Target
# End Project
