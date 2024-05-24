"""
Experimental NED linter.
"""

import os
import re
import sys
import argparse
import omnetpp.ned as ned
from omnetpp.ned import NedResources

def _check_properties(properties, role, is_inet=True):
    allowed = (
        ["isNetwork", "class", "display", "signal", "statistic", "figure", "labels", "sourcecode"] if role == "type" else
        ["unit", "enum", "mutable", "prompt"] if role == "parameter" else
        ["directIn", "loose", "labels"] if role == "gate" else
        ["display", "dynamic"] if role == "submodule" else
        [] if role == "submodule-parameter" else
        [] if role == "submodule-gate" else
        ["display", "reconnect"] if role == "connection" else
        [] if role == "connection-parameter" else
        []
    )
    if is_inet:
        # add properties used in the INET Framework
        allowed += (
            ["networkNode", "networkInterface", "lifecycleSupport", "wireJunction", "application",
             "selfMessageKinds", "group", "omittedTypename", "defaultStatistic"] if role == "type" else
            [] if role == "parameter" else
            ["messageKinds"] if role == "gate" else
            [] if role == "submodule" else
            [] if role == "submodule-parameter" else
            [] if role == "submodule-gate" else
            [] if role == "connection" else
            [] if role == "connection-parameter" else
            []
        )

    for prop in properties:
        assert isinstance(prop, ned.Property)
        if prop.name not in allowed:
            print(f"Warning: unexpected property @{prop.name} in a {role} at {prop.src_loc}")

def check_type_properties(type):
    _check_properties(type.get_local_properties(), "type")

    for p in type.get_local_parameters().values():
        _check_properties(p.get_local_properties(), "parameter")

    if type.is_interface:
        if type.is_module:
            for g in type.get_local_gates().values():
                _check_properties(g.get_local_properties(), "gate")
    else:
        if type.is_module:
            for g in type.get_local_gates().values():
                _check_properties(g.get_local_properties(), "gate")

            for s in type.get_local_submodules():
                _check_properties(s.get_local_properties(), "submodule")
                for p in s.get_local_parameter_assignments().values():
                    _check_properties(p.get_local_properties(), "submodule-parameter")
                for g in s.get_gate_sizes().values():
                    _check_properties(g.get_local_properties(), "submodule-gate")

            for c in type.get_local_connections():
                _check_properties(c.get_local_properties(), "connection")
                for p in c.get_local_parameter_assignments().values():
                    _check_properties(p.get_local_properties(), "connection-parameter")

def _check_name(name, obj, capitalized=False):
    messages = []
    messages = []
    if capitalized and re.match(r"[a-z]", name):
        messages.append(f"should start with an uppercase letter")
    if not capitalized and re.match(r"[A-Z]", name):
        messages.append(f"should start with a lowercase letter")
    if name[0] == '_':
        messages.append(f"starts with underscore")
    elif '_' in name:
        messages.append(f"contains underscore")
    if len(name) > 15 and re.match(r"[^A-Z]+$", name):
        messages.append(f"long name in all-lowercase, perhaps it should be camelized")
    if re.search(r"[A-Z]{3,}", name):
        messages.append(f"possibly contains an acronym in all-uppercase")
    if messages:
        print(f"Warning: name '{name}' {', '.join(messages)} at {obj.src_loc}")

def check_naming(t):
    _check_name(t.name, t, capitalized=True)
    for p in t.get_local_parameters().values():
        if p.type:
            _check_name(p.name, p)
    if t.is_module:
        for g in t.get_local_gates().values():
            _check_name(g.name, g)
        if not t.is_interface:
            for s in t.get_local_submodules():
                _check_name(s.name, s)
            for c in t.get_local_connections():
                if c.name:
                    _check_name(c.name, c)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--check-properties", action="store_true", help="Check properties")
    parser.add_argument("--check-naming", action="store_true", help="Check naming")
    parser.add_argument("--dump-types", action="store_true", help="Dump info on loaded NED types")
    parser.add_argument("dirs", nargs="*", metavar="ned-folder", help="NED source folders to load. Also accepts semicolon-separated folder names so that $OMNETPP_NED_PATH may be specified.")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose mode")
    args = parser.parse_args()

    if not args.dirs:
        print("Error: No NED source folders given; specify one or more folders or $OMNETPP_NED_PATH", file=sys.stderr)
        sys.exit(1)

    # convert semicolon-separated strings into lists
    dirs = [d for dir_list in [dirs.split(";") for dirs in args.dirs] for d in dir_list if d != ""]

    if args.verbose:
        print(f"Loading NED from {dirs}")
    nedRes = NedResources()
    nedRes.load_ned(dirs)

    if args.dump_types:
        nedRes.dump()

    if args.check_properties:
        for t in nedRes.get_types():
            check_type_properties(t)

    if args.check_naming:
        for t in nedRes.get_types():
            check_naming(t)

    if not (args.check_naming or args.check_properties or args.dump_types):
        print("Warning: No action specified, try --help", file=sys.stderr)
