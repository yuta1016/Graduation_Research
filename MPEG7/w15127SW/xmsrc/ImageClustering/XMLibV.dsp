# Microsoft Developer Studio Project File - Name="XMLibV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XMLibV - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XMLibV.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XMLibV.mak" CFG="XMLibV - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XMLibV - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XMLibV - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XMLibV - Win32 Release"

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
# ADD LIB32 /nologo /out:"lib\XMLibV.lib"

!ELSEIF  "$(CFG)" == "XMLibV - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\AddressLib" /I ".." /I "..\..\xml\include" /I "..\..\xml\src" /I "..\..\afsp\include" /I "..\..\ImageMagick" /D "_DEBUG" /D "DEBUG" /D "USECS" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "USEXML" /D "USEDC" /D "USESCC" /D "USEHT" /D "USECL" /D "USEEH" /D "USECT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\XMLibVDB.lib"

!ENDIF 

# Begin Target

# Name "XMLibV - Win32 Release"
# Name "XMLibV - Win32 Debug"
# Begin Group "DominantColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\DominantColor.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\DominantColor.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\DominantColorCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\DominantColorCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\DominantColorExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\DominantColorExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\DominantColorSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\DominantColorSearch.h
# End Source File
# End Group
# Begin Group "HomogeneousTexture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\homo_texture_extractor.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\homo_texture_extractor.h
# End Source File
# Begin Source File

SOURCE=..\Descriptors\HomoTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\HomoTexture.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\HomoTextureCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\HomoTextureCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\HomoTextureExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\HomoTextureExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\HomoTextureSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\HomoTextureSearch.h
# End Source File
# End Group
# Begin Group "ColorStructure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\ColorStructure.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ColorStructure.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorStructureCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorStructureCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorStructureExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorStructureExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ColorStructureSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ColorStructureSearch.h
# End Source File
# End Group
# Begin Group "ColorSpace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\ColorSpace.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ColorSpace.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorSpaceCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorSpaceCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorSpaceExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorSpaceExtraction.h
# End Source File
# End Group
# Begin Group "ColorLayout"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\ColorLayout.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ColorLayout.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorLayoutCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorLayoutCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorLayoutExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorLayoutExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ColorLayoutSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ColorLayoutSearch.h
# End Source File
# End Group
# Begin Group "ScalableColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\ScalableColor.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ScalableColor.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ScalableColorCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ScalableColorCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ScalableColorExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ScalableColorExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ScalableColorSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ScalableColorSearch.h
# End Source File
# End Group
# Begin Group "EdgeHistogram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\EdgeHist.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\EdgeHist.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\EdgeHistCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\EdgeHistCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\EdgeHistExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\EdgeHistExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\EdgeHistSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\EdgeHistSearch.h
# End Source File
# End Group
# Begin Group "ColorQuantizer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\ColorQuant.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ColorQuant.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorQuantCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ColorQuantCS.h
# End Source File
# End Group
# End Target
# End Project
