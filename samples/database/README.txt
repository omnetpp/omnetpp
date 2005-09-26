Database
========

This is not a concrete simulation model, but rather a set of utility classes
to show how OMNeT++ models can make use of a database for input and output.

It demonstrates four ways of using a database:
  - reading the network topology from the database, instead of NED files;
  - getting the configuration (presently in omnetpp.ini) from the database;
  - recording output vectors to the database, instead of omnetpp.vec;
  - writing output scalars to the database, instead of omnetpp.sca.

The first one is implemented as a simple module (cMySQLNetBuilder).

The latter three are plug-in extensions to Envir (cMySQLConfiguration,
cMySQLOutputVectorManager and cMySQLOutputScalarManager) which completely
generic -- they can be used with any simulation model to make it
datatabase-enabled, without having to change a single line of source code.

The current code was written for the MySQL database (www.mysql.com), but it
can be easily ported to other databases like PostgreSQL or Oracle as well,
by making use of their C/C++ APIs.

Directory contents:
  - C++ sources (.cc/h) for the above classes;
  - the sql/ subdirectory contains SQL scripts to create the necessary
    database tables;
  - example1/ configures the Token Ring model to read configuration
    (module parameters) from the database, and write output scalars
    and vectors there;
  - example2/ demonstrates cMySQLNetBuilder, and builds a network with a
    topology coming from the database. An SQL script to fill the database
    with the topology info (nodes and links) is provided.

Steps to get things up and running:

1. First, you obviously need a working MySQL database. Make sure you can
   log in using the "mysql" console, and you can create tables, insert
   rows etc, using SQL. Also make sure that you have the necessary header
   files (mysql.h, etc) and libraries (libmysql.a/so) to build programs
   with MySQL access -- they are in the MySQL-devel packages on Linux.

2. Take the given C++ sources, and compile+link them into your simulation.
   (Use Token Ring sample simulation if you're unsure -- the sample database
   contents for demonstrating cMySQLConfiguration is for this model.)
   On Linux/Unix, you can also build a shared library, and have it
   loaded into the simulation dynamically (using the load-libs= omnetpp.ini
   entry) [1]. To get the sources built, the MySQL includes AND OMNeT++'s src/envir
   directory need to be in the include path (-I compiler options), and you
   need to link against the MySQL library (-lmysql).

3.
You'll find your way around -- in sql/ there are scripts to create the
tables, and in examples/, well, two examples. The "tokenring" one is only
specific to that model in that the concrete module parameters in the config
database are for that model. The easiest way to put another model's existing
config file (say mymodel.ini) into the database is by adding
   include mymodel.ini
into tokenring.ini, and mysqlconfiguration-dumpbootcfg in it. When you start
the simulation, all ini file contents get dumped onto stdout in the form of
SQL INSERT statements, which only have to be edited a bit.

The code obviously hasn't received much testing, but it seems to work well
on my laptop (XP, VC7.1, MySQL 4.1). I tested it with the current 3.2 head,
but it should compile with 3.1 as well. Please let me know if there's any
problem.

[1] It is also possible to build a Windows DLL from them, but this
    has not yet been tested out and currently not recommended.
