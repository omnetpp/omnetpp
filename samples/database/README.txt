Database
========

This is not a concrete simulation model, but rather a set of utility classes
to show how OMNeT++ models can make use of a database for input and output.

It demonstrates four ways of using a database:
  - reading the network topology from the database, instead of NED files;
  - getting the configuration (presently in omnetpp.ini) from the database;
  - recording output vectors to the database, instead of omnetpp.vec;
  - writing output scalars to the database, instead of omnetpp.sca.

The first one is implemented as a simple module (cMySQLNetBuilder),
and it is intended as an example that can be customised at will.
The latter three are plug-in extensions to Envir (cMySQLConfiguration,
cMySQLOutputVectorManager and cMySQLOutputScalarManager) and they are
completely generic -- they can be used with any simulation model to make them
datatabase-enabled, without having to change a single line of source code.

Possible uses of a database include:
  - if you're doing heaps of simulation experiments: organized storage
    for their configuration and results;
  - remote access and intelligent queries/reports via the "LAMP" setup
    (Linux+Apache+MySQL+PHP) and tools like PHP/SWF Charts or JpGraph;
  - many reporting tools like Eclipse/BIRT also work from a database;
  - interfacing with network management systems

The current code was written for the MySQL database (www.mysql.com), but it
can be easily ported to other databases like PostgreSQL or Oracle as well,
by making use of their C/C++ APIs.

Performance: I have observed that with MyISAM tables and INSERT DELAYED,
the performance drop with cMySQLOutputVectorManager was almost unnoticeable
(less than 5%) compared to file output. Measurements were done with the
Token Ring model, on Windows XP, with MySQL 4.1 running on the *same* machine.

Directory contents:
  - C++ sources (.cc/h) for the above classes;
  - the sql/ subdirectory contains SQL scripts to create the necessary
    database tables;
  - example1/ configures the Token Ring model to read module parameters
    from the database, and write output scalars and vectors there;
  - example2/ demonstrates cMySQLNetBuilder, and builds a network with a
    topology coming from the database. An SQL script to fill the database
    with the topology info (nodes and links) is provided.

Steps to get things up and running (These instructions use Linux command-line
syntax, but the equivalent will work on Windows too):

1. First, you obviously need a working MySQL database. The code has been
   tested with MySQL 4.1, so that's what we recommend. Make sure you can
   log in using the MySQL console ("mysql"), and you can create tables,
   insert rows etc using SQL.

   $ mysql test   # test is the database name; any other database can be used
   mysql> CREATE TABLE dummy (name CHAR(80));
   mysql> DROP TABLE dummy;
   mysql> exit;

   On Windows, you'll likely need the -W, -p and -u <username> options to
   "mysql" to log in; the default user is root/root.

   Also make sure that you have the necessary header files (mysql.h, etc)
   and libraries (libmysqlclient.so on Linux, libmysql.lib+libmysql.dll on
   Windows) to build programs with MySQL access. If you are installing from RPM
   in Linux, you need the "-devel" package to get these files. If installed,
   they usually can be found in /usr/include/mysql and /usr/lib. On Windows,
   you need the full package (~40 Meg) not just the Essentials one.

2. Create the database tables: log in using the MySQL console, and
   copy/paste the contents of the scripts in sql/ into it. (Or use the
   "source <filename>" command at the mysql prompt.) If you want to try
   cMySQLNetBuilder or cMySQLConfiguration, fill in the database with
   the sample data by executing the *.sql files in the example subdirectories
   in much the same way.

   $ cd samples/database/sql
   $ mysql test
   mysql> source config.sql;
   mysql> source netbuilder.sql;
   mysql> source scalars.sql;
   mysql> source vectors.sql;
   mysql> exit;

3. Take the given C++ sources, and compile them into your simulation.
   (Use Token Ring sample simulation if you're unsure -- the sample database
   contents for demonstrating cMySQLConfiguration is for this model.)
   To get the sources built, the MySQL headers AND OMNeT++'s src/envir
   directory need to be in the include path (-I compiler options),
   and you need to link against the MySQL library (-lmysqlclient).

   For example, to add database support to the Token Ring model, you'd do the
   following:

   $ cd samples/database
   $ cp *.cc *.h ../tokenring
   $ cd ../tokenring
   $ opp_makemake -f -N -x -I/usr/include/mysql -I$HOME/omnetpp/src/envir
           -lmysqlclient
   $ make

   On Windows, the last commands would look like this (provided you installed
   MySQL into C:\MySQL;):

   > opp_nmakemake -f -N -x -IC:/MySQL/include -IC:/OMNeT++/src/envir
           -LC:/MySQL/lib -llibmysql.lib
   > nmake -f Makefile.vc

   NOTE that on Windows you need to link against libmysql.lib and **not**
   mysqlclient.lib as on Linux; and the program will need libmysql.dll as well
   to run. You won't be able to link against mysqlclient.lib because of multi-
   threaded/single-threaded library issues (there'll be always undefined
   symbols.) If you installed MySQL to the default location under Program Files,
   you'd better use the short directory names like C:/PROGRA~1/MySQL/MYSQLS~1.1
   in the opp_nmakemake command line (dir /x displays them.)

   On Linux, you can also build a shared library, and have it loaded
   into the simulation dynamically (using the load-libs= omnetpp.ini
   entry).

4. Fill the database with the input data. If you want to use cMySQLConfiguration,
   this means INSERTs into the config, configsection and configentry tables,
   and with cMySQLNetBuilder, into the network, node, link and parameter tables.

   To use the sample cMySQLConfiguration data for the Token Ring model, run the
   SQL script in example1:

   $ cd samples/database/example1
   $ mysql test
   mysql> source tokenring-config.sql
   mysql> exit

   You can load the sample topology data for cMySQLNetBuilder in example2/
   net60.sql in much the same way.

5. Prepare an omnetpp.ini to activate the MySQL extensions. Add one or more
   of the following entries:

   [General]
   configuration-class = "cMySQLConfiguration"
   outputvectormanager-class = "cMySQLOutputVectorManager"
   outputscalarmanager-class = "cMySQLOutputScalarManager"

   In addition, you'll also need to specify the database, possibly its location
   (host, port) login name and password too. The full set of config options is
   documented in oppmysqlutils.h.

   [General]
   mysql-host = <hostname>
   mysql-user = <username>
   mysql-password = <password>
   mysql-database = <database-name>
   mysql-port = <TCP-port-number>
   mysql-socket = <unix-socket-name>
   mysql-clientflag = <int>
   mysql-use-pipe = <true/false>  # use Windows named pipes
   mysql-options-file = <MySQL-options-filename>

   Not all of them make sense together and most have defaults. For example,
   with a Linux MySQL default installation and no password (which is, BTW,
   not recommended) I got away with setting the mysql-database= entry only.
   Look up the MySQL documentation if in trouble.

   For the Token Ring example, the provided sample ini file will likely work,
   just copy it over from the example1/ directory:

   $ cp tokenring-db.ini ../../tokenring
   $ cd ../../tokenring

6. Run the simulation with the new ini file.

   $ ./tokenring -f tokenring-db.ini

   If you get an "Error connecting to MySQL", you'll need to play with
   the settings in Step 5.

   When you click Run, your output vectors and output scalars will go into
   the database.

7. Congrats! Now you should be able to query the data on the MySQL console:

   $ mysql test;
   mysql> SELECT * FROM vector;
   mysql> SELECT * FROM vecdata WHERE vectorid=...;
   mysql> SELECT * FROM run;
   mysql> SELECT * FROM scalar WHERE runid=...;
   ...and so on.

A hint about cMySQLConfiguration:

The easiest way to transfer the contents of an existing omnetpp.ini file
into the database is by using the mysqlconfiguration-dumpbootcfg=true
configuration option, which prints the current configuration as a series
of SQL INSERT statements. Add the following lines to your omnetpp.ini:
      [General]
      configuration-class = "cMySQLConfiguration"
      mysqlconfiguration-dumpbootcfg = true
      ...
Then start the simulation and redirect the output into a file. After some
editing, the result can be used as an SQL script.

Enjoy!

