@echo off

:: drive + absolute dir
abspath c:\dir
abspath c:/dir
abspath \\unc\foo
abspath //unc/foo

:: no drive + relative dir
abspath .
abspath dir
abspath dir/dir2
abspath dir\dir2

:: drive + relative dir
abspath c:dir
abspath c:dir/dir2
abspath c:dir\dir2

:: nonexistent or invalid drives
abspath ?:dir
abspath t:dir
abspath t:dir/dir2
abspath t:dir\dir2

:: no drive + absolute dir
abspath \dir
abspath /dir

echo ---------

:: path (arg2) absolute, basedir (arg1) ignored
abspath c:\bla c:\dir
abspath c:/bla c:/dir
abspath \bla c:\dir
abspath /bla c:/dir
