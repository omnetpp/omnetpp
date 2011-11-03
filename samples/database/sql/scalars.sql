#
# Create tables for cMySQLOutputScalarManager.
#

DROP TABLE IF EXISTS scalar;
DROP TABLE IF EXISTS run;

CREATE TABLE run (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     runnumber INT NOT NULL,
     network VARCHAR(80) NOT NULL,
     date TIMESTAMP NOT NULL
);

CREATE TABLE scalar (
     runid INT  NOT NULL,
     module VARCHAR(200) NOT NULL,
     name VARCHAR(80) NOT NULL,
     value DOUBLE PRECISION NOT NULL,
     KEY (runid,module,name),
     FOREIGN KEY (runid) REFERENCES run(id)
) ENGINE = MYISAM;


