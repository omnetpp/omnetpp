
Installation Tests
==================

This test verifies the installation of OMNeT++ using the `./install.sh` script
on various Linux distributions. Docker images and the x11docker script are
utilized for this purpose.

To begin, extract your desired version of OMNeT++ into the `ut`
(under test) directory. This directory will be mapped to the `/mnt` folder
within the Docker container.

Start the Linux distribution of your choice with the following command (e.g.):

    $ ./opp_docker_gui_shell ubuntu:latest

This command will download and start an Ubuntu Docker container, setting up
the environment to enable access to graphical programs. 

[!NOTE] By default, the shell uses the same user ID as the caller and the
root password is 'x11docker'.

[!NOTE] The container will be stopped and removed after exiting the shell.

Navigate to `/mnt/omnetpp-x.y` and execute the `./install.sh` script to test the
installation procedure.

The installation of dependencies and the OMNeT++ build must complete successfully.

Alternatively you can use

    $ ./opp_docker_shell ubuntu:latest

which will start a terminal only docker shell. This is suitable to test non-gui
docker installations.