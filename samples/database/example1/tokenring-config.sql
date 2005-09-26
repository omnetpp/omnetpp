#
# Example configuration for the TokenRing demo simulation.
# Use sql/config.sql to create the tables before running this.
#

DELETE e, s, c FROM configentry e, configsection s, config c
  WHERE e.sectionid=s.id AND s.configid=c.id AND c.name="sampleconfig";

INSERT INTO config (id,name) VALUES (10,"sampleconfig");
INSERT INTO configsection (configid,id,name) VALUES (10,1000,"General");
INSERT INTO configsection (configid,id,name) VALUES (10,1003,"Parameters");
INSERT INTO configsection (configid,id,name) VALUES (10,1004,"Run 1");
INSERT INTO configsection (configid,id,name) VALUES (10,1005,"Run 2");
INSERT INTO configsection (configid,id,name) VALUES (10,1006,"Run 3");
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,"preload-ned-files","*.ned",1);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,"network","token",2);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,"sim-time-limit","1800s",3);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,"preload-ned-files","*.ned",4);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,"network","token",5);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1003,"token.comp[*].gen.numMessages","1000000",21);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1003,"token.comp[*].gen.messageLength","uniform(1,4500)",22);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1003,"token.comp[*].mac.queueMaxLen","20",23);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1004,"description","well loaded ring (if run w/ numStations=5)",24);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1004,"token.THT","0.2s",25);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1004,"token.comp[*].gen.interArrivalTime","truncnormal(0.023, 0.006)",26);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1004,"output-vector-file","token1.vec",27);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1005,"description","overloaded ring (if run w/ numStations=5)",28);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1005,"token.numStations","7",29);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1005,"token.THT","0.2s",30);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1005,"token.comp[*].gen.interArrivalTime","truncnormal(0.015, 0.003)",31);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1005,"output-vector-file","token2.vec",32);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1006,"description","ring of 100 stations",33);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1006,"token.numStations","100",34);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1006,"token.THT","0.2s",35);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1006,"token.comp[*].gen.interArrivalTime","truncnormal(0.40, 0.2)",36);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1006,"output-vector-file","token3.vec",37);


