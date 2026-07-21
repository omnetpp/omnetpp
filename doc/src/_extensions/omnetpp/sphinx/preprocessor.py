"""
    omnetpp.sphinx
    ~~~~~~~~~~~~~~

    Pre-process a source file

    - Global replacement support (required because |xxx| replacement does not work inside inline blocks)
      Note that this replacement does not work on files read by .. include directives
      That replacement could be done the include-read event.
    - If :jinja: is added to the front matter of the page, the page is also run
      through the jinja templating engine.

"""
def opp_preprocess_include(app, relative_path, parent_docname, content):
    opp_preprocess(app, parent_docname, content)


def opp_preprocess(app, docname, content):
    # step 1: perform global replacement using 'opp_replacements' config key
    for key in app.config.opp_replacements:
        content[0] = content[0].replace(key, app.config.opp_replacements[key])

    # step 2: Render html pages as a jinja template for fancy templating goodness.

    # Make sure we're outputting HTML and :jinja: metatag is present otherwise skip the templating part
    if app.builder.format != 'html' or not ':jinja:' in content[0]:
        return

    # run the templating engine on the source file
    content[0] = app.builder.templates.render_string(content[0], app.config.html_context)
