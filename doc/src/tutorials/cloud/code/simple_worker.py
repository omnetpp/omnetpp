import os, subprocess

def greet(name):
    hostname = subprocess.check_output("hostname").decode("utf-8")[:-1]
    return "Hello, {}! I'm {}#{}.".format(name, hostname, os.getppid())
