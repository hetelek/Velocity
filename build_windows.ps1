Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$CLEAN_BUILD = 1
$QT_BIN_PATH = "C:\Qt\6.8.0\mingw_64\bin"

$SCRIPT_DIR = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition
Set-Location -Path $SCRIPT_DIR

if ($CLEAN_BUILD -eq 1) {
    Remove-Item -Recurse -Force vel_build, "Velocity\build", "XboxInternals\build"
}

New-Item -ItemType Directory -Force -Path vel_build
Set-Location -Path vel_build

New-Item -ItemType Directory -Force -Path internals
Set-Location -Path internals
& "$QT_BIN_PATH\qmake6.exe" -config release ..\..\XboxInternals\XboxInternals.pro
& mingw32-make.exe -j10
Set-Location ..

New-Item -ItemType Directory -Force -Path vel
Set-Location -Path vel
& "$QT_BIN_PATH\qmake6.exe" -config release ..\..\Velocity\Velocity.pro
& mingw32-make.exe -j10
Set-Location ..

Set-Location -Path $SCRIPT_DIR
& "$QT_BIN_PATH\windeployqt6.exe" .\Velocity\build\Velocity-Win\release\Velocity.exe
Copy-Item -Path "XboxInternals\build\XboxInternals-Win\release\XboxInternals.dll" -Destination ".\Velocity\build\Velocity-Win\release\"

explorer.exe ".\Velocity\build\Velocity-Win\release"
