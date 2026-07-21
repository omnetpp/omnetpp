__version__ = "1.13.0"

def setup(app):
    from .doxylink import setup_doxylink_roles
    app.add_config_value('doxylink', {}, 'env')
    app.add_config_value('doxylink_pdf_files', {}, 'env')
    # Namespaces of external libraries whose symbols are intentionally not in our
    # tag files (e.g. 'osg', 'std'). References to them render as plain code
    # without an "unresolved reference" warning.
    app.add_config_value('doxylink_external_namespaces', [], 'env')
    app.add_config_value('doxylink_parse_error_ignore_regexes',
                         default=[], types=[str], rebuild='env')
    app.connect('builder-inited', setup_doxylink_roles)

    return {
        "version": __version__,
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
