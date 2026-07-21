#!/usr/bin/env python3
"""
check-conversion.py - audit the TeX->RST conversion of the OMNeT++ manual.

The manual is authored in LaTeX and converted to reStructuredText at build time
by doc/src/manual/tools/tex2rst (a regex pre/post-processor around pandoc). That
conversion is lossy: unhandled LaTeX commands can leak raw TeX into the output,
and the converter can emit roles that are not declared. This tool scans the
GENERATED .rst files and reports only HARD defects - ones that visibly break a
rendered page:

  * leftover LaTeX text/markup macros (\\textbf, \\emph, \\begin{...} ...) in prose
    or code (but NOT inside math, where LaTeX is legal)
  * internal converter sentinels that must never survive (### , §, \\opp)
  * roles the converter emits that are neither declared in prolog.inc nor built
    into Sphinx
  * malformed directive option lines (``name:`` instead of ``:name:``)

Soft defects (missing cross-references and the like) are deliberately out of
scope: Sphinx's own -n nitpicky build catches those better.

Context awareness is essential: LaTeX math commands (\\lambda, \\cdots, \\begin
{array} ...) are CORRECT inside :math:`...` and `.. math::` blocks, and literal
backslash sequences (\\n, \\t, C:\\Windows) are CORRECT inside code/literal
blocks. The scanner tracks block context so these do not raise false positives.

Usage:
    check-conversion.py [--report FILE] PATH [PATH ...]

PATH may be a directory (its ch-*.rst and appendix-*.rst are scanned) or an
explicit .rst file. Writes a markdown report to --report (default: stderr summary
only).

Exit codes:  0 clean, 1 HARD defect(s) found, 2 no .rst files matched PATH
(typically means 'make rst' has not run yet).
"""

import argparse
import glob
import os
import re
import sys

# --- role allow-list ---------------------------------------------------------

# Roles declared in doc/src/_templates/prolog.inc (custom OMNeT++ roles).
PROLOG_ROLES = {
    "class", "conf", "cpp", "def", "fun", "gate", "html", "ini", "kw", "latex",
    "mac", "ned", "opt", "par", "prog", "prop", "protocol", "var",
}
# Roles built into docutils/Sphinx that the converter may emit.
BUILTIN_ROLES = {
    "math", "ref", "numref", "eq", "doc", "download", "any", "term", "abbr",
    "index", "file", "command", "program", "guilabel", "kbd", "menuselection",
    "samp", "dfn", "mailheader", "makevar", "manpage", "mimetype", "newsgroup",
    "regexp", "emphasis", "strong", "literal", "code", "sub", "sup",
    "subscript", "superscript", "title", "pep", "rfc", "raw",
}
ALLOWED_ROLES = PROLOG_ROLES | BUILTIN_ROLES

# LaTeX macros that indicate a conversion FAILURE even inside code/literal blocks
# (unlike \n, \t, backslash paths, which are legitimate literal text there).
TEX_MARKUP_MACROS = re.compile(
    r"\\(?:textbf|textit|texttt|textrm|textsf|textsl|emph|underline|"
    r"textbackslash|tbf|ttt|opp|omnetpp|cclass|nedtype|msgtype|protocol|"
    r"ffunc|fvar|fmac|fkeyword|fpar|fprop|fgate|fconfig|fdef|fprog|fopt|"
    r"ffilename|ref|cite|label|includegraphics|includesvg|includepng|"
    r"includedot|appendixchapter|ensuremath|footnote|href|url|chapter|"
    r"section|subsection|subsubsection|item|begin|end)\b"
)

# \textbackslash may be glued to following text (e.g. "\textbackslashn"), so it
# needs its own boundary-free pattern to be caught inside code blocks.
TEX_ESCAPE_LEAK = re.compile(r"\\textbackslash")

# Any backslash-word — used only OUTSIDE math and code, where nothing should
# remain. Excludes escaped backslash (\\) which is a literal backslash in RST.
ANY_TEX_MACRO = re.compile(r"(?<!\\)\\[a-zA-Z]+")

# Converter sentinels / raw markers that must never reach the output.
SENTINELS = re.compile(r"###|§|\\opp\b")

# Inline spans whose contents are NOT prose and must be stripped before the
# generic outside-of-code scan: inline math, inline literals, any :role:`...`.
INLINE_MATH = re.compile(r":math:`[^`]*`")
INLINE_ROLE = re.compile(r":[a-zA-Z][a-zA-Z0-9_+-]*:`[^`]*`")
INLINE_LITERAL = re.compile(r"``[^`]*``")

ROLE_USE = re.compile(r"(?<![:`])(:([a-zA-Z][a-zA-Z0-9_+-]*):)`")

# Directive whose option lines lost their leading colon (pandoc/postprocess bug).
BAD_OPTION = re.compile(r"^\s+(name|caption|align|width|height|alt):\s")
DIRECTIVE = re.compile(r"^\s*\.\.\s+([a-zA-Z][-a-zA-Z0-9]*)::")

CODE_DIRECTIVES = ("code-block", "parsed-literal", "literalinclude", "sourcecode")

# A "`.. container:: <lang>`" for a known code language means a source code
# environment (e.g. \begin{python}) was NOT mapped to a code-block and rendered
# as reflowed prose, silently losing indentation/highlighting. Flag it.
UNMAPPED_CODE_CONTAINER = re.compile(
    r"^\s*\.\.\s+container::\s+"
    r"(python|cpp|c\+\+|ned|msg|ini|xml|java|bash|sh|json|yaml)\s*$"
)


class Finding:
    __slots__ = ("severity", "file", "line", "category", "text")

    def __init__(self, severity, file, line, category, text):
        self.severity = severity
        self.file = file
        self.line = line
        self.category = category
        self.text = text.strip()[:120]


def scan_file(path):
    """Return a list of Finding for one .rst file, tracking block context."""
    findings = []
    with open(path, encoding="utf-8") as f:
        lines = f.readlines()

    # Block state machine. mode in {prose, code, math}. A block runs while lines
    # are blank or indented deeper than base_indent.
    mode = "prose"
    base_indent = 0
    expect_literal = False        # previous prose line ended with '::'
    literal_parent_indent = 0

    def indent_of(s):
        return len(s) - len(s.lstrip(" "))

    for i, raw in enumerate(lines, start=1):
        line = raw.rstrip("\n")
        stripped = line.strip()
        indent = indent_of(line)

        # --- inside a code or math block ------------------------------------
        def check_code_line(ln, lineno):
            # Inside code/literal blocks only markup macros, escape leaks and
            # sentinels are defects; literal backslashes (\n, C:\Windows) are OK.
            for mm in TEX_MARKUP_MACROS.finditer(ln):
                findings.append(Finding("HARD", path, lineno, "tex-in-code",
                                        mm.group(0) + " | " + ln))
            for mm in TEX_ESCAPE_LEAK.finditer(ln):
                findings.append(Finding("HARD", path, lineno, "tex-in-code",
                                        mm.group(0) + " | " + ln))
            for mm in SENTINELS.finditer(ln):
                findings.append(Finding("HARD", path, lineno, "sentinel", ln))

        if mode in ("code", "math"):
            if stripped == "":
                continue
            if indent > base_indent:
                if mode == "code":
                    check_code_line(line, i)
                else:  # math: LaTeX is legal, only sentinels are wrong
                    for m in SENTINELS.finditer(line):
                        findings.append(Finding("HARD", path, i, "sentinel", line))
                continue
            # dedented out of block
            mode = "prose"
            # fall through to reinterpret this line as prose

        # --- resolve a pending literal block introduced by '::' -------------
        if expect_literal:
            if stripped == "":
                continue
            if indent > literal_parent_indent:
                mode = "code"
                base_indent = literal_parent_indent
                expect_literal = False
                check_code_line(line, i)  # reinterpret current line inside code
                continue
            expect_literal = False  # '::' was not followed by a literal block

        # --- prose line -----------------------------------------------------
        dmatch = DIRECTIVE.match(line)
        if dmatch:
            name = dmatch.group(1)
            if name == "math":
                mode = "math"
                base_indent = indent
                continue
            if name in CODE_DIRECTIVES:
                mode = "code"
                base_indent = indent
                continue
            # other directives: fall through (check their inline content)

        # source-code environment that was not mapped to a code-block
        if UNMAPPED_CODE_CONTAINER.match(line):
            findings.append(Finding("HARD", path, i, "unmapped-code-env", line))

        # malformed option line (lost leading colon)
        if BAD_OPTION.match(line):
            findings.append(Finding("HARD", path, i, "bad-directive-option", line))

        # sentinels anywhere in prose
        for m in SENTINELS.finditer(line):
            findings.append(Finding("HARD", path, i, "sentinel", line))

        # validate role names in use
        for m in ROLE_USE.finditer(line):
            role = m.group(2)
            if role not in ALLOWED_ROLES:
                findings.append(Finding("HARD", path, i, "undefined-role",
                                        ":" + role + ": | " + line))

        # strip inline math / roles / literals, then any leftover \macro is a defect
        scrub = INLINE_MATH.sub(" ", line)
        scrub = INLINE_ROLE.sub(" ", scrub)
        scrub = INLINE_LITERAL.sub(" ", scrub)
        for m in ANY_TEX_MACRO.finditer(scrub):
            findings.append(Finding("HARD", path, i, "tex-in-prose",
                                    m.group(0) + " | " + line))

        # note a trailing '::' that introduces a literal block next
        if stripped.endswith("::") and not stripped.startswith(".."):
            expect_literal = True
            literal_parent_indent = indent

    return findings


def collect_files(paths):
    files = []
    for p in paths:
        if os.path.isdir(p):
            files += sorted(glob.glob(os.path.join(p, "ch-*.rst")))
            files += sorted(glob.glob(os.path.join(p, "appendix-*.rst")))
        elif p.endswith(".rst"):
            files.append(p)
    return files


def main():
    ap = argparse.ArgumentParser(description="Audit TeX->RST conversion quality")
    ap.add_argument("paths", nargs="+", help="directories or .rst files to scan")
    ap.add_argument("--report", help="write a markdown report to this file")
    args = ap.parse_args()

    files = collect_files(args.paths)
    if not files:
        print("check-conversion: no .rst files found (run 'make rst' first?)",
              file=sys.stderr)
        return 2

    all_findings = []
    for path in files:
        all_findings += scan_file(path)

    hard = [f for f in all_findings if f.severity == "HARD"]

    # per file/category tally
    tally = {}
    for f in hard:
        tally.setdefault(os.path.basename(f.file), {}).setdefault(f.category, 0)
        tally[os.path.basename(f.file)][f.category] += 1

    lines = ["# TeX->RST conversion audit", ""]
    lines.append("Scanned %d files. **HARD defects: %d**." % (len(files), len(hard)))
    lines.append("")
    if hard:
        lines.append("| file | category | line | detail |")
        lines.append("|------|----------|------|--------|")
        for f in hard:
            lines.append("| %s | %s | %d | `%s` |" % (
                os.path.basename(f.file), f.category, f.line,
                f.text.replace("|", "\\|")))
        lines.append("")
        lines.append("## Summary by file")
        for fn in sorted(tally):
            cats = ", ".join("%s=%d" % (c, n) for c, n in sorted(tally[fn].items()))
            lines.append("- **%s**: %s" % (fn, cats))
    else:
        lines.append("No HARD defects. Conversion is clean. ✅")
    report = "\n".join(lines) + "\n"

    if args.report:
        with open(args.report, "w", encoding="utf-8") as fh:
            fh.write(report)

    # concise stderr summary
    print("check-conversion: %d HARD defect(s) across %d file(s)" %
          (len(hard), len(tally)), file=sys.stderr)
    for fn in sorted(tally):
        cats = ", ".join("%s=%d" % (c, n) for c, n in sorted(tally[fn].items()))
        print("  %s: %s" % (fn, cats), file=sys.stderr)

    return 1 if hard else 0


if __name__ == "__main__":
    sys.exit(main())
