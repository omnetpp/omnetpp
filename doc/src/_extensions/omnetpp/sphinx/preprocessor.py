"""
    omnetpp.sphinx
    ~~~~~~~~~~~~~~

    Pre-process a source file

    - Global replacement support (required because |xxx| replacement does not work inside inline blocks)
      Note that this replacement does not work on files read by ..include directives

"""

def opp_preprocess(app, docname, source):
    result = source[0]
    for key in app.config.opp_replacements:
        result = result.replace(key, app.config.opp_replacements[key])
        
    source[0] = result
