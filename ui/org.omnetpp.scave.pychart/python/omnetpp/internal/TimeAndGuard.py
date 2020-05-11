"""
A utility decorator to measure the execution time of, and catch and report
any exceptions raised by, any method.
And another (meta)decorator to apply any decorator to all methods of a class.
"""

__copyright__ = "Copyright 2016-2020, OpenSim Ltd"
__license__ = """
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
"""

import time
import traceback

import functools
print = functools.partial(print, flush=True)


class TimeAndGuard:
    def __init__(self, measureTime=False):
        self.measureTime = measureTime

    def __call__(self, function):
        def wrapper(*args, **kwargs):
            try:
                start = time.perf_counter()
                res = function(*args, **kwargs)
                end = time.perf_counter()
                if self.measureTime:
                    print(function.__name__ + " took " + str((end - start) * 1000.0) + " ms in Python.")
                return res
            except Exception as e:
                print("!!!!\nException in " + function.__name__ + ": " + str(e) + "\n")
                print(traceback.format_exc())
                print("!!!!\n")
        return wrapper

# use on a class for example: @TimeAndGuard(measureTime=False)

def for_all_methods(decorator):
    def decorate(cls):
        for attr in cls.__dict__:  # there's propably a better way to do this
            if callable(getattr(cls, attr)):
                setattr(cls, attr, decorator(getattr(cls, attr)))
        return cls
    return decorate

# Use on a class for example: @for_all_methods(TimeAndGuard()))
