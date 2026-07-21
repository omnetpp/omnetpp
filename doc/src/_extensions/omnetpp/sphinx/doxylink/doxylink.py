import bisect
import os
import re
import requests
import shutil
import time
import xml.etree.ElementTree as ET
import urllib.parse
from collections import namedtuple
from typing import List, Optional, Union

from dateutil.parser import parse as parsedate
from docutils import nodes, utils
from sphinx.util.nodes import split_explicit_title
from sphinx.util.console import bold, standout  # type: ignore  # These are not explicitly exported as functions
from sphinx import __version__ as sphinx_version

if sphinx_version >= '1.6.0':
    from sphinx.util.logging import getLogger

from . import __version__
from .parsing import normalise, ParseException


class Entry(namedtuple('_Entry', ['name', 'kind', 'file', 'arglist'])):
    '''Represents a documentation entry produced by Doxygen.'''

    def matches(self, name: str, kind: Optional[str], arglist: Optional[str]) -> bool:
        '''
        Checks whether this entry has the specified name, kind, and argument list.

        Args:
            name (str): symbol name
            kind (Optional[str]): restrict to symbols of this kind
            arglist (Optional[str]): normalized argument list for overload resolution
        '''

        # Are we of the correct kind?
        if kind and self.kind != kind:
            return False

        # Ensure the name matches
        if not self.name.endswith(name):
            return False

        # "do_foo" doesn't match "foo"
        prefix = self.name[:-len(name)]
        if prefix and (prefix[-1].isidentifier() or prefix[-1].isnumeric()):
            return False

        if not arglist:
            # If no argument list is provided, anything matches
            return True

        return self.arglist == arglist


    def __lt__(self, other: Union["Entry", str]) -> bool:  # type:ignore
        '''
        Compares entries for sorting by reverse name. This allows `SymbolMap` to
        match "foo::bar" when searching for "bar".
        '''

        if isinstance(other, Entry):
            return self.name[::-1] < other.name[::-1]
        return self.name[::-1] < other


    @property
    def is_class(self) -> bool:
        '''Returns true if this is a class entry (``kind`` is ``"class"``)'''
        return self.kind == 'class'


    @property
    def is_template(self) -> bool:
        '''Returns true if this is a template entry'''
        return '<' in self.name


def report_info(env, msg, docname=None, lineno=None):
    '''Convenience function for logging an informational

    Args:
        msg (str): Message of the warning
        docname (str): Name of the document on which the error occured
        lineno (str): Line number in the document on which the error occured
    '''
    if sphinx_version >= '1.6.0':
        logger = getLogger(__name__)
        if lineno is not None:
            logger.info(msg, location=(docname, lineno))
        else:
            logger.info(msg, location=docname)
    else:
        env.info(docname, msg, lineno=lineno)


def report_warning(env, msg, docname=None, lineno=None):
    '''Convenience function for logging a warning

    Args:
        msg (str): Message of the warning
        docname (str): Name of the document on which the error occured
        lineno (str): Line number in the document on which the error occured
    '''
    if sphinx_version >= '1.6.0':
        logger = getLogger(__name__)
        if lineno is not None:
            logger.warning(msg, location=(docname, lineno))
        else:
            logger.warning(msg, location=docname)
    else:
        env.warn(docname, msg, lineno=lineno)

def is_url(str_to_validate: str) -> bool:
    ''' Helper function to check if string contains URL

    Args:
        str_to_validate (str): String to validate as URL

    Returns:
        bool: True if given string is a URL, False otherwise
    '''
    regex = re.compile(
        r'^(?:http|ftp)s?://' # http:// or https://
        r'(?:(?:[A-Z0-9](?:[A-Z0-9-]{0,61}[A-Z0-9])?\.)+(?:[A-Z]{2,6}\.?|[A-Z0-9-]{2,}\.?)|' #domain...
        r'localhost|' #localhost...
        r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})' # ...or ip
        r'(?::\d+)?' # optional port
        r'(?:/?|[/?]\S+)$', re.IGNORECASE)
    return bool(re.match(regex, str_to_validate))


class SymbolMap:
    """A SymbolMap maps symbols to Entries."""
    def __init__(self, xml_doc: ET.ElementTree, parse_error_ignore_regexes: Optional[List[str]] = None) -> None:
        entries = parse_tag_file(xml_doc, parse_error_ignore_regexes)

        # Sort the entry list for use with bisect
        self._entries = sorted(entries)


    def _find_entries(self, name: str, kind: Optional[str], arglist: Optional[str]) -> List[Entry]:
        '''
        Finds all potentially matching entries in the symbol list.

        Args:
            name (str): the name symbol to search for
            kind (Optional[str]): the kind of symbols to search for
            arglist (str): normalised function argument list

        Returns:
            list[Entry]: all entries whose name ends with 'name'
        '''

        matches = []

        # Thanks to the sorting, all we need to do is iterate from the first to
        # the last matching entry.
        start = bisect.bisect_left(self._entries, name[::-1])  # type:ignore
        for candidate in self._entries[start:]:
            if not candidate.name.endswith(name):
                # Reached the end of entries that end in 'name'
                break

            if candidate.matches(name, kind, arglist):
                # Found one
                matches.append(candidate)

        return matches


    def _disambiguate(self, name: str, candidates: List[Entry]) -> Entry:
        '''
        Returns the best-fitting candidate for the given symbol name. All
        candidates are expected to be valid.

        Args:
            name (str): symbol name
            candidates (list[Entry]): list of candidates to choose from

        Returns:
            Entry: the best candidate
        '''

        if not candidates:
            raise LookupError(f'No documentation entry matching "{name}"')

        # An exact match would appear at the beginning of the list.
        if len(candidates) == 1 or candidates[0].name == name:
            return candidates[0]

        # If there is more than one candidate then there is an ambiguity
        # Often this is due to the symbol matching the name of the constructor as well as the class name itself
        # We will prefer the class
        classes = [c for c in candidates if c.is_class]

        # If there is only one by here we return it.
        if len(classes) == 1:
            return classes[0]

        # Now, to disambiguate between ``PolyVox::Array< 1, ElementType >::operator[]`` and ``PolyVox::Array::operator[]`` matching ``operator[]``,
        # we will ignore templated (as in C++ templates) tag names by removing names containing ``<``
        no_templates = [c for c in candidates if not c.is_template]

        if len(no_templates) == 1:
            return no_templates[0]

        # If not found by now, return the shortest match, assuming that's the most specific
        if no_templates:
            # TODO return a warning here?
            return min(no_templates, key=lambda entry: len(entry.name))

        # TODO Offer fuzzy suggestion
        raise LookupError('Could not find a match')


    def __getitem__(self, item: str) -> Entry:
        symbol, normalised_arglist = normalise(item)

        # Restrict to functions when given an argument list
        kind = 'function' if normalised_arglist else None
        candidates = self._find_entries(symbol, kind, normalised_arglist)
        return self._disambiguate(symbol, candidates)


def parse_tag_file(doc: ET.ElementTree, parse_error_ignore_regexes: Optional[List[str]]) -> List[Entry]:
    """
    Takes in an XML tree from a Doxygen tag file and returns a list that looks something like:

    .. code-block:: python

        [Entry('PolyVox', ...),
         Entry('PolyVox::Array', ...),
         Entry('PolyVox::Array1DDouble'),
         Entry('PolyVox::Array1DFloat'),
         Entry('PolyVox::Array1DInt16'),
         Entry('QScriptContext::throwError'),
         Entry('QScriptContext::toString'),
        ]

    :Parameters:
        doc : xml.etree.ElementTree
            The XML DOM object

    :return: a list of entries mapping fully qualified symbols to files
    """

    entries: List[Entry] = []
    for compound in doc.findall('./compound'):
        compound_kind = compound.get('kind')
        if compound_kind not in {'namespace', 'class', 'struct', 'file', 'define', 'group', 'page'}:
            continue

        compound_name = compound.findtext('name')
        compound_filename = compound.findtext('filename')

        if compound_name is None:
            raise KeyError(f"Compound does not have a name")
        if compound_filename is None:
            raise KeyError(f"Compound {compound_name} does not have a filename")

        # TODO The following is a hack bug fix I think
        # Doxygen doesn't seem to include the file extension to <compound kind="file"><filename> entries
        # If it's a 'file' type, check if it _does_ have an extension, if not append '.html'
        if compound_kind in ('file', 'page') and not os.path.splitext(compound_filename)[1]:
            compound_filename = compound_filename + '.html'

        # If it's a compound we can simply add it
        entries.append(Entry(compound_name, kind=compound_kind, file=compound_filename, arglist=None))

        for member in compound.findall('member'):
            # If the member doesn't have an <anchorfile> element, use the parent compounds <filename> instead
            # This is the way it is in the qt.tag and is perhaps an artefact of old Doxygen
            anchorfile = member.findtext('anchorfile') or compound_filename
            member_name = member.findtext('name')
            if member_name is None:
                raise KeyError(f"Member of {compound_name} does not have a name")
            member_symbol = compound_name + '::' + member_name
            member_kind = member.get('kind')
            arglist = member.findtext('./arglist')  # If it has an <arglist> then we assume it's a function. Empty <arglist> returns '', not None. Things like typedefs and enums can have empty arglists

            member_file = join(anchorfile, '#', member.findtext('anchor'))

            if arglist and member_kind not in {'variable', 'typedef', 'enumeration', 'enumvalue'}:
                try:
                    # Parse arguments to do overload resolution later
                    normalised_arglist = normalise(member_symbol + arglist)[1]
                    entries.append(
                        Entry(name=member_symbol, kind=member_kind, file=member_file, arglist=normalised_arglist))
                except ParseException as e:
                    message = f'Skipping {member_kind} {member_symbol}{arglist}. Error reported from parser was: {e}'
                    should_report = True

                    if parse_error_ignore_regexes:
                        for pattern in parse_error_ignore_regexes:
                            try:
                                if re.search(pattern, message):
                                    should_report = False
                                    break
                            except re.error:
                                # Invalid regex pattern - ignore it
                                continue

                    if should_report:
                        report_warning(None, message)  # Use None as env since we don't have access to it here
                    continue
            else:
                # Put the simple things directly into the list
                entries.append(Entry(name=member_symbol, kind=member_kind, file=member_file, arglist=None))

    return entries


def join(*args):
    return ''.join(args)


def create_role(app, tag_filename, rootdir, cache_name, pdf=""):
    parse_error_ignore_regexes = getattr(app.config, 'doxylink_parse_error_ignore_regexes', [])

    if parse_error_ignore_regexes:
        report_info(app.env, f'Using parse error ignore patterns: {", ".join(parse_error_ignore_regexes)}')

    # Tidy up the root directory path
    if not rootdir.endswith(('/', '\\')):
        rootdir = join(rootdir, os.sep)

    try:
        if is_url(tag_filename):
            hresponse = requests.head(tag_filename, allow_redirects=True)
            if hresponse.status_code != 200:
                raise FileNotFoundError
            try:
                modification_time = parsedate(hresponse.headers['last-modified']).timestamp()
            except KeyError:  # no last-modified header from server
                modification_time = time.time()
            def _parse():
                response = requests.get(tag_filename, allow_redirects=True)
                if response.status_code != 200:
                    raise FileNotFoundError
                return ET.fromstring(response.text)
        else:
            modification_time = os.path.getmtime(tag_filename)
            def _parse():
                return ET.parse(tag_filename)

        report_info(app.env, bold('Checking tag file cache for %s: ' % cache_name))
        if not hasattr(app.env, 'doxylink_cache'):
            # no cache present at all, initialise it
            report_info(app.env, 'No cache at all, rebuilding...')
            mapping = SymbolMap(_parse(), parse_error_ignore_regexes)
            app.env.doxylink_cache = {cache_name: {'mapping': mapping, 'mtime': modification_time, 'version': __version__}}
        elif not app.env.doxylink_cache.get(cache_name):
            # Main cache is there but the specific sub-cache for this tag file is not
            report_info(app.env, 'Sub cache is missing, rebuilding...')
            mapping = SymbolMap(_parse(), parse_error_ignore_regexes)
            app.env.doxylink_cache[cache_name] = {'mapping': mapping, 'mtime': modification_time, 'version': __version__}
        elif app.env.doxylink_cache[cache_name]['mtime'] < modification_time:
            # tag file has been modified since sub-cache creation
            report_info(app.env, 'Sub-cache is out of date, rebuilding...')
            mapping = SymbolMap(_parse(), parse_error_ignore_regexes)
            app.env.doxylink_cache[cache_name] = {'mapping': mapping, 'mtime': modification_time}
        elif not app.env.doxylink_cache[cache_name].get('version') or app.env.doxylink_cache[cache_name].get('version') != __version__:
            # sub-cache doesn't have a version or the version doesn't match
            report_info(app.env, 'Sub-cache schema version doesn\'t match, rebuilding...')
            mapping = SymbolMap(_parse(), parse_error_ignore_regexes)
            app.env.doxylink_cache[cache_name] = {'mapping': mapping, 'mtime': modification_time, 'version': __version__}
        else:
            # The cache is up to date
            report_info(app.env, 'Sub-cache is up-to-date')
    except FileNotFoundError:
        tag_file_found = False
        report_warning(app.env, standout('Could not find tag file %s. Make sure your `doxylink` config variable is set correctly.' % tag_filename))
    else:
        tag_file_found = True

    def find_doxygen_link(name, rawtext, text, lineno, inliner, options={}, content=[]):
        # from :name:`title <part>`
        has_explicit_title, title, part = split_explicit_title(text)
        part = utils.unescape(part)
        warning_messages = []
        if not tag_file_found:
            warning_messages.append('Could not find match for `%s` because tag file not found' % part)
            return [nodes.inline(title, title)], []

        try:
            url = app.env.doxylink_cache[cache_name]['mapping'][part]
        except LookupError as error:
            # References to symbols in external libraries (e.g. osg::, std::) are
            # never in our tag file by design: render them as plain code without
            # a spurious "could not find match" warning.
            symbol = normalise(part)[0]
            if any(symbol.startswith(ns + '::') for ns in app.config.doxylink_external_namespaces):
                return [nodes.inline(title, title)], []
            inliner.reporter.warning(f'Could not find match for `{part}` in `{tag_filename}` tag file. Error reported was {error}', line=lineno)
            return [nodes.inline(title, title)], []
        except ParseException as error:
            inliner.reporter.warning('Error while parsing `%s`. Is not a well-formed C++ function call or symbol.'
                                     'If this is not the case, it is a doxylink bug so please report it.'
                                     'Error reported was: %s' % (part, error), line=lineno)
            return [nodes.inline(title, title)], []

        if pdf and app.builder.format == 'latex':
            full_url = join(pdf, '#', url.file)
            full_url = full_url.replace('.html#', '_')  # for links to variables and functions
            full_url = full_url.replace('.html', '')  # for links to files
        # If it's an absolute path then the link will work regardless of the document directory
        # Also check if it is a URL (i.e. it has a 'scheme' like 'http' or 'file')
        elif os.path.isabs(rootdir) or urllib.parse.urlparse(rootdir).scheme:
            full_url = join(rootdir, url.file)
        # But otherwise we need to add the relative path of the current document to the root source directory to the link
        else:
            relative_path_to_docsrc = os.path.relpath(app.env.srcdir, os.path.dirname(inliner.document.attributes['source']))
            full_url = join(relative_path_to_docsrc, '/', rootdir, url.file)  # We always use the '/' here rather than os.sep since this is a web link avoids problems like documentation/.\../library/doc/ (mixed slashes)

        if url.kind == 'function' and app.config.add_function_parentheses and normalise(title)[1] == '' and not has_explicit_title:
            title = join(title, '()')

        pnode = nodes.reference(title, title, internal=False, refuri=full_url)
        return [pnode], []

    return find_doxygen_link


def extract_configuration(values):
    if len(values) == 3:
        tag_filename, rootdir, pdf_filename = values
    elif len(values) == 2:
        tag_filename = values[0]
        if values[1].endswith('.pdf'):
            pdf_filename = values[1]
            rootdir = ""
        else:
            rootdir = values[1]
            pdf_filename = ""
    else:
        raise ValueError("Config variable `doxylink` is incorrectly configured. Expected a tuple with 2 to 3 "
                            "elements; got %s" % values)
    return tag_filename, rootdir, pdf_filename


def fetch_file(app, source, output_path):
    """Fetches file and puts it in the desired location if it does not exist yet.

    Local files will be copied and remote files will be downloaded.
    Directories in the ``output_path`` get created if needed.

    Args:
        app: Sphinx' application instance
        source (str): Path to local file or URL to remote file
        output_path (str): Path with filename to copy/download the source to, relative to Sphinx' output directory
    """
    if not os.path.isabs(output_path):
        output_path = os.path.join(app.outdir, output_path)
    if os.path.exists(output_path):
        return
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    if is_url(source):
        response = requests.get(source, allow_redirects=True)
        if response.status_code != 200:
            report_warning(app.env,
                        standout("Could not find file %r. Make sure your `doxylink_pdf_files` config variable is "
                                 "set correctly." % source))
            return
        with open(output_path, 'wb') as file:
            file.write(response.content)
    else:
        if not os.path.isabs(source):
            source = os.path.join(app.outdir, source)
        if os.path.exists(source):
            shutil.copy(source, output_path)
        else:
            report_warning(app.env,
                        standout("Expected a URL or a path that exists as value for `doxylink_pdf_files` "
                                    "config variable; got %r" % source))


def process_configuration(app, tag_filename, rootdir, pdf_filename):
    """Processes the configured values for ``doxylink`` and ``doxylink_pdf_files`` and warns about potential issues.

    The type of builder decides which values shall be used.

    Args:
        app: Sphinx' application instance
        tag_filename (str): Path to the Doxygen tag file
        rootdir (str): Path to the root directory of Doxygen HTML documentation
        pdf_filename (str): Path to the pdf file; may be empty when LaTeX builder is not used
    """
    if app.builder.format == 'latex':
        if not pdf_filename:
            if is_url(rootdir):
                report_warning(app.env,
                               "Linking from PDF to remote Doxygen html is not supported yet; got %r."
                               "Consider linking to a Doxygen pdf file instead as "
                               "third element of the tuple in the `doxylink` config variable." % rootdir)
            else:
                report_warning(app.env,
                               "Linking from PDF to local Doxygen html is not possible; got %r."
                               "Consider linking to a Doxygen pdf file instead as third element of the tuple in the "
                               "`doxylink` config variable." % rootdir)
        elif pdf_filename in app.config.doxylink_pdf_files:
            source = app.config.doxylink_pdf_files[pdf_filename]
            fetch_file(app, source, pdf_filename)
    elif pdf_filename and not rootdir:
        report_warning(app.env,
                       "Linking from HTML to Doxygen pdf (%r) is not supported. Consider setting "
                       "the root directory of Doxygen's HTML output as value instead." % pdf_filename)


def setup_doxylink_roles(app):
    for name, values in app.config.doxylink.items():
        tag_filename, rootdir, pdf_filename = extract_configuration(values)
        process_configuration(app, tag_filename, rootdir, pdf_filename)
        app.add_role(name, create_role(app, tag_filename, rootdir, name, pdf=pdf_filename))
