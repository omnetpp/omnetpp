#!/usr/bin/env python3

import os
import sys
import time
import signal
import tempfile
import subprocess
import argparse

#import socket
#import urllib.request
#import http.client

import psutil

import boto3
from botocore.exceptions import ClientError

ec2 = boto3.resource("ec2")
cloudformation = boto3.resource("cloudformation")
autoscaling = boto3.client("autoscaling")
cloudwatch = boto3.resource("cloudwatch")


# def _check_credentials():
#    if boto3.Session().get_credentials() is None:
#        raise Exception("No AWS credentials")


# def import_key_pair(key_file_name, key_pair_name):
#    """ imports an existing local key into aws """
#    pass


def create_add_key_pair(key_pair_name="inet_key"):
    key_pair = ec2.create_key_pair(KeyName=key_pair_name)

    # ssh-add complains if the .pem file has more permissions than 600
    old_umask = os.umask(0o177)
    try:
        with open(key_pair_name + ".pem", "wt", 0o600) as key_file:
            key_file.write(key_pair.key_material)
    except:
        pass
    finally:
        os.umask(old_umask)

    subprocess.call(["ssh-add", key_pair_name + ".pem"])


# which_asg is either "manager", or "worker".
# It is compared case-insensitively, because the official template
# named them "Manager" and "worker". And it bothers me.
def _get_asg_name(stack_name, which_asg):

    asgs = autoscaling.describe_auto_scaling_groups()

    # find the real name of the first AutoScalingGroup which
    # has the tag "Name" on it with the value stack_name + "-" + which_asg
    asg_name = [
        asg['AutoScalingGroupName']
        for asg in asgs['AutoScalingGroups']
        if [
            tag
            for tag in asg['Tags']
            if tag['Key'] == 'Name' and tag['Value'].lower() == (stack_name + '-' + which_asg).lower()
        ]
    ][0]

    return asg_name


def _get_manager_public_ip(stack_name):
    manager_asg_name = _get_asg_name(stack_name, "manager")

    group_info = autoscaling.describe_auto_scaling_groups(
        AutoScalingGroupNames=[manager_asg_name])

    try:
        manager_instance_id = group_info['AutoScalingGroups'][0]['Instances'][0]['InstanceId']
    except IndexError:
        manager_instance_id = None

    if manager_instance_id is None:
        raise Exception("Swarm manager not found")

    manager_instance = ec2.Instance(manager_instance_id)
    manager_public_ip = manager_instance.public_ip_address

    return manager_public_ip


def create_swarm(stack_name="inet", key_pair_name="inet_key", num_workers=3, instance_type="c4.xlarge"):
    """ creates the various resources on AWS using the official "Docker for AWS" CloudFormation Template """

    stack = cloudformation.create_stack(
        StackName=stack_name,
        TemplateURL='https://editions-us-east-1.s3.amazonaws.com/aws/stable/Docker.tmpl',
        Capabilities=['CAPABILITY_IAM'],
        Parameters=[
            {'ParameterKey': 'KeyName',
             'ParameterValue': key_pair_name},
            {'ParameterKey': 'InstanceType',
             'ParameterValue': instance_type},
            {'ParameterKey': 'ManagerInstanceType',
             'ParameterValue': instance_type},
            {'ParameterKey': 'ManagerSize',
             'ParameterValue': '1'},
            {'ParameterKey': 'ClusterSize',
             'ParameterValue': str(num_workers)},
        ])

    # This could be done properly with a boto3 Waiter...
    print("Waiting for resources to be created... This will take about 10 minutes")
    while stack.stack_status != 'CREATE_COMPLETE':
        time.sleep(5)

        stack.reload()
        resources = list(stack.resource_summaries.iterator())

        num_ready = sum(
            1 for r in resources if r.resource_status == 'CREATE_COMPLETE')
        num_creating = sum(
            1 for r in resources if r.resource_status == 'CREATE_IN_PROGRESS')

        # erase the current output line
        sys.stdout.write(u"\u001b[1000D\u001b[0K")

        print("|" + num_ready * "=" + num_creating * "+" +
              (42 - num_creating - num_ready) * " " + "|", end="", flush=True)

    print()

    print("Creating alarm and scaling policy to terminate all instances after a long period of inactivity")

    manager_asg_name = _get_asg_name(stack_name, "manager")
    worker_asg_name = _get_asg_name(stack_name, "worker")

    # shut down all instances if the alarm is fired
    manager_scaling_policy = autoscaling.put_scaling_policy(
        AutoScalingGroupName=manager_asg_name,
        PolicyName='terminate-manager', PolicyType="SimpleScaling",
        AdjustmentType="ExactCapacity", ScalingAdjustment=0)

    worker_scaling_policy = autoscaling.put_scaling_policy(
        AutoScalingGroupName=worker_asg_name,
        PolicyName='terminate-workers', PolicyType="SimpleScaling",
        AdjustmentType="ExactCapacity", ScalingAdjustment=0)

    # alarm if in the last 12 consecutive 5-minute periods the maximum CPU utilization of the manager was below 10%
    # and execute both terminate policies
    metric = cloudwatch.Metric(namespace='AWS/EC2', name='CPUUtilization')
    metric.put_alarm(
        AlarmName=stack_name + "-manager-idle",
        Dimensions=[{'Name': 'AutoScalingGroupName',
                     'Value': manager_asg_name}],
        Statistic="Maximum",
        AlarmActions=[manager_scaling_policy["PolicyARN"],
                      worker_scaling_policy["PolicyARN"]],
        Period=5 * 60, EvaluationPeriods=12, ComparisonOperator='LessThanThreshold', Threshold=10)

    print("Done.")


def connect_to_swarm(stack_name="inet"):
    """ creates an SSH tunnel to the manager machine, forwarding all necessary ports """

    manager_public_ip = _get_manager_public_ip(stack_name)

    print("Opening tunnel to the manager at " + manager_public_ip)

    pid_file_name = os.path.join(
        tempfile.gettempdir(), stack_name + "-ssh-tunnel.pid")

    try:
        with open(pid_file_name, "rt") as existing_pid_file:
            existing_pid = int(existing_pid_file.readline().strip())
            print("found a pid file for this stack, with pid " + str(existing_pid))
            if existing_pid in psutil.pids():
                print("You are already connected.")
                return
            else:
                print("But that process is no longer running. Starting a new one...")
    except IOError:
        # good, no such pid file yet
        pass

    ssh = subprocess.Popen([
        "ssh", "docker@" + manager_public_ip, "-N", "-q",
        "-o", "StrictHostKeyChecking=no", "-o", "ExitOnForwardFailure=yes",
        "-L", "localhost:2374:/var/run/docker.sock",
        "-L", "9181:172.17.0.1:9181", "-L", "8080:172.17.0.1:8080",
        "-L", "27017:172.17.0.1:27017", "-L", "6379:172.17.0.1:6379"])

    with open(pid_file_name, "wt") as pid_file:
        pid_file.write(str(ssh.pid))

    print("SSH started, its pid is " + str(ssh.pid) + ".")

    while True:
        time.sleep(1)
        exit_code = subprocess.call(["docker", "-H", "tcp://localhost:2374", "info"],
                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if exit_code == 0:
            break

    print("Tunnel is up!")

    # from now on you can do:  docker -H tcp://localhost:2374 info


def disconnect_from_swarm(stack_name="inet"):
    pid_file_name = os.path.join(
        tempfile.gettempdir(), stack_name + "-ssh-tunnel.pid")

    try:
        with open(pid_file_name, "rt") as existing_pid_file:
            existing_pid = int(existing_pid_file.readline().strip())
        print("Found an SSH PID file for this stack, with PID " + str(existing_pid))
        if existing_pid in psutil.pids():
            print("Process still running, killing it...")
            os.kill(existing_pid, signal.SIGTERM)
            print("Tunnel is down!")
            os.remove(pid_file_name)
        else:
            print("But that process is no longer running. Removing pid file...")
            os.remove(pid_file_name)
    except IOError as err:
        print("Your are not connected.")


def deploy_app(stack_name="inet"):

    print("Deploying the stack...")

    subprocess.call(["docker", "-H", "tcp://localhost:2374", "stack", "up",
                     "--compose-file", os.path.dirname(os.path.realpath(__file__)) + "/docker-compose.yml", stack_name])

    """
    print("Waiting for the services to start...")

    # wait for completion by polling the visualizer http service - this does not seem to work
    while True:
        time.sleep(1)
        try:
            if urllib.request.urlopen("http://localhost:9181/queues.json", timeout=1).getcode() == 200:
                break
        except urllib.error.URLError:
            pass
        except http.client.HTTPException:
            pass
        except socket.timeout:
            pass
    """


def halt_swarm(stack_name="inet"):
    manager_asg_name = _get_asg_name(stack_name, "manager")
    worker_asg_name = _get_asg_name(stack_name, "worker")

    autoscaling.update_auto_scaling_group(
        AutoScalingGroupName=manager_asg_name, MinSize=0, DesiredCapacity=0)
    autoscaling.update_auto_scaling_group(
        AutoScalingGroupName=worker_asg_name, MinSize=0, DesiredCapacity=0)

    print("Waiting for instances to terminate...")

    try:
        while True:
            time.sleep(5)

            manager_asg = autoscaling.describe_auto_scaling_groups(
                AutoScalingGroupNames=[manager_asg_name])["AutoScalingGroups"][0]
            worker_asg = autoscaling.describe_auto_scaling_groups(
                AutoScalingGroupNames=[worker_asg_name])["AutoScalingGroups"][0]

            manager_instances = manager_asg["Instances"]
            worker_instances = worker_asg["Instances"]

            if not manager_instances and not worker_instances:
                break
            else:
                # erase the current output line
                sys.stdout.write(u"\u001b[1000D\u001b[0K")
                print("Remaining manager: " + str(len(manager_instances)) +
                      ", workers: " + str(len(worker_instances)), end="", flush=True)

        print()

        print("Done.")

    except KeyboardInterrupt:
        print("\nWaiting aborted, but the operation will still proceed.")


def resume_swarm(stack_name="inet", num_workers=3):
    manager_asg_name = _get_asg_name(stack_name, "manager")
    worker_asg_name = _get_asg_name(stack_name, "worker")

    autoscaling.update_auto_scaling_group(
        AutoScalingGroupName=manager_asg_name, DesiredCapacity=1)
    autoscaling.update_auto_scaling_group(
        AutoScalingGroupName=worker_asg_name, DesiredCapacity=num_workers)

    print("Waiting for instances to start...")

    try:
        while True:
            time.sleep(5)

            manager_asg = autoscaling.describe_auto_scaling_groups(
                AutoScalingGroupNames=[manager_asg_name])["AutoScalingGroups"][0]
            worker_asg = autoscaling.describe_auto_scaling_groups(
                AutoScalingGroupNames=[worker_asg_name])["AutoScalingGroups"][0]

            running_manager_instances = [
                inst for inst in manager_asg["Instances"] if inst["LifecycleState"] == "InService"]
            running_worker_instances = [
                inst for inst in worker_asg["Instances"] if inst["LifecycleState"] == "InService"]

            if len(running_manager_instances) < 1 and len(running_worker_instances) < num_workers:
                # erase the current output line
                sys.stdout.write(u"\u001b[1000D\u001b[0K")
                print("Manager ready: " + str(len(running_manager_instances)) + "/1"
                      ", workers ready: " + str(len(running_worker_instances)) + "/" + str(num_workers), end="", flush=True)
            else:
                break

        print()

        print("Done.")

        manager_public_ip = _get_manager_public_ip(stack_name)

        print("Waiting for the swarm to reassemble...")

        while True:
            time.sleep(5)
            exit_code = subprocess.call(
                ["ssh", "-o", "StrictHostKeyChecking=no", "docker@" + manager_public_ip,
                 "docker", "node", "inspect", "self"],
                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if exit_code == 0:
                break

        print("Ready!")

    except KeyboardInterrupt:
        print("\nWaiting aborted, but the operation will still proceed.")


def remove_swarm(stack_name="inet"):
    stack = cloudformation.Stack(stack_name)

    stack.delete()
    print("Waiting for resources to be deleted... This will take a couple minutes.")

    while stack.stack_status == 'DELETE_IN_PROGRESS':
        time.sleep(5)

        try:
            stack.reload()
            resources = list(stack.resource_summaries.iterator())
        except ClientError:
            # the stack might not be there anymore, but it's fine
            break

        num_deleting = sum(
            1 for r in resources if r.resource_status == 'DELETE_IN_PROGRESS')
        num_remaining = sum(
            1 for r in resources if r.resource_status == 'CREATE_COMPLETE')

        # erase the current output line
        sys.stdout.write(u"\u001b[1000D\u001b[0K")

        print("|" + num_remaining * "=" + num_deleting * "-" +
              (42 - num_remaining - num_deleting) * " " + "|", end="", flush=True)

    print()

    # The CPU idle alarm and scaling policies we added are also deleted automatically

    print("Done.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Tool to manage an INET Swarm app on AWS")

    parser.add_argument('command', metavar='COMMAND',
                        choices=['init', 'connect', 'disconnect', 'deploy', 'halt', 'resume', 'delete'])

    args = parser.parse_args()

    if args.command == "init":
        create_add_key_pair()
        create_swarm()
        connect_to_swarm()
        deploy_app()
    elif args.command == "connect":
        connect_to_swarm()
    elif args.command == "disconnect":
        disconnect_from_swarm()
    elif args.command == "deploy":
        deploy_app()
    elif args.command == "halt":
        halt_swarm()
    elif args.command == "resume":
        resume_swarm()
        connect_to_swarm()
        deploy_app()
    elif args.command == "delete":
        remove_swarm()
