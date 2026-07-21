import subprocess
import os
import shutil
import utils


MODEL_DIR = "/tmp/model/"


def run_simulation(source_zip, executable, arguments):

    # housekeeping
    shutil.rmtree(MODEL_DIR, ignore_errors=True)
    os.makedirs(MODEL_DIR)
    os.chdir(MODEL_DIR)

    # unzip source_zip
    utils.unzip_bytes(source_zip)

    # run make, first clean just to be sure
    subprocess.call(["make", "clean"])
    subprocess.call(["make", "MODE=release"])

    # execute binary with args
    subprocess.call([executable] + arguments)

    # zip results
    results_zip = utils.zip_directory("results")

    # cleaning up
    shutil.rmtree(MODEL_DIR, ignore_errors=True)

    # return zip
    return results_zip
