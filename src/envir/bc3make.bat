@echo off
rem platform: MS-DOS/Borland C++ 3.1
@echo *******************************************************
@echo ****	 Building ENVIR library...		 ****
@echo *******************************************************

@rem Creating makefile from project file
if not exist envir.mak prj2mak envir.prj

@rem Make object files
make -fenvir.mak

@rem Build new lib file
del envir.lib
tlib /0 envir.lib @makelib.lst
