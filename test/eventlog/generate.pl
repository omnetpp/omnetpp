mkdir("generated");

chdir("../stress");
system("stress.exe -u Cmdenv");
rename("omnetpp.log", "../eventlog/generated/stress.log");
