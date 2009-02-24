Installing OMNeT++ on Linux/Unix
================================

1. Installing OMNeT++

Download the omnetpp source code and documentation from the omnetpp site.
Make sure you select to download the Unix archive omnetpp-<version>-src.tgz.

Copy the omnetpp archive to the directory where you want to install it
(usually your home directory). Extract the archive using the command:

    tar zxvf omnetpp-<version>-src.tgz

A sub-directory called omnetpp-<version> will be created which will contain the simulator
files. You should now add the following lines to your startup file (.bashrc
or .bash_profile if you're using bash; .profile if you're using some other
sh-like shell):

    export PATH=$PATH:~/omnetpp-<version>/bin

You will need to restart the shell before proceeding (logout and login again).

Make sure that the packages needed to build OMNeT++ are installed on your system.
On debian the following packages are mandatory:

sudo apt-get install build-essential gcc g++ bison flex perl tcl8.4 tcl8.4-dev \
                     tk8.4 tk8.4-dev blt blt-dev libxml2 libxml2-dev \
		     zlib1g zlib1g-dev libx11-dev

First you should check configure.user to make sure it contains the settings
you need however in most cases you don't need to change anything:
    vi configure.user

Then the usual GNU-like stuff:
    ./configure
    make

This will create both debug and release binaries.

You should now test all samples and check they run correctly. As an example,
the dyna example is started by entering the following commands:

    cd ~/omnetpp/samples/dyna
    ./dyna

By default the samples will run using the Tcl/Tk environment. You should see
nice gui windows and dialogs.

2. Starting the IDE

OMNeT++ 4.0 comes with a brand new IDE (based on Eclipse). You will need at least
sun-java5-jre installed on your machine (sun-java6-jre or openjdk-6-jre should work too).
You should be able to start the IDE by typing:

    omnetpp

NOTE: The IDE is supported ONLY on the following platforms:
 - Windows 32bit
 - Linux 32/64bit (i386)
 - Mac OS X 10.4/5 (i386/ppc)

3. Reconfiguring the libraries

If you need to recompile the OMNeT++ components with different flags (e.g.
different optimization), then cd to the top-level omnetpp directory, edit 
configure.user accordingly, then say:

    ./configure
    make clean
    make

If you want to recompile just a single library, then cd to the directory
of the library and type:

    make clean
    make

If you want to build ONLY release or debug builds, use:
(if MODE is not specified, both debug and release will be created)

    make MODE=release

If you want to create static libraries use:
(by default shared libraries will be created)

    make SHARED_LIBS=no

NOTE: The built libraries and programs are immediately copied to the 
lib/ and bin/ subdirs.


4. Portability issues

OMNeT++ has been tested on Linux, but it should work on other Unixes too,
although the IDE may not be supported. It probably makes things easier 
if you have gcc and the usual GNU tools installed, but you should be 
able to get along using other C++ compilers too. If you get it to work, 
please send a mail to the mailing list!

5. TIPS

- If you are using Advanced Desktop effects on Linux you may notice that all windows
  (including tooltip, menu etc.) are using the the open/close animation. This can be
  annoying for menus and tooltips. To disable this behavior (on Gnome), go to
  Advanced Desktop Effect Settings / Animations, and on the Open/Close Animation tab
  change the "type=Unknown" to "name=tk" selection entries. This will effectively
  disable the animation for non-toplevel Tcl/Tk windows.

- If you are running the IDE on Gnome, you will notice that a lot of space is vasted in
  list and treelist controls (this is a general GTK issue). To make the IDE a little
  more compact and usable, look into the 'contrib' directory. We have created a .gtkrc
  file which tweaks GTK controls to be a little bit more compact.

