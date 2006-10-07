#
# Create tables for cMySQLNetBuilder.
#

DROP TABLE IF EXISTS network;
DROP TABLE IF EXISTS node;
DROP TABLE IF EXISTS link;
DROP TABLE IF EXISTS parameter;

CREATE TABLE network (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     name VARCHAR(80)
);

CREATE TABLE node (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     networkid INT,
     type VARCHAR(80),
     name VARCHAR(80),
     display VARCHAR(200)
);

CREATE TABLE link (
     srcnodeid INT,
     destnodeid INT,
     duplex BOOL,
     delay DOUBLE PRECISION,
     error DOUBLE PRECISION,
     datarate DOUBLE PRECISION
);

CREATE TABLE parameter (
     nodeid INT,
     name VARCHAR(200),
     value VARCHAR(200)
);
