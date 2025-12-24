# Microsoft Developer Studio Project File - Name="XMLibM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XMLibM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XMLibM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XMLibM.mak" CFG="XMLibM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XMLibM - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XMLibM - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XMLibM - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /I "C:\Program Files\OpenCV\cv\include" /I "C:\Program Files\OpenCV\cxcore\include" /I "..\lib\cv_P3DS" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "AFLIB" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USEIS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibM.lib"

!ELSEIF  "$(CFG)" == "XMLibM - Win32 Debug"

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
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /I "C:\Program Files\OpenCV\cv\include" /I "C:\Program Files\OpenCV\cxcore\include" /I "..\lib\cv_P3DS" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "AFLIB" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USEIS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibMDB.lib"

!ENDIF 

# Begin Target

# Name "XMLibM - Win32 Release"
# Name "XMLibM - Win32 Debug"
# Begin Group "AffectiveSegmentFilter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\AffectiveSegmentFilterSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\AffectiveSegmentFilterSearch.h
# End Source File
# End Group
# Begin Group "DependStructure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\DependStructureSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\DependStructureSearch.h
# End Source File
# End Group
# Begin Group "ExtendedTextualType"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\ExtendedTextualTypeSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ExtendedTextualTypeSearch.h
# End Source File
# End Group
# Begin Group "HierarchicalKeyFrame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\HierarchicalKeyFrameExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\HierarchicalKeyFrameExtraction.h
# End Source File
# End Group
# Begin Group "ImportanceHint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\ImportanceHintSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ImportanceHintSearch.h
# End Source File
# End Group
# Begin Group "InkSegment"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\InkSegmentExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\InkSegmentExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\InkSegmentSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\InkSegmentSearch.h
# End Source File
# End Group
# Begin Group "MatchingHint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\MatchingHintExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MatchingHintExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MatchingHintSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MatchingHintSearch.h
# End Source File
# End Group
# Begin Group "MediaMeta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\MediaMetaSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MediaMetaSearch.h
# End Source File
# End Group
# Begin Group "MediaQuality"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\MediaQualityExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MediaQualityExtraction.h
# End Source File
# End Group
# Begin Group "MediaReview"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\MediaReviewSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MediaReviewSearch.h
# End Source File
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\ModelExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ModelExtraction.h
# End Source File
# End Group
# Begin Group "Mosaic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\MosaicExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MosaicExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MosaicSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MosaicSearch.h
# End Source File
# End Group
# Begin Group "OrderingKey"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\OrderingKeySearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\OrderingKeySearch.h
# End Source File
# End Group
# Begin Group "Package"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\PackageSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\PackageSearch.h
# End Source File
# End Group
# Begin Group "PhoneticTranslationHints"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\PhoneticTranslationHintsSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\PhoneticTranslationHintsSearch.h
# End Source File
# End Group
# Begin Group "PointOfView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\PointOfViewSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\PointOfViewSearch.h
# End Source File
# End Group
# Begin Group "SegmentRelGraph"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\SegmentRelGraphExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SegmentRelGraphExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SegmentRelGraphSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SegmentRelGraphSearch.h
# End Source File
# End Group
# Begin Group "SeqSummaryFastMovingStoryBoard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\SeqSummaryFastMovingStoryBoardExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SeqSummaryFastMovingStoryBoardExtraction.h
# End Source File
# End Group
# Begin Group "SeqSummaryKeywords"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\SeqSummaryKeywordsExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SeqSummaryKeywordsExtraction.h
# End Source File
# End Group
# Begin Group "SequentialSummary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\SequentialSummaryExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SequentialSummaryExtraction.h
# End Source File
# End Group
# Begin Group "SoundProperty"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\SoundPropertyExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SoundPropertyExtraction.h
# End Source File
# End Group
# Begin Group "SpaceFrequencyView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\SpaceFrequencyViewExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SpaceFrequencyViewExtraction.h
# End Source File
# End Group
# Begin Group "SpatialResolutionHint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\SpatialResolutionHintSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SpatialResolutionHintSearch.h
# End Source File
# End Group
# Begin Group "StateTransitionModel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\StateTransitionModelExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\StateTransitionModelExtraction.h
# End Source File
# End Group
# Begin Group "Summarization"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\SummarizationExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SummarizationExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SummarizationSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SummarizationSearch.h
# End Source File
# End Group
# Begin Group "SummaryPreferences"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\SummaryPreferencesSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SummaryPreferencesSearch.h
# End Source File
# End Group
# Begin Group "Time"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\TimeExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TimeExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\TimeSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\TimeSearch.h
# End Source File
# End Group
# Begin Group "MediaTranscodingHints"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\TranscodingHintsExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TranscodingHintsExtraction.h
# End Source File
# End Group
# Begin Group "UserPreference"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\UserPreferenceSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\UserPreferenceSearch.h
# End Source File
# End Group
# Begin Group "Variation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\VariationExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VariationExtraction.h
# End Source File
# End Group
# Begin Group "VideoEditing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoEditingExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoEditingExtraction.h
# End Source File
# End Group
# Begin Group "VideoText"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Area.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Area.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Coordinate.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Coordinate.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Detect.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Detect.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_FrameFile.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_FrameFile.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Line.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Line.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Parameters.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoText_Parameters.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoTextExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\VideoTextExtraction.h
# End Source File
# End Group
# End Target
# End Project
