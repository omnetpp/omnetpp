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

Steps to get things up and running:

1. First, you obviously need a working MySQL database. Make sure you can
   log in using the MySQL console ("mysql"), and you can create tables,
   insert rows etc using SQL. Also make sure that you have the necessary
   header files (mysql.h, etc) and libraries (libmysql.a/so) to build programs
   with MySQL access -- they are in the MySQL-devel packages on Linux.

2. Take the given C++ sources, and compile+link them into your simulation.
   (Use Token Ring sample simulation if you're unsure -- the sample database
   contents for demonstrating cMySQLConfiguration is for this model.)
   On Linux/Unix, you can also build a shared library, and have it loaded
   into the simulation dynamically (using the load-libs= omnetpp.ini
   entry) [1]. To get the sources built, the MySQL headers AND OMNeT++'s
   src/envir directory need to be in the include path (-I compiler options),
   and you need to link against the MySQL library (-lmysql).

3. Create the database tables: log in using the MySQL console, and
   copy/paste the contents of the scripts in sql/ into it. (Or use the
   "source <filename>" command at the mysql prompt.) If you want to try
   cMySQLNetBuilder or cMySQLConfiguration, fill in the database with
   the sample data by executing the *.sql files in the example subdirectories
   in much the same way.

4. Adjust the database name, user, password and other connection properties
   in the ini files in the example directories. The full set of config options
   understood are documented in oppmysqlutils.h as:
      [General]
      mysql-host = <hostname>
      mysql-user = <username>
      mysql-password = <password>
      mysql-database = <database-name>
      mysql-port = <TCP-port-number>
      mysql-socket = <unix-socket-name>
      mysql-clientflag = <int>
      mysql-use-pipe = <true/false>  # Windows named pipes
      mysql-options-file = <MySQL-options-filename>
   Not all of them make sense together and most have defaults; look up
   the MySQL documentation if in trouble.

5. Run the simulation! If you get an "Error connecting to MySQL" error,
   go back to Step 4. Otherwise you should be able to query the data
   on the MySQL console, try e.g. "SELECT * FROM vector;"

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

The code has been tested with MySQL 4.1.

Enjoy!


