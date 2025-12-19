@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by XMMANAGER.HPJ. >"hlp\XMManager.hm"
echo. >>"hlp\XMManager.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\XMManager.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\XMManager.hm"
echo. >>"hlp\XMManager.hm"
echo // Prompts (IDP_*) >>"hlp\XMManager.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\XMManager.hm"
echo. >>"hlp\XMManager.hm"
echo // Resources (IDR_*) >>"hlp\XMManager.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\XMManager.hm"
echo. >>"hlp\XMManager.hm"
echo // Dialogs (IDD_*) >>"hlp\XMManager.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\XMManager.hm"
echo. >>"hlp\XMManager.hm"
echo // Frame Controls (IDW_*) >>"hlp\XMManager.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\XMManager.hm"
REM -- Make help for Project XMMANAGER


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\XMManager.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\XMManager.hlp" goto :Error
if not exist "hlp\XMManager.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\XMManager.hlp" Debug
if exist Debug\nul copy "hlp\XMManager.cnt" Debug
if exist Release\nul copy "hlp\XMManager.hlp" Release
if exist Release\nul copy "hlp\XMManager.cnt" Release
echo.
goto :done

:Error
echo hlp\XMManager.hpj(1) : error: Problem encountered creating help file

:done
echo.
