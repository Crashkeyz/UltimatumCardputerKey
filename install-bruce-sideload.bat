@echo off
REM Installation script for Bruce Firmware sideload (Windows)
REM Usage: install-bruce-sideload.bat DRIVE_LETTER:

echo ======================================================================
echo   Ultimatum Cardputer - Bruce Firmware Sideload Installer
echo ======================================================================
echo.

if "%~1"=="" (
    echo [X] Error: No SD card drive letter provided
    echo.
    echo Usage: install-bruce-sideload.bat DRIVE_LETTER:
    echo.
    echo Example: install-bruce-sideload.bat E:
    echo          ^(where E: is your SD card drive letter^)
    echo.
    echo To find your SD card drive letter:
    echo   1. Open 'This PC' or 'My Computer'
    echo   2. Look for your SD card ^(usually shows as Removable Disk^)
    echo   3. Note the drive letter ^(e.g., D:, E:, F:^)
    echo.
    pause
    exit /b 1
)

set SD_PATH=%~1
set FIRMWARE_DIR=%SD_PATH%\firmware
set TARGET_DIR=%FIRMWARE_DIR%\ultimatum

REM Check if SD card path exists
if not exist "%SD_PATH%\" (
    echo [X] Error: SD card drive not found: %SD_PATH%
    echo.
    echo Please check that:
    echo   1. SD card is inserted into your computer
    echo   2. Drive letter is correct ^(check in 'This PC'^)
    echo   3. You included the colon ^(e.g., E: not E^)
    echo.
    pause
    exit /b 1
)

echo [OK] SD card found at: %SD_PATH%
echo.

REM Check if firmware files exist locally
if not exist ".pio\build\m5stack-cardputer\firmware.bin" (
    echo [X] Error: Firmware binary not found!
    echo.
    echo Please build the firmware first:
    echo   pio run -e m5stack-cardputer
    echo.
    echo Or download a pre-built release from:
    echo   https://github.com/Crashkeyz/UltimatumCardputerKey/releases
    echo.
    pause
    exit /b 1
)

if not exist "bruce_manifest.json" (
    echo [X] Error: bruce_manifest.json not found!
    echo Please ensure you're running this script from the project root directory.
    pause
    exit /b 1
)

if not exist "default_16MB.csv" (
    echo [X] Error: default_16MB.csv not found!
    echo Please ensure you're running this script from the project root directory.
    pause
    exit /b 1
)

REM Create firmware directory structure
echo Creating directory structure...
if not exist "%FIRMWARE_DIR%" mkdir "%FIRMWARE_DIR%"
if %ERRORLEVEL% NEQ 0 (
    echo [X] Error: Could not create firmware directory
    pause
    exit /b 1
)

if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"
if %ERRORLEVEL% NEQ 0 (
    echo [X] Error: Could not create directory: %TARGET_DIR%
    pause
    exit /b 1
)

echo [OK] Directory created: %TARGET_DIR%
echo.

REM Copy firmware files
echo Copying firmware files...

copy /Y ".pio\build\m5stack-cardputer\firmware.bin" "%TARGET_DIR%\" >nul
if %ERRORLEVEL% NEQ 0 (
    echo [X] Error: Failed to copy firmware.bin
    pause
    exit /b 1
)
echo [OK] Copied firmware.bin

copy /Y "bruce_manifest.json" "%TARGET_DIR%\manifest.json" >nul
if %ERRORLEVEL% NEQ 0 (
    echo [X] Error: Failed to copy manifest.json
    pause
    exit /b 1
)
echo [OK] Copied manifest.json

copy /Y "default_16MB.csv" "%TARGET_DIR%\" >nul
if %ERRORLEVEL% NEQ 0 (
    echo [X] Error: Failed to copy default_16MB.csv
    pause
    exit /b 1
)
echo [OK] Copied default_16MB.csv

REM Create README in target directory
(
echo Ultimatum Cardputer Firmware - Bruce Firmware Sideload Package
echo ================================================================
echo.
echo This firmware can be loaded through Bruce Firmware's sideload menu.
echo.
echo To Use:
echo 1. Ensure this folder is at: SD:/firmware/ultimatum/
echo 2. Boot your M5Stack Cardputer with Bruce Firmware
echo 3. Navigate to the sideload or apps menu
echo 4. Select "Ultimatum Cardputer"
echo 5. The firmware will load
echo.
echo To Return to Bruce Firmware:
echo - Simply restart your device
echo.
echo Files:
echo - firmware.bin: Main firmware binary
echo - manifest.json: Metadata for Bruce Firmware loader
echo - default_16MB.csv: Partition table for proper memory management
echo - README.txt: This file
echo.
echo For more information:
echo https://github.com/Crashkeyz/UltimatumCardputerKey/blob/main/docs/BRUCE_FIRMWARE_SIDELOAD.md
echo.
echo Version: 1.0.0
) > "%TARGET_DIR%\README.txt"

if %ERRORLEVEL% NEQ 0 (
    echo [X] Warning: Failed to create README.txt
)

echo.
echo ======================================================================
echo   Installation Complete!
echo ======================================================================
echo.
echo Files installed to: %TARGET_DIR%
echo.
echo Directory contents:
dir /B "%TARGET_DIR%"
echo.
echo Next Steps:
echo   1. Safely eject your SD card from the computer
echo      ^(Right-click on the drive in 'This PC' and select 'Eject'^)
echo   2. Insert the SD card into your M5Stack Cardputer
echo   3. Power on or restart your device ^(should be running Bruce Firmware^)
echo   4. Navigate to Bruce Firmware's sideload/apps menu
echo   5. Select 'Ultimatum Cardputer' from the list
echo   6. The firmware will load and run
echo.
echo To return to Bruce Firmware, simply restart your device.
echo.
echo For detailed instructions and troubleshooting:
echo   https://github.com/Crashkeyz/UltimatumCardputerKey/blob/main/docs/BRUCE_FIRMWARE_SIDELOAD.md
echo.
pause
exit /b 0
