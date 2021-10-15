import sys

message = """Warning: The omnetpp.scave.plot module was renamed to omnetpp.scave.ideplot.
Please change the import statement in the chart script."""

print(message, file=sys.stderr)

# falling back to the new name for now
from .ideplot import *