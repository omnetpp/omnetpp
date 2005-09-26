#
# Create tables for cMySQLOutputScalarManager.
#

DROP TABLE IF EXISTS run;
DROP TABLE IF EXISTS scalar;

CREATE TABLE run (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     runnumber INT,
     network VARCHAR(80),
     date TIMESTAMP
) ENGINE = MYISAM;

CREATE TABLE scalar (
     runid INT,
     module VARCHAR(200),
     name VARCHAR(80),
     value DOUBLE PRECISION
) ENGINE = MYISAM;
