"""
This module allows parsing NED files, browsing the types they contain,
and exploring their relationships.

Usage:

    from omnetpp.ned import NedResources
    ned = NedResources()
    ned.load_ned_folder('inet/src')
    ned.dump()
    example_type = ned.get_type("inet.transportlayer.tcp.Tcp")
    ned.dump([example_type])
"""

import os
import subprocess
import tempfile
import xml.etree.ElementTree as ET

class NedFile:
    def __init__(self, ned_resources, tree):
        self.ned_resources = ned_resources
        self.tree = tree
        self.file_name = tree.get('filename')
        package_element = tree.find('package')
        self.package_name = package_element.get('name') if package_element is not None else None
        self.imports = [import_element.get('import-spec') for import_element in tree.findall('import')]
        self.comment = "\n".join([comment_element.get('content') for comment_element in tree.findall('comment')])
        self.types = _collect_child_types(self, tree)

    def resolve(self, simple_name):
        """Resolves simple name into type object based on imports"""
        for i in self.imports:
            if i.endswith("." + simple_name):
                if not self.ned_resources.has_type(i):
                    raise ValueError(f"Imported type {i} in {self.file_name} does not exist")
                return self.ned_resources.get_type(i)
        if self.ned_resources.has_type(simple_name):
            return self.ned_resources.get_type(simple_name)
        if self.package_name and self.ned_resources.has_type(self.package_name + "." + simple_name):
            return self.ned_resources.get_type(self.package_name + "." + simple_name)
        raise ValueError(f"Could not resolve {simple_name} in {self.file_name}")

    def __str__(self):
        return self.file_name

    def __repr__(self):
        return self.file_name

def _collect_child_types(parent, tree):
    child_types = []
    for child_element in tree:
        type = None
        if child_element.tag == 'simple-module':
            type = SimpleModule(parent, child_element)
        elif child_element.tag == 'compound-module':
            type = CompoundModule(parent, child_element)
        elif child_element.tag == 'channel':
            type = Channel(parent, child_element)
        elif child_element.tag == 'module-interface':
            type = ModuleInterface(parent, child_element)
        elif child_element.tag == 'channel-interface':
            type = ChannelInterface(parent, child_element)
        else:
            pass # comment, import, package decl, property, etc.
        if type:
            child_types.append(type)
    return child_types

class NedType:
    def __init__(self, parent, tree):
        self.ned_resources = parent.ned_resources
        self.parent = parent
        self.tree = tree
        self.keyword = ""
        self.package_name = parent.package_name
        self.name = tree.get('name')
        self.qname = self.package_name + "." + self.name if self.package_name else self.name
        self.comment = "\n".join([comment_element.get('content') for comment_element in tree.findall('comment')])
        self.ned_resources.ned_types_by_qname[self.qname] = self

    def resolve(self, simple_name):
        return self.parent.resolve(simple_name)

    def __str__(self):
        return self.qname

    def __repr__(self):
        return self.keyword + " " + self.qname

class Component(NedType):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.is_interface = False
        extends_element = tree.find('extends')
        self.extends_name = extends_element.get('name') if extends_element is not None else None
        self.interface_names = [interface_element.get('name') for interface_element in tree.findall('interface-name')]

    def get_base_type(self):
        return self.resolve(self.extends_name) if self.extends_name else None

    def get_inheritance_chain(self):
        base = self.get_base_type()
        if base is not None:
            return [self] + base.get_inheritance_chain()
        else:
            return [self]

    def get_interface_types(self):
        return [self.resolve(interface_name) for interface_name in self.interface_names]

    def get_all_interface_types(self):
        base = self.get_base_type()
        if base is not None:
            return self.get_interface_types() + base.get_all_interface_types()
        else:
            return self.get_interface_types()

    def get_subtypes(self):
        return [t for t in self.ned_resources.get_types() if not t.is_interface and t.get_base_type() == self]

    def get_all_subtypes(self):
        all = list(self.get_subtypes())
        for subtype in self.get_subtypes():
            all += subtype.get_all_subtypes()
        return all

class Module(Component):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.is_module = True
        inner_types_element = tree.find("types")
        self.types = _collect_child_types(self, inner_types_element) if inner_types_element else []

    def get_inner_types(self):
        return list(self.types)

class SimpleModule(Module):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.keyword = "simple"

    def get_submodules(self):
        return []

    def get_all_submodules(self):
        return []

class CompoundModule(Module):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.keyword = "module"
        submodules_element = tree.find('submodules')
        self.submodules = [Submodule(self, submodule_element) for submodule_element in submodules_element.findall('submodule')] if submodules_element else []

    def get_submodules(self):
        return list(self.submodules)

    def get_all_submodules(self):
        if self.get_base_type():
            return self.get_submodules() + self.get_base_type().get_all_submodules()
        else:
            return self.get_submodules()

    def get_types_used_in_all_submodules(self):
        types = [s.get_type() for s in self.get_all_submodules() if not s.is_parametric_type]
        return list(set(types))

    def get_interface_types_used_in_all_submodules(self):
        types = [s.get_interface_type() for s in self.get_all_submodules() if s.is_parametric_type]
        return list(set(types))

class Channel(Component):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.is_module = False
        self.keyword = "channel"

class ComponentInterface(NedType):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.is_interface = True
        self.extends_names = [extends_element.get('name') for extends_element in tree.findall('extends')]

    def get_base_types(self):
        return [self.resolve(extends_name) for extends_name in self.extends_names]

    def get_all_base_types(self):
        all = list(self.get_base_types())
        for base_type in self.get_base_types():
            all += base_type.get_all_base_types()
        return all

    def get_subtypes(self):
        return [t for t in self.ned_resources.get_types() if t.is_interface and self in t.get_base_types()]

    def get_all_subtypes(self):
        all = list(self.get_subtypes())
        for subtype in self.get_subtypes():
            all += subtype.get_all_subtypes()
        return all

    def get_all_implementors(self):
        return [t for t in self.ned_resources.get_types() if not t.is_interface and self in t.get_all_interface_types()]

    def resolve_like_type(self, simple_name):
        candidates = self.ned_resources.get_types_by_name(simple_name)
        candidates = [t for t in candidates if self in t.get_all_interface_types()]
        if len(candidates) == 1:
            return candidates[0]
        elif len(candidates) == 1:
            raise ValueError(f"Could not resolve {simple_name} that implements {self.qname} -- no candidate found")
        else:
            raise ValueError(f"Could not resolve {simple_name} that implements {self.qname} -- more than one candidate: {candidates}")

class Submodule:
    def __init__(self, parent, tree):
        self.ned_resources = parent.ned_resources
        self.parent = parent
        self.tree = tree
        self.name = tree.get('name')
        self.type_name = tree.get('type')
        self.vector_size = tree.get('vector-size')
        self.interface_type_name = tree.get('like-type')
        self.like_expr = tree.get('like-expr')
        self.is_parametric_type = bool(self.interface_type_name)
        self.is_default = tree.get('is-default')
        self.comment = "\n".join([comment_element.get('content') for comment_element in tree.findall('comment')])

    def get_type(self):
        if self.is_parametric_type:
            raise ValueError(f"Submodule {self.name} does not have a fixed type, it is of parametric type")
        return self.parent.resolve(self.type_name)

    def get_interface_type(self):
        if not self.is_parametric_type:
            raise ValueError(f"Submodule {self.name} is not a parametric type")
        return self.parent.resolve(self.interface_type_name)

    def resolve_like_type(self, simple_name):
        interface_type = self.get_interface_type()
        return interface_type.resolve_like_type(simple_name)

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name

class ModuleInterface(ComponentInterface):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.is_module = True
        self.keyword = "moduleinterface"

class ChannelInterface(ComponentInterface):
    def __init__(self, parent, tree):
        super().__init__(parent, tree)
        self.is_module = False
        self.keyword = "channelinterface"

class NedResources:
    """
    This class is used to hold the parsed NED files. It contains a dictionary of
    NED files indexed by their file names. Each NED file is represented by an
    instance of the appropriate subclass of the `NedType` class, which holds the
    parsed NED data. The class also has a dictionary of NED types indexed by
    their fully qualified names. It provides methods for loading NED files into
    the class, and for resolving NED type names.
    """
    def __init__(self):
        self.ned_files = {}
        self.ned_types_by_qname = {}
        self._load_builtins()

    def _load_builtins(self):
        with tempfile.NamedTemporaryFile(suffix=".ned") as builtin_decls_file:
            subprocess.run(["opp_run", "-s", "-h", "neddecls"], stdout=builtin_decls_file, check=True)
            self.load_ned_file(builtin_decls_file.name)

    def load_ned_file(self, file_name):
        self.load_ned([file_name])

    def load_ned_folder(self, dir):
        self.load_ned([dir])

    def load_ned(self, file_and_folder_paths):
        with tempfile.NamedTemporaryFile(suffix=".xml") as temp_file:
            subprocess.run(["opp_nedtool", "convert", "-x", "-m", "-o", temp_file.name] + file_and_folder_paths, check=True)
            tree = ET.parse(temp_file.name)

        root = tree.getroot()

        for ned_file in root.findall('ned-file'):
            filename = ned_file.get('filename')
            self.ned_files[filename] = NedFile(self, ned_file)

    def get_ned_files(self):
        return self.ned_files.values()

    def get_ned_file(self, filename):
        return self.ned_files[filename]

    def get_types(self):
        return self.ned_types_by_qname.values()

    def has_type(self, qname):
        return qname in self.ned_types_by_qname

    def get_type(self, qname):
        return self.ned_types_by_qname[qname]

    def get_types_by_name(self, name):
        return [type for type in self.get_types() if type.name == name]

    def dump(self, types=None):
        for type in types or self.get_types():
            print()
            print(type.keyword, type.qname)
            print("  " +type.comment.replace("\n","\n  ").strip())
            if type.is_interface:
                print("  base types =", type.get_base_types())
                print("  all base types =", type.get_all_base_types())
                print("  subtypes =", type.get_subtypes())
                print("  all subtypes =", type.get_all_subtypes())
                print("  all implementors =", type.get_all_implementors())
            else:
                print("  base type =", type.get_base_type())
                print("  inheritance chain =", type.get_inheritance_chain())
                print("  interface types =", type.get_interface_types())
                print("  all interface types =", type.get_all_interface_types())
                print("  subtypes =", type.get_subtypes())
                print("  all subtypes =", type.get_all_subtypes())
                if type.is_module:
                    print("  inner types =", type.get_inner_types())
                    if type.keyword == "module":
                        print("  types used in submodules =", type.get_types_used_in_all_submodules())
                        print("  interface types used in submodules =", type.get_interface_types_used_in_all_submodules())

try:
    subprocess.run(["opp_run", "-v"], stdout = subprocess.DEVNULL, check=True)
    subprocess.run(["opp_nedtool", "-h"], stdout = subprocess.DEVNULL, check=True)
except:
    raise RuntimeError("The omnetpp.ned package requires the `opp_run` and `opp_nedtool` programs, both of which are parts of OMNeT++. Please ensure that OMNeT++ is in the path and it is built.")
