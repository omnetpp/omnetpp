#
# Example configuration for the Fifo demo simulation.
# Use sql/config.sql to create the tables before running this.
#

DELETE e, s, c FROM configentry e, configsection s, config c
    WHERE e.sectionid=s.id AND s.configid=c.id AND c.name='fifoconfig';

INSERT INTO config (id,name) VALUES (10,'fifoconfig');
INSERT INTO configsection (configid,id,name) VALUES (10,1000,'General');
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,'network','FifoNet',13);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,'sim-time-limit','100h',14);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1000,'cpu-time-limit','300s',15);
INSERT INTO configsection (configid,id,name) VALUES (10,1001,'Config Fifo1');
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1001,'description','\"low job arrival rate\"',16);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1001,'**.gen.sendIaTime','exponential(0.2s)',17);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1001,'**.fifo.serviceTime','0.01s',18);
INSERT INTO configsection (configid,id,name) VALUES (10,1002,'Config Fifo2');
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1002,'description','\"high job arrival rate\"',19);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1002,'**.gen.sendIaTime','exponential(0.01s)',20);
INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (1002,'**.fifo.serviceTime','0.01s',21);

