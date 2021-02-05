mkdir("elog/generated");

chdir("../../models/stress");
system("./stress -u Cmdenv -c Simple --sim-time-limit=1000s");
rename("results/Simple-#0.elog", "../../misc/eventlog/elog/generated/stress.elog");
