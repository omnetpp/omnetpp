@echo off
dir /s /b *.cc *.h *.cpp *.hpp >cppfiles.lst
ruby %~dp0\migratecpp.rb cppfiles.lst
