mkdir("elog/generated");

chdir("../stress");
system("stress -u Cmdenv -c Simple --sim-time-limit=1000s");
rename("result/Simple-0.elog", "../eventlog/elog/generated/stress.elog");
