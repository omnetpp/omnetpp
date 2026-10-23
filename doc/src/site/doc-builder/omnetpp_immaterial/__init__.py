"""The sphinx_immaterial theme in the density of the former OMNeT++ documentation.

The sidebars show more entries: the entries are closer together, and nested lists are shaded by their depth. The
entries that lead into the parts of the site named by omnetpp_nav_nowrap (e.g. an API reference), and the table
of contents of their pages, scroll the sidebar sideways instead of wrapping. The reader sets the widths of the
primary sidebar and of the content by dragging their edges, for every page of the site and later visits. Table
rows are lower, too.

The tabs open their sections as menus (omnetpp_nav_menus), with submenus down to omnetpp_nav_menu_depth levels, built
from the navigation that the page carries. The tabs named by omnetpp_nav_dropdown (or all, with True) show no
navigation sidebar on wide screens, which gives its width to the content, and are navigated by their menus.

The table of contents follows the section in view also with the theme's instant navigation, which the theme's own
tracking does not. With omnetpp_homepage set, the logo and the name of the site in the header lead to that URL.
The footer credits the OMNeT++ Discrete Event Simulator.

A build that reads no document still writes the CSS and JavaScript bundles of the theme (see bundle_always).

It is part of the doc-builder image, for every OMNeT++ site: a site enables it by listing 'omnetpp_immaterial' in its
extensions, with html_theme = 'sphinx_immaterial'.
"""

import json
import os

import markupsafe

HERE = os.path.dirname(os.path.abspath(__file__))


def config_inited(app, config):
    config.html_static_path.append(os.path.join(HERE, 'static'))
    config.templates_path.append(os.path.join(HERE, 'templates'))


def builder_inited(app):
    if app.config.omnetpp_nav_nowrap:
        app.add_js_file(None, body=f'window.omnetppNavNowrap = {json.dumps(list(app.config.omnetpp_nav_nowrap))};')
    if app.config.omnetpp_nav_menus:
        menus = {'depth': app.config.omnetpp_nav_menu_depth}
        app.add_js_file(None, body=f'window.omnetppNavMenus = {json.dumps(menus)};')


def dropdown(app, context):
    """Whether the tab of the page is navigated by its menu only."""
    tabs = app.config.omnetpp_nav_dropdown
    if not tabs or 'nav' not in context:
        return False
    tab = next((entry for entry in context['nav'] if entry.active), None)
    if tab is None:
        return False
    return tabs is True or markupsafe.Markup(tab.title).striptags() in tabs


def page_context(app, pagename, templatename, context, doctree):
    # The theme leads the logo to the root document of the build, e.g. the start page of a version.
    if app.config.omnetpp_homepage and 'config' in context:
        context['config'].setdefault('extra', {})['homepage'] = app.config.omnetpp_homepage
    # The theme's own way of hiding the navigation of a page (hide-navigation), which its instant navigation follows.
    if dropdown(app, context) and 'page' in context:
        hide = context['page'].setdefault('meta', {}).setdefault('hide', [])
        if 'navigation' not in hide:
            hide.append('navigation')


def bundle_always(app):
    """The theme generates its CSS and JavaScript bundles (and the CSS of its admonitions) on env-check-consistency,
    which Sphinx only emits when it has read at least one document. A build into an empty output directory that
    keeps the doctrees and reads nothing, e.g. after only a template has changed, would write every page without
    the stylesheets and scripts of the theme."""
    checked = False

    def consistency_checked(app, env):
        nonlocal checked
        checked = True

    def write_started(app, builder):
        if not checked:
            app.events.emit('env-check-consistency', builder.env)

    app.connect('env-check-consistency', consistency_checked)
    app.connect('write-started', write_started)


def setup(app):
    app.add_config_value('omnetpp_homepage', None, 'html')
    app.add_config_value('omnetpp_nav_nowrap', [], 'html')
    app.add_config_value('omnetpp_nav_menus', True, 'html')
    app.add_config_value('omnetpp_nav_menu_depth', 3, 'html')
    app.add_config_value('omnetpp_nav_dropdown', False, 'html', types=(bool, list, tuple))
    app.connect('config-inited', config_inited)
    app.connect('builder-inited', builder_inited)
    app.connect('html-page-context', page_context, priority=900)
    bundle_always(app)
    app.add_css_file('omnetpp-immaterial.css')
    app.add_js_file('omnetpp-immaterial.js', defer='defer')
    return {'parallel_read_safe': True, 'parallel_write_safe': True}
