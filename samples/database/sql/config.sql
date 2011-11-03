#
# Create tables for cMySQLConfigReader.
#

DROP TABLE IF EXISTS configentry;
DROP TABLE IF EXISTS configsection;
DROP TABLE IF EXISTS config;

CREATE TABLE config (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     name VARCHAR(80) NOT NULL
);

CREATE TABLE configsection (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     configid INT NOT NULL,
     name VARCHAR(80) NOT NULL,
     UNIQUE KEY (configid, name),
     FOREIGN KEY (configid) REFERENCES config(id)
);

CREATE TABLE configentry (
     sectionid INT NOT NULL,
     name VARCHAR(200) NOT NULL,
     value VARCHAR(200) NOT NULL,
     entryorder INT,
     KEY (sectionid, name),
     KEY (sectionid, entryorder),
     FOREIGN KEY (sectionid) REFERENCES configsection(id)
);

