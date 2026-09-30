import os

# Try to use modern packaging modules first
try:
    from importlib.metadata import requires, distribution, PackageNotFoundError
    from packaging.requirements import Requirement
    from packaging.version import Version, InvalidVersion
    USE_MODERN_PACKAGES = True
    USE_LEGACY_PACKAGES = False
except ImportError:
    # Fall back to deprecated pkg_resources if packaging is not available
    USE_MODERN_PACKAGES = False
    try:
        from pkg_resources import parse_requirements, DistributionNotFound, VersionConflict, working_set
        USE_LEGACY_PACKAGES = True
    except ImportError:
        # Neither packaging nor pkg_resources is available
        USE_LEGACY_PACKAGES = False


def check_dependencies():
    """
    Checks if the provided Python dependencies are fulfilled.
    If the check passes, return 0. Otherwise, print an error and return 1.
    If neither packaging nor pkg_resources is available, returns 0 (assume success).
    """
    requirements_file_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../requirements.txt")

    if USE_MODERN_PACKAGES:
        # Modern implementation using importlib.metadata and packaging
        with open(requirements_file_name, 'r') as f:
            req_text = f.read()

        dependencies = []
        for line in req_text.splitlines():
            line = line.strip()
            if line and not line.startswith('#'):
                try:
                    dependencies.append(Requirement(line))
                except InvalidVersion:
                    pass

        for req in dependencies:
            if req.marker and not req.marker.evaluate():
                continue

            try:
                dist = distribution(req.name)
                # Check if installed version satisfies the requirement
                if req.specifier and Version(dist.version) not in req.specifier:
                    print(f"'{req.name}=={dist.version}' was found, but '{req}' is required.\n")
                    return 1
            except PackageNotFoundError:
                print(f"'{req}' was not found.\n")
                return 1
    elif USE_LEGACY_PACKAGES:
        # Legacy implementation using pkg_resources
        dependencies = [str(req) for req in parse_requirements(open(requirements_file_name, 'r').read()) if req.name is not None]

        try:
            working_set.require(*dependencies)
        except VersionConflict as e:
            print(f"'{e.dist}' was found, but '{e.req}' is required.\n")
            return 1
        except DistributionNotFound as e:
            print(f"'{e.req}' was not found.\n")
            return 1

    # If neither packaging nor pkg_resources were available to properly test the dependencies,
    # we just assume success and hope for the best
    return 0

__version__ = "6.5.0"
