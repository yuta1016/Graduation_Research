# Microsoft Developer Studio Project File - Name="XMLibC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XMLibC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XMLibC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XMLibC.mak" CFG="XMLibC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XMLibC - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XMLibC - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XMLibC - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\AddressLib" /I ".." /I "..\..\xml\include" /I "..\..\xml\src" /I "..\..\afsp\include" /I "..\..\ImageMagick" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "USEXML" /FD /c
# SUBTRACT CPP /X /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\XMLibC.lib"

!ELSEIF  "$(CFG)" == "XMLibC - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\AddressLib" /I ".." /I "..\..\xml\include" /I "..\..\xml\src" /I "..\..\afsp\include" /I "..\..\ImageMagick" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "USEXML" /FR /FD /c
# SUBTRACT CPP /X /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\XMLibCDB.lib"

!ENDIF 

# Begin Target

# Name "XMLibC - Win32 Release"
# Name "XMLibC - Win32 Debug"
# Begin Group "AddressLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AddressLib\address.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\AddressLib\address.h
# End Source File
# Begin Source File

SOURCE=..\AddressLib\fifo.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\AddressLib\fifo.h
# End Source File
# Begin Source File

SOURCE=..\AddressLib\inter.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\AddressLib\inter.h
# End Source File
# Begin Source File

SOURCE=..\AddressLib\intra.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\AddressLib\intra.h
# End Source File
# Begin Source File

SOURCE=..\AddressLib\lifo.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\AddressLib\lifo.h
# End Source File
# Begin Source File

SOURCE=..\AddressLib\momusys.h
# End Source File
# Begin Source File

SOURCE=..\AddressLib\nhood.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\AddressLib\nhood.h
# End Source File
# Begin Source File

SOURCE=..\AddressLib\vopio.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\AddressLib\vopio.h
# End Source File
# End Group
# Begin Group "BasicArchitecture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\BasicArchitecture\XMABCInterfaces.cpp
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMABCInterfaces.h
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMABCObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMABCObjects.h
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMInterfaces.cpp
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMInterfaces.h
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMObjects.h
# End Source File
# Begin Source File

SOURCE=..\BasicArchitecture\XMTypes.h
# End Source File
# End Group
# Begin Group "BasicModules"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Media\AudioFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\Media\AudioFrame.h
# End Source File
# Begin Source File

SOURCE=..\Utilities\BitBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\Utilities\BitBuffer.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\CodingSchemes.h
# End Source File
# Begin Source File

SOURCE=..\Descriptors\Descriptors.h
# End Source File
# Begin Source File

SOURCE=..\Utilities\DoubleArray.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\Utilities\DoubleArray.h
# End Source File
# Begin Source File

SOURCE=..\Utilities\DoubleArrayMatrix.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\Utilities\DoubleArrayMatrix.h
# End Source File
# Begin Source File

SOURCE=..\Utilities\DoubleVector.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\Utilities\DoubleVector.h
# End Source File
# Begin Source File

SOURCE=.\ExtractionUtilities.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\imgutil.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\imgutil.h
# End Source File
# Begin Source File

SOURCE=..\Utilities\LinkedList.c
# SUBTRACT CPP /D "c_plusplus"
# End Source File
# Begin Source File

SOURCE=..\Utilities\LinkedList.h
# End Source File
# Begin Source File

SOURCE=..\Media\MultiMedia.cpp
# End Source File
# Begin Source File

SOURCE=..\Media\MultiMedia.h
# End Source File
# Begin Source File

SOURCE=.\readparam.cpp
# End Source File
# Begin Source File

SOURCE=.\readparam.h
# End Source File
# Begin Source File

SOURCE=..\Descriptors\RegisteredDescriptors.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\RegisteredExtractionUtilities.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\RegisteredSearchTools.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SearchUtilities.h
# End Source File
# Begin Source File

SOURCE=..\Utilities\Utilities.h
# End Source File
# Begin Source File

SOURCE=..\Utilities\UUIDs.cpp
# End Source File
# Begin Source File

SOURCE=..\Utilities\UUIDs.h
# End Source File
# End Group
# Begin Group "GenericDS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DescriptionSchemes\DescriptionSchemes.h
# End Source File
# Begin Source File

SOURCE=..\DescriptionSchemes\GenericDS.cpp
# End Source File
# Begin Source File

SOURCE=..\DescriptionSchemes\GenericDS.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\GenericDSCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\GenericDSCS.h
# End Source File
# Begin Source File

SOURCE=..\DescriptionSchemes\RegisteredDescriptionSchemes.cpp
# End Source File
# Begin Source File

SOURCE=..\DescriptionSchemes\VectorR.cpp
# End Source File
# Begin Source File

SOURCE=..\DescriptionSchemes\VectorR.h
# End Source File
# Begin Source File

SOURCE=..\DDL\XML4Cutils.cpp
# End Source File
# Begin Source File

SOURCE=..\DDL\XML4Cutils.h
# End Source File
# End Group
# End Target
# End Project
