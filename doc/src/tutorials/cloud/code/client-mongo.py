import time
import argparse
import subprocess
import re

from rq import Queue
from redis import Redis

import utils
import worker_mongo

import pymongo
import gridfs

def get_runs_from_filter(configuration, runfilter):
    runs = []

    output = subprocess.check_output(["opp_run", "-q", "runnumbers",
                                      "-c", configuration, "-r", runfilter]).decode("utf-8")
    match = re.search(r"Run numbers: ([\d ]+)", output)
    runs = match.group(1).split()

    return runs


parser = argparse.ArgumentParser()

parser.add_argument('executable', type=str, help='the Simulation program')
parser.add_argument('-c', metavar='configuration', dest='configuration',
                    type=str, required=True, help='the Configuration to run')
parser.add_argument('-r', metavar='runfilter', dest='runfilter', type=str,
                    required=False, default='', help='the Run Filter selecting the runs')
parser.add_argument('--redis-host', metavar='addr', dest='redis_host',
                    type=str, required=False, default="localhost",
                    help="""the address of the Redis server to use
                            (default: localhost)""")
parser.add_argument('--mongo-host', metavar='addr', dest='mongo_host',
                    type=str, required=False, default="localhost",
                    help="""the address of the MongoDB server to use
                            (default: localhost)""")

args = parser.parse_args()


print("Connecting to Redis at '" + args.redis_host + "'...")

redis_conn = Redis(host=args.redis_host) # Tell RQ what Redis connection to use
q = Queue(connection=redis_conn)  # no args implies the default queue

gfs = gridfs.GridFS(pymongo.MongoClient(args.mongo_host).opp)

runs = get_runs_from_filter(args.configuration, args.runfilter)
print("Matched runs: " + ", ".join(runs))

model_source_zip = utils.zip_directory(".", exclude_dirs=["results", "frames", "out"])
print("Size of sources: " + str(len(model_source_zip)) + "B")

try:
    gfs.delete("model_source") # eh
except:
    pass

gfs.put(model_source_zip, _id="model_source")

build_job = q.enqueue(worker_mongo.build_model, args.mongo_host, "model_source")

jobs = []

print("Enqueueing " + str(len(runs)) + " jobs...")
for r in runs:
    j = q.enqueue(worker_mongo.run_simulation, args.mongo_host, build_job.get_id(),
                  args.executable, ["-c", args.configuration, "-r", r], depends_on=build_job)
    j.meta['runnumber'] = r
    j.save_meta()
    jobs.append(j)

print("Waiting for results...")

while jobs:
    for j in jobs[:]:
        if j.is_finished:
            print("Job for Run #" + j.meta["runnumber"] + " finished!")
            result = gfs.get(j.get_id()).read()
            #print("Size of results: " + str(len(result)))
            utils.unzip_bytes(result)
            jobs.remove(j)

    time.sleep(0.1)

print("All done, bye!")
