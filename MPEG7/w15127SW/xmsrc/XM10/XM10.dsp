# Microsoft Developer Studio Project File - Name="XM10" - Package Owner=<4>

# Microsoft Developer Studio Generated Build File, Format Version 5.00

# ** DO NOT EDIT **



# TARGTYPE "Win32 (x86) Static Library" 0x0104



CFG=XM10 - Win32 Debug

!MESSAGE This is not a valid makefile. To build this project using NMAKE,

!MESSAGE use the Export Makefile command and run

!MESSAGE 

!MESSAGE NMAKE /f "XM10.mak".

!MESSAGE 

!MESSAGE You can specify a configuration when running NMAKE

!MESSAGE by defining the macro CFG on the command line. For example:

!MESSAGE 

!MESSAGE NMAKE /f "XM10.mak" CFG="XM10 - Win32 Debug"

!MESSAGE 

!MESSAGE Possible choices for configuration are:

!MESSAGE 

!MESSAGE "XM10 - Win32 Release" (based on "Win32 (x86) Static Library")

!MESSAGE "XM10 - Win32 Debug" (based on "Win32 (x86) Static Library")

!MESSAGE 



# Begin Project

# PROP Scc_ProjName ""

# PROP Scc_LocalPath ""

CPP=cl.exe



!IF  "$(CFG)" == "XM10 - Win32 Release"



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

# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c

BSC32=bscmake.exe

# ADD BASE BSC32 /nologo

# ADD BSC32 /nologo

LIB32=link.exe -lib

# ADD BASE LIB32 /nologo

# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "XM10 - Win32 Debug"



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

# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\BasicArchitecture" /I "..\XMManager" /I "..\Utilities" /YX /FD /c

BSC32=bscmake.exe

# ADD BASE BSC32 /nologo

# ADD BSC32 /nologo

LIB32=link.exe -lib

# ADD BASE LIB32 /nologo

# ADD LIB32 /nologo



!ENDIF 



# Begin Target



# Name "XM10 - Win32 Release"

# Name "XM10 - Win32 Debug"

# Begin Group "XMManager"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\XMManager\ComponentDB.cpp

# End Source File

# Begin Source File



SOURCE=..\XMManager\ComponentDB.h

# End Source File

# Begin Source File



SOURCE=..\XMManager\ConnectionControl.cpp

# End Source File

# Begin Source File



SOURCE=..\XMManager\XMConnectionControl.h

# End Source File

# End Group

# Begin Group "Utilities"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\Utilities\Streams.cpp

# End Source File

# Begin Source File



SOURCE=..\Utilities\Streams.h

# End Source File

# Begin Source File



SOURCE=..\Utilities\UUIDs.cpp

# End Source File

# Begin Source File



SOURCE=..\Utilities\UUIDs.h

# End Source File

# End Group

# Begin Group "SystemTools"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\SystemTools\RegisteredSystems.cpp

# End Source File

# End Group

# Begin Group "SearchUtilities"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\SearchUtilities\RegisteredSearchTools.cpp

# End Source File

# End Group

# Begin Group "ParsingUtilities"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\ParsingUtilities\RegisteredParsers.cpp

# End Source File

# End Group

# Begin Group "ExtractionUtilities"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\ExtractionUtilities\ExtractionUtilities.cpp

# End Source File

# Begin Source File



SOURCE=..\ExtractionUtilities\ExtractionUtilities.h

# End Source File

# Begin Source File



SOURCE=..\ExtractionUtilities\RegisteredExtractionUtilities.cpp

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

# Begin Group "Encoders"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\Encoders\RegisteredEncoders.cpp

# End Source File

# End Group

# Begin Group "Descriptors"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\Descriptors\descriptors.cpp

# End Source File

# Begin Source File



SOURCE=..\Descriptors\Descriptors.h

# End Source File

# Begin Source File



SOURCE=..\Descriptors\RegisteredDescriptors.cpp

# End Source File

# End Group

# Begin Group "DescriptionSchemes"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\DescriptionSchemes\DescriptionSchemes.cpp

# End Source File

# Begin Source File



SOURCE=..\DescriptionSchemes\DescriptionSchemes.h

# End Source File

# Begin Source File



SOURCE=..\DescriptionSchemes\RegisteredDescriptionSchemes.cpp

# End Source File

# End Group

# Begin Group "Decoders"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\Decoders\RegisteredDecoders.cpp

# End Source File

# End Group

# Begin Group "DDL"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\Ddl\RegisteredDDLs.cpp

# End Source File

# End Group

# Begin Group "CodingSchemes"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\CodingSchemes\CodingSchemes.cpp

# End Source File

# Begin Source File



SOURCE=..\CodingSchemes\CodingSchemes.h

# End Source File

# Begin Source File



SOURCE=..\CodingSchemes\RegisteredCodingSchemes.cpp

# End Source File

# End Group

# Begin Group "BasicArchitecture"



# PROP Default_Filter ""

# Begin Group "source"



# PROP Default_Filter "*.cpp"

# Begin Source File



SOURCE=..\BasicArchitecture\XMABCInterfaces.cpp

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMABCObjects.cpp

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMInterfaces.cpp

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMObjects.cpp

# End Source File

# End Group

# Begin Group "include"



# PROP Default_Filter "*.h"

# Begin Source File



SOURCE=..\BasicArchitecture\MediaDescriptor.h

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMABCInterfaces.h

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMABCObjects.h

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMInterfaces.h

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMObjects.h

# End Source File

# Begin Source File



SOURCE=..\BasicArchitecture\XMTypes.h

# End Source File

# End Group

# End Group

# Begin Group "Applications"



# PROP Default_Filter ""

# Begin Source File



SOURCE=..\Applications\RegisteredClientApplications.cpp

# End Source File

# Begin Source File



SOURCE=..\Applications\RegisteredServerApplications.cpp

# End Source File

# Begin Source File



SOURCE=..\Applications\ServerApplications.cpp

# End Source File

# Begin Source File



SOURCE=..\Applications\ServerApplications.h

# End Source File

# End Group

# End Target

# End Project

