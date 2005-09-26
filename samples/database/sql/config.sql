#
# Create tables for cMySQLConfiguration.
#

DROP TABLE IF EXISTS config;
DROP TABLE IF EXISTS configsection;
DROP TABLE IF EXISTS configentry;

CREATE TABLE config (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     name VARCHAR(80)
);

CREATE TABLE configsection (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     configid INT,
     name VARCHAR(80)
);

CREATE TABLE configentry (
     sectionid INT,
     name VARCHAR(200),
     value VARCHAR(200),
     entryorder INT
);
