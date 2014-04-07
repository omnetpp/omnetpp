call ..\setenv-vc90.bat
path %PATH%;C:\Util
perl _convert.pl tkenv.lst icons.tcl icons
perl _convert.pl clearlooks.lst clearlooksimg.tcl I
copy icons.tcl ..\src\tkenv
copy clearlooksimg.tcl ..\src\tkenv