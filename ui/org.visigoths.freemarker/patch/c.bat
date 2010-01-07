dir /s/b freemarker\*.java >$$$.tmp
javac -target 1.5 -source 1.5 -classpath ..\freemarker.jar @$$$.tmp -d classes
del $$$.tmp