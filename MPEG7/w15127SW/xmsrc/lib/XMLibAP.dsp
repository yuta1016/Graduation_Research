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
# ADD CPP /nologo /W3 /GX /O2 /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /I "C:\Program Files\OpenCV\cv\include" /I "C:\Program Files\OpenCV\cxcore\include" /D "NDEBUG" /D "USED_CT" /D "USECTB" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "AFLIB" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USEIS" /D "USEVS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibAP.lib"

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
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\AddressLib" /I ".." /I "..\..\afsp\include" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /I "C:\Program Files\OpenCV\cv\include" /I "C:\Program Files\OpenCV\cxcore\include" /I "..\lib\cv_P3DS" /D "_DEBUG" /D "DEBUG" /D "USED_CT" /D "USECTB" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /D "AFLIB" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USEIS" /D "USEVS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibAPDB.lib"

!ENDIF 

# Begin Target

# Name "XMLibAP - Win32 Release"
# Name "XMLibAP - Win32 Debug"
# Begin Group "BasicModules"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ClientApplications.h
# End Source File
# Begin Source File

SOURCE=..\XMManager\ComponentDB.cpp
# End Source File
# Begin Source File

SOURCE=..\XMManager\ComponentDB.h
# End Source File
# Begin Source File

SOURCE=..\Applications\RegisteredClientApplications.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\RegisteredServerApplications.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ServerApplications.h
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version1.cpp
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version1.h
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version2.cpp
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version2.h
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version3.cpp
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version3.h
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version4.cpp
# End Source File
# Begin Source File

SOURCE=..\XMManager\Version4.h
# End Source File
# End Group
# Begin Group "AudioSignature"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\AudioSignatureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\AudioSignatureCAppl.h
# End Source File
# End Group
# Begin Group "AudioSpectralFlatness"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\AudioSpectralFlatnessCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\AudioSpectralFlatnessCAppl.h
# End Source File
# End Group
# Begin Group "MelodySequence"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MelodySequenceCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MelodySequenceCAppl.h
# End Source File
# End Group
# Begin Group "Perceptual3DShape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\Perceptual3DShapeCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\Perceptual3DShapeCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\Perceptual3DShapeSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\Perceptual3DShapeSAppl.h
# End Source File
# End Group
# Begin Group "ScalableColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ScalableColorCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ScalableColorCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ScalableColorSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ScalableColorSAppl.h
# End Source File
# End Group
# Begin Group "3DShapeSpectrum"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\3DShapeSpectrumCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\3DShapeSpectrumCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\3DShapeSpectrumSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\3DShapeSpectrumSAppl.h
# End Source File
# End Group
# Begin Group "ColorLayout"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ColorLayoutCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorLayoutCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorLayoutSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorLayoutSAppl.h
# End Source File
# End Group
# Begin Group "ColorStructure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ColorStructureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorStructureCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorStructureSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ColorStructureSAppl.h
# End Source File
# End Group
# Begin Group "ContourShape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ContourShapeCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ContourShapeCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ContourShapeSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ContourShapeSAppl.h
# End Source File
# End Group
# Begin Group "DominantColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\DominantColorCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\DominantColorCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\DominantColorSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\DominantColorSAppl.h
# End Source File
# End Group
# Begin Group "EdgeHistogram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\EdgeHistCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\EdgeHistCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\EdgeHistSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\EdgeHistSAppl.h
# End Source File
# End Group
# Begin Group "FaceRecognition"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\FaceRecognitionCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\FaceRecognitionCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\FaceRecognitionSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\FaceRecognitionSAppl.h
# End Source File
# End Group
# Begin Group "GoFGoPColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\GoFGoPColorCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\GoFGoPColorCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\GoFGoPColorSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\GoFGoPColorSAppl.h
# End Source File
# End Group
# Begin Group "HomogeneousTexture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\HomoTextureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\HomoTextureCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\HomoTextureSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\HomoTextureSAppl.h
# End Source File
# End Group
# Begin Group "MotionActivity"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MotionActivityCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MotionActivityCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\MotionActivitySAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MotionActivitySAppl.h
# End Source File
# End Group
# Begin Group "MotionTrajectory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MotionTrajectoryCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MotionTrajectoryCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\MotionTrajectorySAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MotionTrajectorySAppl.h
# End Source File
# End Group
# Begin Group "MultiImage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MultiImageCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MultiImageCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\MultiImageSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MultiImageSAppl.h
# End Source File
# End Group
# Begin Group "MultiView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MultiViewCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MultiViewCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\MultiViewSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MultiViewSAppl.h
# End Source File
# End Group
# Begin Group "ParametricObjectMotion"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ParametricObjectMotionCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ParametricObjectMotionCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ParametricObjectMotionSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ParametricObjectMotionSAppl.h
# End Source File
# End Group
# Begin Group "RegionLocator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\RegionLocatorCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\RegionLocatorCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\RegionLocatorSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\RegionLocatorSAppl.h
# End Source File
# End Group
# Begin Group "RegionShape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\RegionShapeCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\RegionShapeCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\RegionShapeSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\RegionShapeSAppl.h
# End Source File
# End Group
# Begin Group "SpatioTemporalLocator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SpatioTemporalLocatorCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SpatioTemporalLocatorCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\SpatioTemporalLocatorSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SpatioTemporalLocatorSAppl.h
# End Source File
# End Group
# Begin Group "TextureBrowsing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\TextureBrowsingCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\TextureBrowsingCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\TextureBrowsingSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\TextureBrowsingSAppl.h
# End Source File
# End Group
# Begin Group "AffectiveSegmentFilter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\AffectiveSegmentFilterCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\AffectiveSegmentFilterCAppl.h
# End Source File
# End Group
# Begin Group "DependStructure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\DependStructureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\DependStructureCAppl.h
# End Source File
# End Group
# Begin Group "ExtendedTextualType"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ExtendedTextualTypeCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ExtendedTextualTypeCAppl.h
# End Source File
# End Group
# Begin Group "HierarchicalKeyFrame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\HierarchicalKeyFrameSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\HierarchicalKeyFrameSAppl.h
# End Source File
# End Group
# Begin Group "ImportanceHint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ImportanceHintCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ImportanceHintCAppl.h
# End Source File
# End Group
# Begin Group "InkSegment"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\InkSegmentCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\InkSegmentCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\InkSegmentSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\InkSegmentSAppl.h
# End Source File
# End Group
# Begin Group "MatchingHint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MatchingHintCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MatchingHintCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\MatchingHintSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MatchingHintSAppl.h
# End Source File
# End Group
# Begin Group "MediaMeta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MediaMetaCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MediaMetaCAppl.h
# End Source File
# End Group
# Begin Group "MediaQuality"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MediaQualitySAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MediaQualitySAppl.h
# End Source File
# End Group
# Begin Group "MediaReview"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MediaReviewCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MediaReviewCAppl.h
# End Source File
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ModelSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ModelSAppl.h
# End Source File
# End Group
# Begin Group "Mosaic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\MosaicCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MosaicCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\MosaicSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\MosaicSAppl.h
# End Source File
# End Group
# Begin Group "OrderingKey"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\OrderingKeyCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\OrderingKeyCAppl.h
# End Source File
# End Group
# Begin Group "Package"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\PackageCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\PackageCAppl.h
# End Source File
# End Group
# Begin Group "PhoneticTranslationHints"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\PhoneticTranslationHintsCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\PhoneticTranslationHintsCAppl.h
# End Source File
# End Group
# Begin Group "PointOfView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\PointOfViewCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\PointOfViewCAppl.h
# End Source File
# End Group
# Begin Group "SegmentRelGraph"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SegmentRelGraphCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SegmentRelGraphCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\SegmentRelGraphSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SegmentRelGraphSAppl.h
# End Source File
# End Group
# Begin Group "SeqSummaryFastMovingStoryBoard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SeqSummaryFastMovingStoryBoardSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SeqSummaryFastMovingStoryBoardSAppl.h
# End Source File
# End Group
# Begin Group "SeqSummaryKeywords"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SeqSummaryKeywordsSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SeqSummaryKeywordsSAppl.h
# End Source File
# End Group
# Begin Group "SequentialSummary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SequentialSummarySAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SequentialSummarySAppl.h
# End Source File
# End Group
# Begin Group "SoundProperty"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SoundPropertySAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SoundPropertySAppl.h
# End Source File
# End Group
# Begin Group "SpaceFrequencyView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SpaceFrequencyViewSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SpaceFrequencyViewSAppl.h
# End Source File
# End Group
# Begin Group "SpatialResolutionHint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SpatialResolutionHintCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SpatialResolutionHintCAppl.h
# End Source File
# End Group
# Begin Group "StateTransitionModel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\StateTransitionModelSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\StateTransitionModelSAppl.h
# End Source File
# End Group
# Begin Group "Summarization"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SummarizationCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SummarizationCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\SummarizationSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SummarizationSAppl.h
# End Source File
# End Group
# Begin Group "SummaryPreferences"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\SummaryPreferencesCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\SummaryPreferencesCAppl.h
# End Source File
# End Group
# Begin Group "Time"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\TimeCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\TimeCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\TimeSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\TimeSAppl.h
# End Source File
# End Group
# Begin Group "MediaTranscodingHints"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\TranscodingSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\TranscodingSAppl.h
# End Source File
# End Group
# Begin Group "UserPreference"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\UserPreferenceCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\UserPreferenceCAppl.h
# End Source File
# End Group
# Begin Group "Variation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\VariationSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\VariationSAppl.h
# End Source File
# End Group
# Begin Group "VideoEditing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\VideoEditingSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\VideoEditingSAppl.h
# End Source File
# End Group
# Begin Group "VideoText"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\VideoTextSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\VideoTextSAppl.h
# End Source File
# End Group
# Begin Group "DominantColorTemperature"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\DominantColorTemperatureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\DominantColorTemperatureCAppl.h
# End Source File
# End Group
# Begin Group "ColorTemperature"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\CTBrowsingCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\CTBrowsingCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\CTBrowsingSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\CTBrowsingSAppl.h
# End Source File
# End Group
# Begin Group "ImageSignature"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\ImageSignatureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ImageSignatureCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\ImageSignatureSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\ImageSignatureSAppl.h
# End Source File
# End Group
# Begin Group "VideoSignature"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Applications\VideoSignatureCAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\VideoSignatureCAppl.h
# End Source File
# Begin Source File

SOURCE=..\Applications\VideoSignatureSAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Applications\VideoSignatureSAppl.h
# End Source File
# End Group
# End Target
# End Project
