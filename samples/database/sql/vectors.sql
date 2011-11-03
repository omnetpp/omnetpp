#
# Create tables for cMySQLOutputVectorManager.
#

DROP TABLE IF EXISTS vecdata;
DROP TABLE IF EXISTS vecattr;
DROP TABLE IF EXISTS vector;
DROP TABLE IF EXISTS vrun;

CREATE TABLE vrun (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     runnumber INT NOT NULL,
     network VARCHAR(80) NOT NULL,
     date TIMESTAMP NOT NULL
);

CREATE TABLE vector (
     id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
     runid INT NOT NULL,
     module VARCHAR(200) NOT NULL,
     name VARCHAR(80) NOT NULL,
     FOREIGN KEY (runid) REFERENCES vrun(id)
);

CREATE TABLE vecattr (
     vectorid INT NOT NULL,
     name VARCHAR(200) NOT NULL,
     value VARCHAR(200) NOT NULL,
     FOREIGN KEY (vectorid) REFERENCES vector(id)
);

CREATE TABLE vecdata (
     vectorid INT NOT NULL,
     time DOUBLE PRECISION NOT NULL,
     value DOUBLE PRECISION NOT NULL,
     FOREIGN KEY (vectorid) REFERENCES vector(id)
) ENGINE = MYISAM;

