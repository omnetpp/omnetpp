"""
    omnetpp.sphinx.searchfix
    ~~~~~~~~~~~~~~~~~~~~~~~~

    Post-build patch that keeps the sphinx-immaterial client-side search working.

"""

import re
from pathlib import Path
from sphinx.application import Sphinx
from sphinx.util import logging

logger = logging.getLogger(__name__)

# Sphinx 9 emits the stopword list in _static/language_data.js as a JavaScript Set;
# it used to be an array. The search code bundled with sphinx-immaterial 0.13.9
# still probes it with indexOf(), so every query dies with "TypeError:
# stopwords.indexOf is not a function" and the site search stays empty. Patch the
# theme's bundle to accept both shapes. See
# https://github.com/jbms/sphinx-immaterial/issues/485 -- drop this module once the
# theme ships its own fix.
_BUNDLE_GLOB = 'sphinx_immaterial_theme.*.min.js'
# the bundle is minified, so match around whatever the minifier named the variable
_BROKEN = re.compile(r'stopwords\.indexOf\((\w+)\)!==-1')
_FIXED = r'(stopwords.has?stopwords.has(\1):stopwords.indexOf(\1)!==-1)'


def fix_stopwords_lookup(app: Sphinx, exception: Exception) -> None:
    if exception is not None:
        return

    get_theme_config = getattr(app.builder, 'get_theme_config', None)
    if get_theme_config is None or get_theme_config()[0] != 'sphinx_immaterial':
        return

    bundles = sorted((Path(app.outdir) / '_static').glob(_BUNDLE_GLOB))
    if len(bundles) != 1:
        logger.warning('search fixup: expected one %s, found %d, search may be broken',
                       _BUNDLE_GLOB, len(bundles))
        return

    bundle = bundles[0]
    source = bundle.read_text(encoding='utf-8')
    if 'stopwords.has' in source:
        return

    patched, count = _BROKEN.subn(_FIXED, source)
    if count != 1:
        logger.warning('search fixup: %d stopword lookups in %s, expected 1, search may be broken',
                       count, bundle.name)
        return

    bundle.write_text(patched, encoding='utf-8')
