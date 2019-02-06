@echo off

:: drive + absolute dir
abspath c:\dir
abspath c:/dir
abspath \\unc\foo
abspath //unc/foo
echo.

:: no drive + relative dir
abspath .
abspath dir
abspath dir/dir2
abspath dir\dir2
echo.

:: drive + relative dir
abspath c:dir
abspath c:dir/dir2
abspath c:dir\dir2
echo.

:: nonexistent or invalid drives
abspath ?:dir
abspath t:dir
abspath t:dir/dir2
abspath t:dir\dir2
echo.

:: no drive + absolute dir
abspath \dir
abspath /dir
echo.

echo ---------

:: path (arg2) absolute, basedir (arg1) ignored
abspath c:\bla c:\dir
abspath c:/bla c:/dir
abspath \bla c:\dir
abspath /bla c:/dir
echo.

:: path has drive+relative dir, basedir has same drive+absolute dir
abspath c:\base c:dir
abspath c:/base c:dir
echo.

:: path has drive+relative dir, but basedir cannot be applied
abspath \base c:dir
abspath /base c:dir
abspath x:\base c:dir
abspath x:/base c:dir
abspath x:base c:dir
echo.

:: path has no drive but absolute dir
abspath base \dir
abspath base /dir
abspath \base \dir
abspath /base /dir
abspath c:\base \dir
abspath c:/base /dir
abspath c:base \dir
abspath c:base /dir
echo.

:: path has no drive and relative dir
abspath base dir
abspath \base dir
abspath /base dir
abspath c:\base dir
abspath c:/base dir
abspath c:base dir
echo.
