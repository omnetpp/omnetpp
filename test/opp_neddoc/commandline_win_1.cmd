:#
:# Tests if files and directories are accepted OK as arguments
:#

call setenv.cmd
del /s /q %work% >nul
mkdir work

:#
:# -a switch
:#
%opp_neddoc% -o %work%/cl1_all_1 -a

:#
:# directory, with relative and absolute paths, fwd and backslashes
:#
%opp_neddoc% -o %work%/cl1_dir_1 .
%opp_neddoc% -o %work%/cl1_dir_2 %dir1%
%opp_neddoc% -o %work%/cl1_dir_3 %dir2%

:#
:# files, with relative and absolute paths, fwd and backslashes
:#
%opp_neddoc% -o %work%/cl1_files_1 formatting_1.ned
%opp_neddoc% -o %work%/cl1_files_2 formatting_1.ned syntaxhighlight_1.ned
%opp_neddoc% -o %work%/cl1_files_3 ./formatting_1.ned ./syntaxhighlight_1.ned
%opp_neddoc% -o %work%/cl1_files_4 .\formatting_1.ned .\syntaxhighlight_1.ned
%opp_neddoc% -o %work%/cl1_files_5 %dir1%\formatting_1.ned %dir1%\syntaxhighlight_1.ned
%opp_neddoc% -o %work%/cl1_files_6 %dir2%/formatting_1.ned %dir2%/syntaxhighlight_1.ned

:#
:# wildcards. NOTE: directory+wildcard (e.g. dir/*.ned) is not supported!!
:#
%opp_neddoc% -o %work%/cl1_wildcard_1 *.ned
%opp_neddoc% -o %work%/cl1_wildcard_2 *.msg

:#
:# nonexistent directory
:#
%opp_neddoc% -o %work%/cl1_nonexdir_1 c:\no\such\directory
%opp_neddoc% -o %work%/cl1_nonexdir_2 c:\no/such/directory

:#
:# nonexistent file
:#
%opp_neddoc% -o %work%/cl1_nonexfile_1 no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_2 .\no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_3 ./no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_2 %dir1%\no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_3 %dir2%/no-such-file.ned

:#
:# duplicate files
:#
%opp_neddoc% -o %work%/cl1_dupl_1 formatting_1.ned formatting_1.ned formatting_1.ned

:#
:# upper-case file names
:#
%opp_neddoc% -o %work%/cl1_case_1 FormatTing_1.Ned SyntaxHIGHlight_1.NED


