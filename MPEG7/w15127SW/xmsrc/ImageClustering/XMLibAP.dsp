# Microsoft Developer Studio Project File - Name="XMLibAP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XMLibAP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XMLibAP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XMLibAP.mak" CFG="XMLibAP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XMLibAP - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XMLibAP - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XMLibAP - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\AddressLib" /I ".." /I "..\..\xml\include" /I "..\..\xml\src" /I "..\..\afsp\include" /I "..\..\ImageMagick" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "USEXML" /D "USEDC" /D "USESCC" /D "USEHT" /D "USECL" /D "USEEH" /D "USECT" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\XMLibAP.lib"

!ELSEIF  "$(CFG)" == "XMLibAP - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\AddressLib" /I ".." /I "..\..\xml\include" /I "..\..\xml\src" /I "..\..\afsp\include" /I "..\..\ImageMagick" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "USEXML" /D "USEDC" /D "USESCC" /D "USEHT" /D "USECL" /D "USEEH" /D "USECT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\XMLibAPDB.lib"

!ENDIF 

# Begin Target

# Name "XMLibAP - Win32 Release"
# Name "XMLibAP - Win32 Debug"
# Begin Group "BasicModules"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ClientApplications.h
# End Source File
# Begin Source File

SOURCE=..\XMManager\ComponentDB.cpp
# End Source File
# Begin Source File

SOURCE=..\XMManager\ComponentDB.h
# End Source File
# Begin Source File

SOURCE=.\MyExif.cpp
# End Source File
# Begin Source File

SOURCE=.\MyExif.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ServerApplications.h
# End Source File
# Begin Source File

SOURCE=.\Version1.cpp
# End Source File
# Begin Source File

SOURCE=.\Version1.h
# End Source File
# End Group
# Begin Group "DominantColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DominantColorCAppl.cpp
# End Source File
# Begin Source File

SOURCE=.\DominantColorCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\DominantColorSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\DominantColorSAppl.h
# End Source File
# End Group
# Begin Group "HomogeneousTexture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HomoTextureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=.\HomoTextureCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\HomoTextureSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\HomoTextureSAppl.h
# End Source File
# End Group
# Begin Group "ColorStructure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ColorStructureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorStructureCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorStructureSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorStructureSAppl.h
# End Source File
# End Group
# Begin Group "ColorLayout"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ColorLayoutCAppl.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorLayoutCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorLayoutSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorLayoutSAppl.h
# End Source File
# End Group
# Begin Group "ScalableColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ScalableColorCAppl.cpp
# End Source File
# Begin Source File

SOURCE=.\ScalableColorCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ScalableColorSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ScalableColorSAppl.h
# End Source File
# End Group
# Begin Group "EdgeHistogram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EdgeHistCAppl.cpp
# End Source File
# Begin Source File

SOURCE=.\EdgeHistCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\EdgeHistSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\EdgeHistSAppl.h
# End Source File
# End Group
# End Target
# End Project
