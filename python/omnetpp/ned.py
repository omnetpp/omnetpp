"""
Experimental code for parsing NED files, and querying their content.
Allows for accessing the NED types they contain, and exploring their relationships.

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
import omnetpp.nedast as nedast

class NedFile:
    def __init__(self, ned_resources, ast_node):
        self.ned_resources = ned_resources
        self.ast_node = ast_node
        self.file_name = ast_node.filename
        package_element = ast_node.find('package')
        self.package_name = package_element.name if package_element is not None else None
        self.imports = [import_element.import_spec for import_element in ast_node.findall('import')]
        self.comment = "\n".join([comment_element.content for comment_element in ast_node.findall('comment')])
        self.types = _collect_child_types(self, ast_node)

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

def _collect_child_types(parent, ast_node):
    child_types = []
    for child_element in ast_node:
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

def _collect_properties(parent, ast_node):
    if not ast_node:
        return {}
    nested_dict = {}
    for property_element in ast_node.findall('property'):
        property = Property(parent, property_element)
        if property.name not in nested_dict:
            nested_dict[property.name] = {}
        nested_dict[property.name][property.index] = property
    return nested_dict

class NedPart:
    def __init__(self, parent, ast_node):
        self.ned_resources = parent.ned_resources
        self.parent = parent
        self.ast_node = ast_node
        self.src_loc = ast_node.src_loc
        self.src_region = ast_node.src_region

class NedType(NedPart):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.keyword = ""
        self.package_name = parent.package_name
        self.name = ast_node.name
        self.qname = self.package_name + "." + self.name if self.package_name else self.name
        self.comment = "\n".join([comment_element.content for comment_element in ast_node.findall('comment')])
        self.ned_resources.ned_types_by_qname[self.qname] = self

    def get_local_parameters(self):
        return {}

    def get_local_parameter_definitions(self):
        return {}

    def get_all_parameter_definitions(self):
        return {}

    def get_local_parameter_assignments(self):
        return {}

    def get_all_parameter_assignments(self):
        return {}

    def get_local_gates(self):
        return {}

    def get_local_gate_definitions(self):
        return {}

    def get_all_gate_definitions(self):
        return {}

    def get_local_submodules(self):
        return []

    def get_all_submodules(self):
        return []

    def get_local_connections(self):
        return []

    def get_all_connections(self):
        return []

    def resolve(self, simple_name):
        return self.parent.resolve(simple_name)

    def __str__(self):
        return self.qname

    def __repr__(self):
        return self.keyword + " " + self.qname

class Component(NedType):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.is_interface = False
        extends_element = ast_node.find('extends')
        self.extends_name = extends_element.name if extends_element is not None else None
        self.interface_names = [interface_element.name for interface_element in ast_node.findall('interface-name')]
        parameters_element = ast_node.find('parameters')
        self.parameters_map = {param.name : Parameter(self, param) for param in parameters_element.findall('param')} if parameters_element is not None else {}
        self.properties_map = _collect_properties(self, parameters_element)

    def get_base_type(self):
        return self.resolve(self.extends_name) if self.extends_name else None

    def get_inheritance_chain(self):
        base = self.get_base_type()
        if base is not None:
            return [self] + base.get_inheritance_chain()
        else:
            return [self]

    def get_local_interface_types(self):
        return [self.resolve(interface_name) for interface_name in self.interface_names]

    def get_all_interface_types(self):
        base = self.get_base_type()
        if base is not None:
            return self.get_local_interface_types() + base.get_all_interface_types()
        else:
            return self.get_local_interface_types()

    def get_immediate_subtypes(self):
        return [t for t in self.ned_resources.get_types() if not t.is_interface and t.get_base_type() == self]

    def get_all_subtypes(self):
        all = list(self.get_immediate_subtypes())
        for subtype in self.get_immediate_subtypes():
            all += subtype.get_all_subtypes()
        return all

    def get_local_parameters(self):
        return dict(self.parameters_map)

    def get_local_parameter_definitions(self):
        return {name : param for name, param in self.parameters_map.items() if param.type}

    def get_all_parameter_definitions(self):
        all = self.get_local_parameter_definitions()
        base = self.get_base_type()
        if base is not None:
            all.update(base.get_all_parameter_definitions())
        return all

    def get_local_parameter_assignments(self):
        return {name : param for name, param in self.parameters_map.items() if param.value}

    def get_all_parameter_assignments(self):
        all = self.get_local_parameter_assignments()
        base = self.get_base_type()
        if base is not None:
            all.update(base.get_all_parameter_assignments())
        return all

    def get_local_properties(self, name=None):
        if name is None:
            return [item for index_map in self.properties_map.values() for item in index_map.values()] # flatten nested dict into list of properties
        else:
            return self.properties_map.get(name, {}).values()

    def get_local_property(self, name, index=None):
        return self.properties_map.get(name, {}).get(index)

    def get_implementation_class_name(self):
        # first, look at local @class property
        class_property = self.get_local_property("class")
        if class_property is not None:
            class_name = class_property.get_single_value()
            if not class_name:
                raise ValueError(f"Property @class in {self.name} has no value")
            return class_name

        # if there is no @class, C++ class name is inherited from base type
        base = self.get_base_type()
        if base is not None:
            return base.get_implementation_class_name()

        # if there is no base type, use the NED type name as C++ class name
        return self.name if self.keyword == "simple" else "omnetpp::cModule" if self.keyword == "module" else "omnetpp::cChannel"

class Module(Component):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.is_module = True
        inner_types_element = ast_node.find("types")
        self.types = _collect_child_types(self, inner_types_element) if inner_types_element else []
        gates_element = ast_node.find('gates')
        self.gates_map = {gate.name : Gate(self, gate) for gate in gates_element.findall('gate')} if gates_element is not None else {}

    def get_inner_types(self):
        return list(self.types)

    def get_local_gates(self):
        return dict(self.gates_map)

    def get_local_gate_definitions(self):
        return {name : gate for name, gate in self.gates_map.items() if gate.type}

    def get_all_gate_definitions(self):
        all = self.get_local_gate_definitions()
        base = self.get_base_type()
        if base is not None:
            all.update(base.get_all_gate_definitions())
        return all

class SimpleModule(Module):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.keyword = "simple"

class CompoundModule(Module):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.keyword = "module"
        submodules_element = ast_node.find('submodules')
        self.submodules = [Submodule(self, submodule_element) for submodule_element in submodules_element.findall('submodule')] if submodules_element else []
        connections_element = ast_node.find('connections')
        # note: search for ".//connection", because connections may occur inside <connection-group> elements, too
        self.connections = [Connection(self, connection_element) for connection_element in connections_element.findall('.//connection')] if connections_element else []

    def get_local_submodules(self):
        return list(self.submodules)

    def get_all_submodules(self):
        if self.get_base_type():
            return self.get_local_submodules() + self.get_base_type().get_all_submodules()
        else:
            return self.get_local_submodules()

    def get_local_connections(self):
        return list(self.connections)

    def get_all_connections(self):
        if self.get_base_type():
            return self.get_local_connections() + self.get_base_type().get_all_connections()
        else:
            return self.get_local_connections()

    def get_types_used_in_all_submodules(self):
        types = [s.get_type() for s in self.get_all_submodules() if not s.is_parametric_type]
        return list(set(types))

    def get_interface_types_used_in_all_submodules(self):
        types = [s.get_interface_type() for s in self.get_all_submodules() if s.is_parametric_type]
        return list(set(types))

class Channel(Component):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.is_module = False
        self.keyword = "channel"

class ComponentInterface(NedType):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.is_interface = True
        self.extends_names = [extends_element.name for extends_element in ast_node.findall('extends')]
        parameters_element = ast_node.find('parameters')
        self.parameters_map = {param.name : Parameter(self, param) for param in parameters_element.findall('param')} if parameters_element is not None else {}
        self.properties_map = _collect_properties(self, parameters_element)

    def get_base_types(self):
        return [self.resolve(extends_name) for extends_name in self.extends_names]

    def get_all_base_types(self):
        all = list(self.get_base_types())
        for base_type in self.get_base_types():
            all += base_type.get_all_base_types()
        return all

    def get_immediate_subtypes(self):
        return [t for t in self.ned_resources.get_types() if t.is_interface and self in t.get_base_types()]

    def get_all_subtypes(self):
        all = list(self.get_immediate_subtypes())
        for subtype in self.get_immediate_subtypes():
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

    def get_local_parameters(self):
        return dict(self.parameters_map)

    def get_local_parameter_definitions(self):
        return {name : param for name, param in self.parameters_map.items() if param.type}

    def get_all_parameter_definitions(self):
        all = self.get_local_parameter_definitions()
        for base in self.get_base_types():
            all.update(base.get_all_parameter_definitions())
        return all

    def get_local_parameter_assignments(self):
        return {name : param for name, param in self.parameters_map.items() if param.value}

    def get_all_parameter_assignments(self):
        all = self.get_local_parameter_assignments()
        for base in self.get_base_types():
            all.update(base.get_all_parameter_assignments())
        return all

    def get_local_properties(self, name=None):
        if name is None:
            return [item for index_map in self.properties_map.values() for item in index_map.values()] # flatten nested dict into list of properties
        else:
            return self.properties_map.get(name, {}).values()

    def get_local_property(self, name, index=None):
        return self.properties_map.get(name, {}).get(index)

class ModuleInterface(ComponentInterface):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.is_module = True
        self.keyword = "moduleinterface"
        gates_element = ast_node.find('gates')
        self.gates_map = {gate.name : Gate(self, gate) for gate in gates_element.findall('gate')} if gates_element is not None else {}

    def get_local_gates(self):
        return dict(self.gates_map)

    def get_local_gate_definitions(self):
        return {name : gate for name, gate in self.gates_map.items() if gate.type}

    def get_all_gate_definitions(self):
        all = self.get_local_gate_definitions()
        for base in self.get_base_types():
            all.update(base.get_all_gate_definitions())
        return all

class ChannelInterface(ComponentInterface):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.is_module = False
        self.keyword = "channelinterface"

class Subcomponent(NedPart):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.name = ast_node.name
        self.type_name = ast_node.type
        self.vector_size = ast_node.vector_size
        self.interface_type_name = ast_node.like_type
        self.like_expr = ast_node.like_expr
        self.is_parametric_type = bool(self.interface_type_name)
        self.is_default = ast_node.is_default
        parameters_element = ast_node.find('parameters')
        self.parameters_map = {param.name : Parameter(self, param) for param in parameters_element.findall('param')} if parameters_element is not None else {}
        self.properties_map = _collect_properties(self, parameters_element)
        self.comment = "\n".join([comment_element.content for comment_element in ast_node.findall('comment')])

    def get_type(self):
        if self.is_parametric_type:
            raise ValueError(f"Submodule/connection {self.name} does not have a fixed type, it is of parametric type")
        return self.parent.resolve(self.type_name)

    def get_interface_type(self):
        if not self.is_parametric_type:
            raise ValueError(f"Submodule/connection {self.name} is not a parametric type")
        return self.parent.resolve(self.interface_type_name)

    def resolve_like_type(self, simple_name):
        interface_type = self.get_interface_type()
        return interface_type.resolve_like_type(simple_name)

    def get_local_parameter_assignments(self):
        return dict(self.parameters_map)  # note: they must all be assignments

    def get_local_properties(self, name=None):
        if name is None:
            return [item for index_map in self.properties_map.values() for item in index_map.values()] # flatten nested dict into list of properties
        else:
            return self.properties_map.get(name, {}).values()

    def get_local_property(self, name, index=None):
        return self.properties_map.get(name, {}).get(index)

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name

class Submodule(Subcomponent):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        gates_element = ast_node.find('gates')
        self.gates_map = {gate.name : Gate(self, gate) for gate in gates_element.findall('gate')} if gates_element is not None else {}

    def get_gate_sizes(self):
        return dict(self.gates_map)

class Connection(Subcomponent):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)

class Parameter(NedPart):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.type = ast_node.type
        self.name = ast_node.name
        self.value = ast_node.value
        self.is_volatile = ast_node.is_volatile
        self.is_pattern = ast_node.is_pattern
        self.is_default = ast_node.is_default
        self.properties_map = _collect_properties(self, ast_node)
        self.comment = "\n".join([comment_element.content for comment_element in ast_node.findall('comment')])

    def get_local_properties(self, name=None):
        if name is None:
            return [item for index_map in self.properties_map.values() for item in index_map.values()] # flatten nested dict into list of properties
        else:
            return self.properties_map.get(name, {}).values()

    def get_local_property(self, name, index=None):
        return self.properties_map.get(name, {}).get(index)

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name

class Gate(NedPart):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.type = ast_node.type
        self.name = ast_node.name
        self.is_vector = ast_node.is_vector
        self.vector_size = ast_node.vector_size
        self.properties_map = _collect_properties(self, ast_node)
        self.comment = "\n".join([comment_element.content for comment_element in ast_node.findall('comment')])

    def get_local_properties(self, name=None):
        if name is None:
            return [item for index_map in self.properties_map.values() for item in index_map.values()] # flatten nested dict into list of properties
        else:
            return self.properties_map.get(name, {}).values()

    def get_local_property(self, name, index=None):
        return self.properties_map.get(name, {}).get(index)

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name

class Property(NedPart):
    def __init__(self, parent, ast_node):
        super().__init__(parent, ast_node)
        self.name = ast_node.name
        self.index = ast_node.index
        self.is_implicit = ast_node.is_implicit
        self.comment = "\n".join([comment_element.content for comment_element in ast_node.findall('comment')])
        self.keys_map = {
            prop_key.name or "" : [literal.value for literal in prop_key.findall('literal')]
            for prop_key in ast_node.findall('property-key')
        }

    def get_single_value(self, default=None):
        if not self.keys_map:
            return default
        if list(self.keys_map.keys()) != [""]:
            raise ValueError(f"Property {self.name} has multiple keys")
        values_list = self.keys_map.get("")
        if len(values_list) > 1:
            raise ValueError(f"Property {self.name} has multiple values")
        return values_list[0] if values_list else default

    def __str__(self):
        return f"@{self.name}" if self.index is None else f"@{self.name}[{self.index}]"

    def __repr__(self):
        return f"@{self.name}" if self.index is None else f"@{self.name}[{self.index}]"

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
            try:
                subprocess.run(["opp_run", "-s", "-h", "neddecls"], stdout=builtin_decls_file, check=True)
            except:
                subprocess.run(["opp_run_dbg", "-s", "-h", "neddecls"], stdout=builtin_decls_file, check=True)
            self.load_ned_file(builtin_decls_file.name)

    def load_ned_file(self, file_name):
        self.load_ned([file_name])

    def load_ned_folder(self, dir):
        self.load_ned([dir])

    def load_ned(self, file_and_folder_paths):
        with tempfile.NamedTemporaryFile(suffix=".xml") as temp_file:
            subprocess.run(["opp_nedtool", "convert", "--xml", "--srcloc", "--storesrc", "--merge", "-o", temp_file.name] + file_and_folder_paths, check=True)
            tree = ET.parse(temp_file.name)

        root = tree.getroot()

        def dom_to_nedelement_tree(element, parent=None):
            node = nedast.nedelement_from_tag(element.tag, parent)
            node.set_fields(**element.attrib)
            node.children = [dom_to_nedelement_tree(child, parent=node) for child in element]
            return node

        ast_root = dom_to_nedelement_tree(root)

        for ned_file_node in ast_root.findall('ned-file'):
            self.ned_files[ned_file_node.filename] = NedFile(self, ned_file_node)

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
                print("  subtypes =", type.get_immediate_subtypes())
                print("  all subtypes =", type.get_all_subtypes())
                print("  all implementors =", type.get_all_implementors())
            else:
                print("  base type =", type.get_base_type())
                print("  c++ class =", type.get_implementation_class_name())
                print("  inheritance chain =", type.get_inheritance_chain())
                print("  interface types =", type.get_local_interface_types())
                print("  all interface types =", type.get_all_interface_types())
                print("  subtypes =", type.get_immediate_subtypes())
                print("  all subtypes =", type.get_all_subtypes())
                print("  parameter definitions =", type.get_local_parameter_definitions().keys())
                print("  all parameter definitions =", type.get_all_parameter_definitions().keys())
                if type.is_module:
                    print("  inner types =", type.get_inner_types())
                    if type.keyword == "module":
                        print("  types used in submodules =", type.get_types_used_in_all_submodules())
                        print("  interface types used in submodules =", type.get_interface_types_used_in_all_submodules())
