# Microsoft Developer Studio Project File - Name="XMLibV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XMLibV - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "XMLibV.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "XMLibV.mak" CFG="XMLibV - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "XMLibV - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "XMLibV - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
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
# ADD CPP /nologo /W3 /GX /O2 /I "..\AddressLib" /I ".." /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /YX /FD /c /I "..\..\afsp\include" /D "AFLIB" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USETS" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEGCS" /D "USESG" /D "USESEM" /D "USESTM" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USESTC" /D "USEIS"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibV.lib"

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
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\AddressLib" /I ".." /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /YX /FD /c /I "..\..\afsp\include" /D "AFLIB" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USETS" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEGCS" /D "USESG" /D "USESEM" /D "USESTM" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USESTC" /D "USEIS"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibVDB.lib"

!ENDIF 

# Begin Target

# Name "XMLibV - Win32 Release"
# Name "XMLibV - Win32 Debug"
# Begin Group "Perceptual3DShape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\Perceptual3DShape.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\Perceptual3DShape.h
# End Source File
# Begin Source File

SOURCE=..\Descriptors\Perceptual3DShape_Aux.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\Perceptual3DShape_Aux.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\Perceptual3DShapeCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\Perceptual3DShapeCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\Perceptual3DShapeExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\Perceptual3DShapeExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\Perceptual3DShapeSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\Perceptual3DShapeSearch.h
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
# Begin Group "TimeSeries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\TimeSeries.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\TimeSeries.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\TimeSeriesCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\TimeSeriesCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TimeSeriesExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TimeSeriesExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\TimeSeriesSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\TimeSeriesSearch.h
# End Source File
# End Group
# Begin Group "3DShapeSpectrum"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\3DShapeSpectrum.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\3DShapeSpectrum.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\3DShapeSpectrumCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\3DShapeSpectrumCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\3DShapeSpectrumExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\3DShapeSpectrumExtraction.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\3d_shape_spectrum_extraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\3d_shape_spectrum_extraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\3DShapeSpectrumSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\3DShapeSpectrumSearch.h
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
# Begin Group "ColorStructure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorStructureExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ColorStructureExtraction.h
# End Source File
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

SOURCE=..\SearchUtilities\ColorStructureSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ColorStructureSearch.h
# End Source File
# End Group
# Begin Group "ContourShape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\ContourShape.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ContourShape.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ContourShapeCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ContourShapeCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ContourShapeExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ContourShapeExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ContourShapeSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ContourShapeSearch.h
# End Source File
# End Group
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
# Begin Group "FaceRecognition"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\FaceRecognition.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\FaceRecognition.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\FaceRecognitionCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\FaceRecognitionCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\FaceRecognitionExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\FaceRecognitionExtraction.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\face_detect.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\face_detect.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\FaceRecognitionSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\FaceRecognitionSearch.h
# End Source File
# End Group
# Begin Group "GoFGoPColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\GoFGoPColor.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\GoFGoPColor.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\GoFGoPColorCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\GoFGoPColorCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\GoFGoPColorExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\GoFGoPColorExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\GoFGoPColorSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\GoFGoPColorSearch.h
# End Source File
# End Group
# Begin Group "GridLayout"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\GridLayout.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\GridLayout.h
# End Source File
# End Group
# Begin Group "HomogeneousTexture"

# PROP Default_Filter ""
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

SOURCE=..\ExtractionUtilities\homo_texture_extractor.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\homo_texture_extractor.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\HomoTextureSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\HomoTextureSearch.h
# End Source File
# End Group
# Begin Group "MotionActivity"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\MotionActivity.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\MotionActivity.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MotionActivityCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MotionActivityCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MotionActivityExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MotionActivityExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MotionActivitySearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MotionActivitySearch.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\motion_activity_categorization.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\motion_activity_extraction.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\motion_activity_categorization.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\motion_activity_extraction.cpp
# End Source File
# End Group
# Begin Group "MotionTrajectory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\MotionTrajectory.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\MotionTrajectory.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MotionTrajectoryCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MotionTrajectoryCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MotionTrajectoryExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MotionTrajectoryExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MotionTrajectorySearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MotionTrajectorySearch.h
# End Source File
# End Group
# Begin Group "MultiImage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\MultiImage.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\MultiImage.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MultiImageCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MultiImageCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MultiImageExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MultiImageExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MultiImageSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MultiImageSearch.h
# End Source File
# End Group
# Begin Group "MultiView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\MultiView.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\MultiView.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MultiViewCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\MultiViewCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MultiViewExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\MultiViewExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MultiViewSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MultiViewSearch.h
# End Source File
# End Group
# Begin Group "ParametricObjectMotion"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\coortrans.c

# SUBTRACT CPP /D "c_plusplus"

# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\coortrans.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\gme_for_iso.c

# SUBTRACT CPP /D "c_plusplus"

# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\gme_for_iso.h
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ParametricObjectMotion.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\ParametricObjectMotion.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ParametricObjectMotionCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\ParametricObjectMotionCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ParametricObjectMotionExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ParametricObjectMotionExtraction.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ParametricObjectMotionExtraction2.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\ParametricObjectMotionExtraction2.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ParametricObjectMotionSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ParametricObjectMotionSearch.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ParametricObjectMotionUtilities.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\ParametricObjectMotionUtilities.h
# End Source File
# End Group
# Begin Group "RegionLocator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\RegionLocator.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\RegionLocator.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\RegionLocatorCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\RegionLocatorCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\RegionLocatorExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\RegionLocatorExtraction.h
# End Source File
# End Group
# Begin Group "RegionShape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\RegionShape.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\RegionShape.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\RegionShapeCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\RegionShapeCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\RegionShapeExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\RegionShapeExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\RegionShapeSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\RegionShapeSearch.h
# End Source File
# End Group
# Begin Group "Spatial2DCoordinates"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\Spatial2DCoordinates.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\Spatial2DCoordinates.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\Spatial2DCoordinatesCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\Spatial2DCoordinatesCS.h
# End Source File
# End Group
# Begin Group "SpatioTemporalLocator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\spatio_temporal_locator_extraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\spatio_temporal_locator_extraction.h
# End Source File
# Begin Source File

SOURCE=..\Descriptors\SpatioTemporalLocator.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\SpatioTemporalLocator.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\SpatioTemporalLocatorCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\SpatioTemporalLocatorCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SpatioTemporalLocatorExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\SpatioTemporalLocatorExtraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SpatioTemporalLocatorSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\SpatioTemporalLocatorSearch.h
# End Source File
# End Group
# Begin Group "TemporalInterpolation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ExtractionUtilities\temporal_interpolation_extraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\temporal_interpolation_extraction.h
# End Source File
# Begin Source File

SOURCE=..\Descriptors\TemporalInterpolation.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\TemporalInterpolation.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\TemporalInterpolationCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\TemporalInterpolationCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TemporalInterpolationExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TemporalInterpolationExtraction.h
# End Source File
# End Group
# Begin Group "TextureBrowsing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\TextureBrowsing.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\TextureBrowsing.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\TextureBrowsingCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\TextureBrowsingCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TextureBrowsingExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\TextureBrowsingExtraction.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\texture_browsing_extraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\texture_browsing_extraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\TextureBrowsingSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\TextureBrowsingSearch.h
# End Source File
# End Group
# Begin Group "CameraMotion"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Descriptors\CameraMotion.cpp
# End Source File
# Begin Source File

SOURCE=..\Descriptors\CameraMotion.h
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\CameraMotionCS.cpp
# End Source File
# Begin Source File

SOURCE=..\CodingSchemes\CameraMotionCS.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\CameraMotionExtraction.cpp
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\CameraMotionExtraction.h
# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\camera_motion_extraction.c

# SUBTRACT CPP /D "c_plusplus"

# End Source File
# Begin Source File

SOURCE=..\ExtractionUtilities\camera_motion_extraction.h
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\CameraMotionSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\CameraMotionSearch.h
# End Source File
# End Group
# End Target
# End Project
