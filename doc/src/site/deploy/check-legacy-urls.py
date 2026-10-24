#!/usr/bin/env python3
"""
Checks that the URLs of the documentation site from before it was versioned
(MkDocs, e.g. /tutorials/tictoc/part2/) still lead to a page of an assembled site.

    check-legacy-urls.py <site-dir>       check the tree made by assemble-site.sh
    check-legacy-urls.py --cloudflare     print the Cloudflare redirect rules

The old URLs end in a slash, which GitHub Pages only serves for directories, so
two Cloudflare Single Redirects (printed by --cloudflare, from the lists below)
handle them. This script resolves every old URL, with and without its trailing
slash, through those rules and GitHub Pages' own lookup, and fails if one does
not end on a page. The lists are the old site's complete set of pages, so they
never grow; what can break them is a page of the new site being renamed.
"""

import os
import sys

SITE_URL = "https://docs.omnetpp.org"

# Pages of the old site that the current documentation has, served from <path-without-slash>.html.
LEAF_PAGES = [
    "/articles/building-omnetpp-in-docker/",
    "/articles/neuralnet-errormodels/",
    "/articles/omnetpp-debugging-tips/",
    "/articles/porting-code-into-omnetpp/",
    "/articles/running-omnetppgui-in-docker/",
    "/articles/running-omnetpp-in-docker/",
    "/articles/wsl2/",
    "/articles/zero-conf-parsim/",
    "/tutorials/cloud/page1/",
    "/tutorials/cloud/page2/",
    "/tutorials/cloud/page4/",
    "/tutorials/cloud/page5/",
    "/tutorials/cloud/page6/",
    "/tutorials/cloud/page7/",
    "/tutorials/swarm/page1/",
    "/tutorials/swarm/page2/",
    "/tutorials/tictoc/conclusion/",
    "/tutorials/tictoc/part1/",
    "/tutorials/tictoc/part2/",
    "/tutorials/tictoc/part3/",
    "/tutorials/tictoc/part4/",
    "/tutorials/tictoc/part5/",
    "/tutorials/tictoc/part6/",
    "/tutorials/tictoc/part7/",
]

# Pages of the old site that are directories in the current one too.
DIRECTORY_PAGES = [
    "/",
    "/tutorials/cloud/",
    "/tutorials/swarm/",
    "/tutorials/tictoc/",
]

# Pages of the old site that the current documentation does not have: they lead to the home page.
GONE_PAGES = ["/tutorials/"]
GONE_PREFIXES = ["/tutorials/pandas"]
GONE_PREFIX_PAGES = ["/tutorials/pandas/", "/tutorials/pandas/index-src/", "/tutorials/pandas/pivottablejs.html"]

# Pages of the old site that are left to the 404 page.
NOT_FOUND = ["/showfile/"]


def cloudflare_rules():
    gone = " or ".join([f'http.request.uri.path eq "{p}"' for p in GONE_PAGES] +
                       [f'starts_with(http.request.uri.path, "{p}")' for p in GONE_PREFIXES])
    leaves = " ".join(f'"{p}"' for p in LEAF_PAGES)
    return f"""\
Rule 1 - removed pages to the home page
  When:   {gone}
  Then:   Static redirect, 301, URL {SITE_URL}/ , do not preserve the query string

Rule 2 - strip the trailing slash of the old pages
  When:   http.request.uri.path in {{{leaves}}}
  Then:   Dynamic redirect, 301, preserve the query string, expression
          concat("{SITE_URL}", substring(http.request.uri.path, 0, -1))
"""


def cloudflare(path):
    if path in GONE_PAGES or any(path.startswith(p) for p in GONE_PREFIXES):
        return "/"
    if path in LEAF_PAGES:
        return path[:-1]
    return None


def github_pages(site, path):
    """Returns (status, location) as GitHub Pages answers a request of path."""
    fs = os.path.join(site, path.lstrip("/"))
    if path.endswith("/"):
        return (200, None) if os.path.isfile(os.path.join(fs, "index.html")) else (404, None)
    if os.path.isfile(fs):
        return 200, None
    if os.path.isdir(fs):
        return 301, path + "/"
    if os.path.isfile(fs + ".html"):
        return 200, None
    return 404, None


def resolve(site, path):
    """Follows the redirects of a request; returns (final status, chain of paths)."""
    chain = [path]
    for _ in range(5):
        target = cloudflare(path)
        if target is not None:
            path = target
        else:
            status, location = github_pages(site, path)
            if status != 301:
                return status, chain
            path = location
        if path in chain:
            return "loop", chain + [path]
        chain.append(path)
    return "too many redirects", chain


def main():
    if sys.argv[1:] == ["--cloudflare"]:
        print(cloudflare_rules(), end="")
        return 0
    if len(sys.argv) != 2:
        print(__doc__.strip(), file=sys.stderr)
        return 2
    site = sys.argv[1]
    failures = 0
    for page in LEAF_PAGES + DIRECTORY_PAGES + GONE_PAGES + GONE_PREFIX_PAGES + NOT_FOUND:
        expected = 404 if page in NOT_FOUND else 200
        for path in dict.fromkeys([page, page.rstrip("/") or "/"]):
            status, chain = resolve(site, path)
            ok = status == expected
            failures += not ok
            print(f"{'ok  ' if ok else 'FAIL'} {status!s:>4}  {' -> '.join(chain)}")
    if failures:
        print(f"check-legacy-urls: {failures} old URL(s) do not lead to a page")
        return 1
    print("check-legacy-urls: all old URLs lead to a page")
    return 0


if __name__ == "__main__":
    sys.exit(main())
