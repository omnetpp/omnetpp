#
# Create tables for cMySQLNetBuilder.
#

DROP TABLE IF EXISTS linkparameter;
DROP TABLE IF EXISTS link;
DROP TABLE IF EXISTS nodeparameter;
DROP TABLE IF EXISTS node;
DROP TABLE IF EXISTS network;

CREATE TABLE network (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     name VARCHAR(80) NOT NULL
);

CREATE TABLE node (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     networkid INT NOT NULL,
     type VARCHAR(80) NOT NULL,
     name VARCHAR(80) NOT NULL,
     display VARCHAR(200),
     FOREIGN KEY (networkid) REFERENCES network(id)
);

CREATE TABLE nodeparameter (
     nodeid INT NOT NULL,
     name VARCHAR(200) NOT NULL,
     value VARCHAR(200) NOT NULL,
     FOREIGN KEY (nodeid) REFERENCES node(id)
);

CREATE TABLE link (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     srcnodeid INT NOT NULL,
     srcgatevector VARCHAR(80) NOT NULL,
     destnodeid INT NOT NULL,
     destgatevector VARCHAR(80) NOT NULL,
     channeltype VARCHAR(80) NOT NULL,
     FOREIGN KEY (srcnodeid) REFERENCES node(id),
     FOREIGN KEY (destnodeid) REFERENCES node(id)
);

CREATE TABLE linkparameter (
     linkid INT  NOT NULL,
     name VARCHAR(200) NOT NULL,
     value VARCHAR(200) NOT NULL,
     FOREIGN KEY (linkid) REFERENCES link(id)
);

