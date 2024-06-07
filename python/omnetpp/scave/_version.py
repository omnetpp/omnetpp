import os
from pkg_resources import parse_requirements, DistributionNotFound, VersionConflict, working_set

def check_dependencies():
    """
    Checks if the provided Python dependencies are fulfilled.
    If the check passes, return 0. Otherwise, print an error and return 1.
    """
    requirements_file_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../requirements.txt")

    dependencies = [str(req) for req in parse_requirements(open(requirements_file_name, 'r').read()) if req.name is not None]

    try:
        working_set.require(dependencies)
    except VersionConflict as e:
        print(f"'{e.dist}' was found, but '{e.req}' is required.\n")
        return 1
    except DistributionNotFound as e:
        print(f"'{e.req}' was not found.\n")
        return 1

    return 0

__version__ = "6.0.2"

