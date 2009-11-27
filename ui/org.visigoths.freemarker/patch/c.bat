dir /s/b freemarker\*.java >$$$.tmp
javac -classpath ..\freemarker.jar @$$$.tmp -d classes
del $$$.tmp