# Microsoft Developer Studio Project File - Name="XMLibA" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XMLibA - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "XMLibA.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "XMLibA.mak" CFG="XMLibA - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "XMLibA - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "XMLibA - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XMLibA - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\AddressLib" /I ".." /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /YX /FD /c /I "..\..\afsp\include" /D "AFLIB" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USEIS"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibA.lib"

!ELSEIF  "$(CFG)" == "XMLibA - Win32 Debug"

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
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\AddressLib" /I ".." /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "_VISUALC_" /D "c_plusplus" /D "NODISP" /YX /FD /c /I "..\..\afsp\include" /D "AFLIB" /I "..\..\ImageMagick" /I "..\..\xml\include" /I "..\..\xml\src" /D "USEXML" /D "USEAST" /D "USESFM" /D "USEMSQ" /D "USEP3DS" /D "USESCC" /D "USE3DSS" /D "USECL" /D "USECT" /D "USESC" /D "USEDC" /D "USEEH" /D "USEFR" /D "USEGF" /D "USEHT" /D "USEMA" /D "USEMT" /D "USEMI" /D "USE2D3D" /D "USEPOM" /D "USERL" /D "USESR" /D "USES2D" /D "USESTL" /D "USETI" /D "USETB" /D "USEASF" /D "USEDPS" /D "USEETT" /D "USEHS" /D "USEIMH" /D "USEINK" /D "USEMH" /D "USEMM" /D "USEMQ" /D "USEMR" /D "USEMO" /D "USEMK" /D "USEODK" /D "USEPACK" /D "USEPTH" /D "USEPOV" /D "USESRG" /D "USESMS" /D "USESKW" /D "USESSZ" /D "USESP" /D "USESF" /D "USESRH" /D "USESTX" /D "USESZ" /D "USESPR" /D "USETM" /D "USETH" /D "USEUP" /D "USEVA" /D "USEVE" /D "USEVT" /D "USEIS"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"XMLibADB.lib"

!ENDIF 

# Begin Target

# Name "XMLibA - Win32 Release"
# Name "XMLibA - Win32 Debug"
# Begin Group "AudioSignature"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\AudioSignatureSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\AudioSignatureSearch.h
# End Source File
# End Group
# Begin Group "AudioSpectralFlatness"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\AudioSpectralFlatnessSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\AudioSpectralFlatnessSearch.h
# End Source File
# End Group
# Begin Group "MelodySequence"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SearchUtilities\MelodySequenceSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\SearchUtilities\MelodySequenceSearch.h
# End Source File
# End Group
# End Target
# End Project
