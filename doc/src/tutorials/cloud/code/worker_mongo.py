import subprocess
import os
import shutil
import rq

import pymongo
import gridfs

import utils


MODEL_DIR = "/tmp/model/"

def build_model(mongo_host, source_id):
    client = pymongo.MongoClient(mongo_host)
    gfs = gridfs.GridFS(client.opp)

    source_zip = gfs.get(source_id).read()

    # housekeeping
    shutil.rmtree(MODEL_DIR, ignore_errors=True)
    os.makedirs(MODEL_DIR)
    os.chdir(MODEL_DIR)

    # unzip source_zip
    utils.unzip_bytes(source_zip)

    # run make, first clean just to be sure
    subprocess.call(["make", "clean"])
    subprocess.call(["make", "MODE=release"])

    # zip built model
    binary_zip = utils.zip_directory(".")

    # cleaning up
    shutil.rmtree(MODEL_DIR, ignore_errors=True)

    job_id = rq.get_current_job().get_id()

    gfs.put(binary_zip, _id=job_id)

def run_simulation(mongo_host, binary_id, executable, arguments):
    client = pymongo.MongoClient(mongo_host)
    gfs = gridfs.GridFS(client.opp)

    binary_zip = gfs.get(binary_id).read()

    # housekeeping
    shutil.rmtree(MODEL_DIR, ignore_errors=True)
    os.makedirs(MODEL_DIR)
    os.chdir(MODEL_DIR)

    # unzip binary_zip
    utils.unzip_bytes(binary_zip)

    subprocess.call(["chmod", "+x", executable]) # eh

    # execute binary with args
    subprocess.call([executable] + arguments)

    # zip results
    results_zip = utils.zip_directory("results")

    # cleaning up
    shutil.rmtree(MODEL_DIR, ignore_errors=True)

    job_id = rq.get_current_job().get_id()

    gfs.put(results_zip, _id=job_id)
