import os
from omnetpp.scave.analysis import Workspace

work_dir = "work"

def make_dotproject_content(name, referenced_projects, broken):
    projects = "".join(["\n            <project>" + p + "</project>" for p in referenced_projects])
    return f"""<?xml version="1.0" encoding="UTF-8"?>
<projectDescription>
        <name>{name}</name>
        <comment></comment>
        <projects>{projects}
        </projects>
</projectDescription{"" if broken else ">"}
    """

def make_project(path, name=None, referenced_projects=[], make_projdesc=True, broken_projdesc=False):
    # create dir
    global work_dir
    dir = os.path.join(work_dir, path)
    os.makedirs(dir, exist_ok=True)

    # create .project
    if not name:
        name = os.path.basename(path)
    if make_projdesc:
        with open(os.path.join(dir, ".project"), "w") as f:
            f.write(make_dotproject_content(name, referenced_projects, broken_projdesc))

    # create example file
    with open(os.path.join(dir, "data.txt"), "w") as f:
        f.write(f"Sample file in project {name} in {dir}\n")

def make_workspace_dir(path, project_locations=dict(), make_metadata=True, broken_locationfile=False):
    # create dir
    global work_dir
    workspace_dir = os.path.join(work_dir, path)
    os.makedirs(workspace_dir, exist_ok=True)

    if project_locations and not make_metadata:
        raise RuntimeError(f"Cannot write .location files without creating .metadata")

    # create mockup metadata with appropriate content
    if make_metadata:
        metadata_dir = os.path.join(workspace_dir, ".metadata")
        os.mkdir(metadata_dir)
        for project_name, project_path in project_locations.items():
            project_location = os.path.join(work_dir, project_path)
            location_dir = metadata_dir + "/.plugins/org.eclipse.core.resources/.projects/" + project_name
            os.makedirs(location_dir)
            location_filename = location_dir + "/.location"
            location_content_mockup = "URI//fura:" if broken_locationfile else "@@@@@@@@@@@@URI//file:" + project_location + "\0@@@@@@@@@@"

            with open(location_filename, "w") as f:
                f.write(location_content_mockup)

def access_workspace(anf_path, workspace_path, project_paths=None):
    def read_sample_file(workspace, project):
        file_location = workspace.to_filesystem_path("/" + project + "/data.txt")
        with open(file_location, "r") as f:
            return f.read().strip()

    global work_dir
    workspace_dir = os.path.join(work_dir, workspace_path) if workspace_path is not None else None
    workspace = Workspace(workspace_dir, project_paths)
    anf_dir = os.path.join(work_dir, anf_path)
    project = workspace.find_enclosing_project(anf_dir)
    print(project + ": \"" + read_sample_file(workspace, project) + "\"")

    ref_projects = workspace.get_all_referenced_projects(project, False)
    for ref_project in ref_projects:
        print(" - " + ref_project + ": \"" + read_sample_file(workspace, ref_project) + "\"")

def cleanup():
    import shutil
    global work_dir
    if not work_dir or os.path.isabs(work_dir) or "." in work_dir:
        raise RuntimeError(f"Cowardly refusing to delete work dir of suspicious location '{work_dir}'")
    shutil.rmtree(work_dir, ignore_errors=True)

class TestCase:
    def __init__(self, test_name):
        print()
        print(f"*** {test_name}")

    def __enter__(self):
        cleanup()

    def __exit__(self, exc_type, exc_val, traceback):
        if exc_type is not None:
            print(exc_type.__name__, ":", exc_val)
        return True

######################################

with TestCase("single project in workspace"):
    make_project(path="projs/foo")
    make_workspace_dir(path="projs")
    access_workspace(anf_path="projs/foo/a/b", workspace_path="projs")

with TestCase("single project, no workspace given"):
    make_project(path="projs/foo")
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("single project, missing .metadata"):
    make_project(path="projs/foo")
    access_workspace(anf_path="projs/foo/a/b", workspace_path="projs")

with TestCase("single project, invalid workspace"):
    make_project(path="projs/foo")
    access_workspace(anf_path="projs/foo/a/b", workspace_path="nonexistent")

with TestCase("single project, empty .metadata"):
    make_project(path="projs/foo")
    make_workspace_dir(path="projs", make_metadata=False)
    access_workspace(anf_path="projs/foo/a/b", workspace_path="projs")

with TestCase("single renamed project in workspace"):
    make_project(path="projs/foo", name="Foo")
    make_workspace_dir(path="projs")
    access_workspace(anf_path="projs/foo/a/b", workspace_path="projs")

with TestCase("broken .project"):
    make_project(path="projs/foo", broken_projdesc=True)
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("broken .location"):
    make_project(path="projs/foo", referenced_projects=["Bar"])
    make_project(path="projs/bar", name="Bar")
    make_workspace_dir(path="projs", project_locations={"Bar": "projs/bar"}, broken_locationfile=True)
    access_workspace(anf_path="projs/foo/a/b", workspace_path="projs")

with TestCase("referenced project"):
    make_project(path="projs/foo", referenced_projects=["bar"])
    make_project(path="projs/bar")
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("multiple referenced projects"):
    make_project(path="projs/foo", referenced_projects=["bar", "baz"])
    make_project(path="projs/bar")
    make_project(path="projs/baz")
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("missing referenced project"):
    make_project(path="projs/foo", referenced_projects=["bar"])
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("indirectly referenced project"):
    make_project(path="projs/foo", referenced_projects=["bar"])
    make_project(path="projs/bar", referenced_projects=["baz"])
    make_project(path="projs/baz")
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("renamed project"):
    make_project(path="projs/foo", name="Foo")
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("renamed referenced project"):
    make_project(path="projs/foo", referenced_projects=["Bar"])
    make_project(path="projs/bar", name="Bar")
    access_workspace(anf_path="projs/foo/a/b", workspace_path=None)

with TestCase("renamed referenced project with workspace"):
    make_workspace_dir("projs", project_locations={"Bar": "projs/bar"})
    make_project(path="projs/foo", referenced_projects=["Bar"])
    make_project(path="projs/bar", name="Bar")
    access_workspace(anf_path="projs/foo/a/b", workspace_path="projs")

with TestCase("moved project"):
    make_project(path="projs/foo", name="Foo")
    make_workspace_dir("ws")
    access_workspace(anf_path="projs/foo/a/b", workspace_path="ws")

with TestCase("moved referenced project not found"):
    make_workspace_dir("ws")
    make_project(path="ws/foo", referenced_projects=["Bar"])
    make_project(path="projs/bar", name="Bar")
    access_workspace(anf_path="ws/foo/a/b", workspace_path="ws")

with TestCase("moved referenced project found"):
    make_workspace_dir("ws", project_locations={"Bar": "projs/bar"})
    make_project(path="ws/foo", referenced_projects=["Bar"])
    make_project(path="projs/bar", name="Bar")
    access_workspace(anf_path="ws/foo/a/b", workspace_path="ws")

with TestCase("finding workspace"):
    make_workspace_dir("ws")
    project_dir = work_dir + "/ws/proj1/folder"
    os.makedirs(project_dir)
    print(Workspace.find_workspace(project_dir))