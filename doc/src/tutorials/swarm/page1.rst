Installation and Usage
======================

In this page we show you how to deploy and use the tools to run simulations on AWS.

.. caution::
    The solution and the toolset presented here are currently experimental, and we are collecting
    feedback about it. Please let us know if you are interested, and if you try it,
    we request you to report bugs, errors or any difficulty you experience in using it.
    We'd also be happy to hear about successful usage.
    Use the bugtracker link at the bottom of this page for feedback.


Installation
------------

Install Docker, python3 and pip3 on your local computer. We recommend that you use Linux.
A working OMNeT++ installation, and a checked-out INET (fork) git repository is also necessary.


Deployment on AWS
~~~~~~~~~~~~~~~~~

Creating an AWS Account
^^^^^^^^^^^^^^^^^^^^^^^

To access any web service AWS offers, you must first create an AWS account at http://aws.amazon.com.
If you already have one, just log in to the `AWS Management Console <https://console.aws.amazon.com>`_ with it;
otherwise, follow the instructions here: `AWS registration <https://portal.aws.amazon.com/gp/aws/developer/registration/index.html>`_.

During registration, you will have to enter your credit card number. Amazon will charge it with a small amount (1 USD) to verify it.

Creating the Access Policy
^^^^^^^^^^^^^^^^^^^^^^^^^^

On the AWS Management Console, navigate to the IAM Service configuration page, and switch to **Policies**. Or just
click `this link <https://console.aws.amazon.com/iam/home#/policies>`_, it will take you there.

Click the **Create policy** button. Switch over to the **JSON** tab.
Paste the contents of :download:`this file <code/docker-for-aws-policy.json>` into the entry field, replacing its entire contents.

This policy is the superset of the officially published one on the `Docker for AWS website <https://docs.docker.com/docker-for-aws/iam-permissions/>`_.
It had to be slightly altered to make it fit into the default size limit, so it grants slightly more privileges than necessary.
It also adds the ``ec2:CreateKeyPair`` and the ``cloudwatch:PutMetricAlarm`` permissions.
These are necessary to automate the connection to the swarm, and the shutting down of the machines after they have been idle for a while.

Click **Review policy**. Enter a **Name** for the new policy, for example "inet-docker-swarm-policy", then click **Create policy**.

Creating the User
^^^^^^^^^^^^^^^^^

Switch over to `**Users** <https://console.aws.amazon.com/iam/home#/users>`__ and click **Add user**.

Enter a **User name**, for example "inet-swarm-cli", and tick the checkbox next to **Programmatic access**. Leave the other checkbox unchecked. Click **Next: Permissions**. Select **Attach existing policies directly**, search for the name of the policy we just created *(by typing in a part of it, like "inet")*, then check the checkboy next to it, and click **Next: Review** at the bottom. If everything looks alright, click **Create user**.

As the final step of user creation, save the **Access key ID** and the **Secret access key**, somewhere safe. It's a good idea that you do this by clicking **Download .csv**. This will let you download this information into a simple text file, so you won't make any mistakes while copy-pasting them.

Also, read the notice in the green area, particularly this part:
*"This is the last time these credentials will be available to download. However, you can create new credentials at any time."*
This means that if you don't save the key ID and the secret key now, you will have to delete this user and create a new one.

.. caution::
    **Important**
    
    Keep these strings private, since these grant access to your account, without the need for your password or other credentials.
    (Of course, only until you delete this user, or revoke its permissions.) Treat them with similar caution as you do your passwords.


Configuring CLI Access
^^^^^^^^^^^^^^^^^^^^^^

To let your computer manage and use your AWS account, we have to configure it with the credentials of the user you just created for it.
First we need to install the AWS CLI utility using ``pip``:

.. code-block:: bash

    $ pip3 install --user --upgrade awscli

Then start the configuration:

.. code-block:: bash

    $ aws configure

If at first you get ``aws: command not found``, or ``The program 'aws' is currently not installed``, try running ``~/.local/bin/aws configure`` instead.

When asked, enter the **Access Key ID**, then the **Secret Access Key**. They will be recorded into an INI file, which is by default at ``~/.aws/credentials``.

For **Default region**, choose the one closest to you geographically. You can find the list of region codes and their locations
`here <https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/using-regions-availability-zones.html#concepts-available-regions>`_.
In my case, the Frankfurt datacenter was closest, so I entered ``eu-central-1``. This setting is recorded in the ``~/.aws/config`` file.
You can find more info about Regions and Availability Zones
`here <https://docs.aws.amazon.com/general/latest/gr/rande.html>`_,
`here <https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/using-regions-availability-zones.html>`_, and
`here <https://aws.amazon.com/about-aws/global-infrastructure/>`_.

You can leave **Default output format** empty.

.. caution::
    **Important**
    
    Once all this information is entered correctly, any software you run on your computer has access
    to your AWS account, as permitted by the policy attached to the configured user.
    Remove (rename) the ``credentials`` file mentioned above to (temporarily) disable access.
    The proper way to completely and permanently revoke this access is to delete the IAM User we just created.

From this point on in this tutorial, we won't need the AWS Management Console to initiate any actions. However, if you wish, you can use it to observe and check for yourself what the ``aws_swarm_tool.py`` script does.


Installing Local Client Scripts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Having set up your AWS account and access to it, we can proceed by installing the client-side tools for submitting simulations.

First, install the required Python libraries:

.. code-block:: bash

    $ pip3 install --user --upgrade boto3 psutil pymongo redis rq

Save the following files into ``~/.local/bin``:

* :download:`aws_swarm_tool.py <code/aws_swarm_tool.py>`
* :download:`docker-compose.yml <code/docker-compose.yml>`
* :download:`inet_runall_swarm.py <code/inet_runall_swarm.py>`

Make the script files executable, and put ``~/.local/bin`` into the PATH if it does not already contain it:

.. code-block:: bash

    $ cd ~/.local/bin; chmod +x aws_swarm_tool.py inet_runall_swarm.py
    $ export PATH=$PATH:$HOME/.local/bin

Run the following command to set up the virtual machines and necessary infrastructure on AWS,
and perform local configuration for accessing it:

.. code-block:: bash

    $ aws_swarm_tool.py init


Usage
-----

Running Simulations
~~~~~~~~~~~~~~~~~~~

Change into the directory under INET (or your INET fork) that contains your simulation.

IMPORTANT: Your INET installation should be a checked-out copy of a GitHub repository with all changes pushed up to GitHub,
because our tool only sends the Git URL of your project and the hash of the currently checked-out commit to AWS, not the full source code.
Additionally, OMNeT++ should be ready to use, with its tools (like ``opp_run`` or ``opp_run_dbg``) accessible as commands.


.. code-block:: bash

    $ cd examples/inet/ber

Enter the command for running the simulations, using our ``inet_runall_swarm.py`` program instead of ``./run`` or ``inet``:

.. code-block:: bash

    $ inet_runall_swarm.py -c ber-flavour-experiment

The ``inet_runall_swarm.py`` tool will expand the list of simulation runs to be executed, submit them to the job queue, and wait for the jobs to finish.
The results will be downloaded automatically into the ``results`` folder.

You can monitor progress at http://localhost:9181/ which displays the content of the job queue.

Stopping, Restarting and Deleting the Swarm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once you are done, you can stop the machines:

.. code-block:: bash

    $ aws_swarm_tool.py halt

They will also shut down automatically after an hour of being idle.

.. caution::
    **Important**
    
    AWS usage is billed by uptime, i.e. you are charged for the time the Swarm is up and running,
    regardless whether you are actually using it for simulations or not. Our tool configures AWS
    to halt the Swarm after one hour of idle time, but for extra safety, double-check that the
    Swarm has been stopped after you have finished working with it. WE ARE NOT RESPONSIBLE FOR
    EXTRA COSTS CAUSED BY LEAVING AWS NODES RUNNING. Read on for instructions.

The ``halt`` command only shuts down the virtual machine instances, but leaves all other resources intact,
this way resuming the usage is faster than the initial deployment (with ``init``). These additional
resources being there inactively do not cost you anything.


To use the Swarm again, you have to start it up again:

.. code-block:: bash

    $ aws_swarm_tool.py resume

To completely delete the entire Swarm:

.. code-block:: bash

    $ aws_swarm_tool.py delete

Checking the Status Manually
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. caution::
    **Important**
    
    When you do not intend to work with AWS for an extended period of time, you may want to
    double-check manually that the stack on AWS has been stopped indeed, and you are no longer
    being billed for it.

To check the status, select the EC2 service on the AWS Management Console,
and check the `Running Instances <https://console.aws.amazon.com/ec2/v2/home#Instances>`_.
If you see instances named ``inet-Node`` or ``inet-Manager``, in the ``running`` state, the Swarm is active.

To delete the Swarm manually (if the command-line tool does not work for some reason), go to the
`CloudFormation service page <https://console.aws.amazon.com/cloudformation/home#/stacks>`_ on
the AWS Management Console. Tick the checkbox next to the ``inet`` Stack in the table, then click
**Actions > Delete Stack**. This should remove all INET-related resources.

Feedback and Discussion
-----------------------

We are looking for any kind of feedback or suggestion you might have regarding this tutorial here:
https://github.com/omnetpp/omnetpp-tutorials/issues/3
