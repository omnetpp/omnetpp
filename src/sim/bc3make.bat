@echo off
rem platform: MS-DOS/Borland C++ 3.1
@echo *******************************************************
@echo ****	 Building SIM library...		 ****
@echo *******************************************************

@rem Creating makefile from project file
if not exist sim_std.mak prj2mak sim_std.prj

@rem Make object files
make -fsim_std.mak

@rem Build new lib file
del sim_std.lib
tlib /0 sim_std.lib @makelib.lst
