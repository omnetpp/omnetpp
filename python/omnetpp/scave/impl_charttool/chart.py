

class NoneDict(dict):

    def __getitem__(self, key):
        return dict.get(self, key, None)


properties = NoneDict()
name = ""


def get_configured_properties():
    return properties


def get_configured_property(key):
    return properties[key]


def get_name():
    return name
