set _INSTALL_DIR="%ProgramFiles%\brilliant_wp"

xcopy /s /i %~p0 %_INSTALL_DIR%

echo "WallpaperChanger has been installed to %_INSTALL_DIR%"
pause