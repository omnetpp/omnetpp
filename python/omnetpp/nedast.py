"""
Experimental code for representing parsed NED files.
Classes correspond to elements in doc/etc/ned2.dtd.
"""

import re

class NedElement:
    def __init__(self, tag, parent=None):
            self.tag = tag
            self.parent = parent
            self.children = []
            self.src_loc = None
            self.src_region = None

    def set_fields(self, **attributes):
        if attributes is not None:
            for key, value in attributes.items():
                if key.startswith('is-'):
                    value = value.lower() == 'true'  # convert to boolean
                setattr(self, key.replace('-', '_'), value)

    def find(self, tag):
            for child in self.children:
                if child.tag == tag:
                    return child

    def findall(self, tag):
            return [child for child in self.children if child.tag == tag]

    def __iter__(self):
            return iter(self.children)

    def __repr__(self):
            attributes = {key: value for key, value in self.__dict__.items() if key not in {'tag', 'parent', 'children'}}
            return f"<NedElement(tag='{self.tag}', attributes={attributes}, children={len(self.children)})>"

def nedelement_from_tag(tag, parent=None):
    class_name = re.sub(r'-(\w)', lambda m: m.group(1).upper(), '-' + tag) + 'Node'
    return globals()[class_name](parent=parent)

class FilesNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("files", parent)

class NedFileNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("ned-file", parent)
        self.filename = None
        self.version = None

class CommentNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("comment", parent)
        self.locid = None
        self.content = None

class PackageNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("package", parent)
        self.name = None

class ImportNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("import", parent)
        self.import_spec = None

class PropertyDeclNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("property-decl", parent)
        self.name = None
        self.is_array = False

class ExtendsNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("extends", parent)
        self.name = None

class InterfaceNameNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("interface-name", parent)
        self.name = None

class SimpleModuleNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("simple-module", parent)
        self.name = None

class ModuleInterfaceNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("module-interface", parent)
        self.name = None

class CompoundModuleNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("compound-module", parent)
        self.name = None

class ChannelInterfaceNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("channel-interface", parent)
        self.name = None

class ChannelNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("channel", parent)
        self.name = None

class ParametersNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("parameters", parent)
        self.is_implicit = False

class ParamNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("param", parent)
        self.type = None
        self.is_volatile = False
        self.name = None
        self.value = None
        self.is_pattern = False
        self.is_default = False

class PropertyNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("property", parent)
        self.is_implicit = False
        self.name = None
        self.index = None

class PropertyKeyNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("property-key", parent)
        self.name = None

class GatesNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("gates", parent)

class GateNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("gate", parent)
        self.name = None
        self.type = None
        self.is_vector = False
        self.vector_size = None

class TypesNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("types", parent)

class SubmodulesNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("submodules", parent)

class SubmoduleNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("submodule", parent)
        self.name = None
        self.type = None
        self.like_type = None
        self.like_expr = None
        self.is_default = False
        self.vector_size = None

class ConnectionsNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("connections", parent)
        self.allow_unconnected = None

class ConnectionNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("connection", parent)
        self.src_module = None
        self.src_module_index = None
        self.src_gate = None
        self.src_gate_plusplus = None
        self.src_gate_index = None
        self.src_gate_subg = None
        self.dest_module = None
        self.dest_module_index = None
        self.dest_gate = None
        self.dest_gate_plusplus = None
        self.dest_gate_index = None
        self.dest_gate_subg = None
        self.name = None
        self.type = None
        self.like_type = None
        self.like_expr = None
        self.is_default = False
        self.is_bidirectional = False
        self.is_forward_arrow = False

class ConnectionGroupNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("connection-group", parent)

class LoopNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("loop", parent)
        self.param_name = None
        self.from_value = None
        self.to_value = None

class ConditionNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("condition", parent)
        self.condition = None

class LiteralNode(NedElement):
    def __init__(self, parent=None):
        super().__init__("literal", parent)
        self.type = None
        self.text = None
        self.value = None

