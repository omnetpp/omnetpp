"""
This package makes the OMNeT++ simulation kernel and library classes accessible
from Python.
"""

from omnetpp.runtime.omnetpp import *

__all__ = [k for k,v in locals().items() if k[0] != "_" and v.__class__.__name__ != "module"]
