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
%opp_neddoc% -o %work%/cl1_dir_1 cl_files
%opp_neddoc% -o %work%/cl1_dir_2 %dir1%\cl_files
%opp_neddoc% -o %work%/cl1_dir_3 %dir2%/cl_files

:#
:# files, with relative and absolute paths, fwd and backslashes
:#
%opp_neddoc% -o %work%/cl1_files_1 cl_files\one.ned
%opp_neddoc% -o %work%/cl1_files_2 cl_files/one.ned
%opp_neddoc% -o %work%/cl1_files_3 cl_files\one.ned cl_files\two.ned
%opp_neddoc% -o %work%/cl1_files_4 cl_files/one.ned cl_files/two.ned
%opp_neddoc% -o %work%/cl1_files_5 %dir1%\cl_files\one.ned %dir1%\cl_files\two.ned
%opp_neddoc% -o %work%/cl1_files_6 %dir2%/cl_files/one.ned %dir2%/cl_files/two.ned

:#
:# wildcards. NOTE that wildcards don't work recursively (ie. you cannot say
:# "all .mymsg files from all subdirs")
:#
%opp_neddoc% -o %work%/cl1_wildcard_1 cl_files\*.msg
%opp_neddoc% -o %work%/cl1_wildcard_2 cl_files/*.msg

:#
:# nonexistent directory: error
:#
%opp_neddoc% -o %work%/cl1_nonexdir_1 c:\no\such\directory
%opp_neddoc% -o %work%/cl1_nonexdir_2 c:\no/such/directory

:#
:# nonexistent file: error
:#
%opp_neddoc% -o %work%/cl1_nonexfile_1 no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_2 cl_files\no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_3 cl_files/no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_4 %dir1%\cl_files\no-such-file.ned
%opp_neddoc% -o %work%/cl1_nonexfile_5 %dir2%/cl_files/no-such-file.ned

:#
:# pattern that doesn't match anything: warning
:#
%opp_neddoc% -o %work%/cl1_nonpatt_1 cl_files\*.bla
%opp_neddoc% -o %work%/cl1_nonpatt_2 cl_files/*.bla

:#
:# duplicate files
:#
%opp_neddoc% -o %work%/cl1_dupl_1 cl_files/one.ned cl_files/one.ned

:#
:# wrong file name case
:#
%opp_neddoc% -o %work%/cl1_case_1 FormatTing_1.Ned SyntaxHIGHlight_1.NED


