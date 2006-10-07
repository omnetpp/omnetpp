#
# Create tables for cMySQLOutputVectorManager.
#

DROP TABLE IF EXISTS vrun;
DROP TABLE IF EXISTS vector;
DROP TABLE IF EXISTS vecdata;

CREATE TABLE vrun (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     runnumber INT,
     network VARCHAR(80),
     date TIMESTAMP
) ENGINE = MYISAM;

CREATE TABLE vector (
     runid INT,
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     module VARCHAR(200),
     name VARCHAR(80)
) ENGINE = MYISAM;

CREATE TABLE vecdata (
     vectorid INT,
     time DOUBLE PRECISION,
     value DOUBLE PRECISION
) ENGINE = MYISAM;


