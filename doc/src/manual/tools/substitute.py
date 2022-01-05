import sys
import os
import re
import glob

#
# Process @BEGINFILE...@ENDFILE directives in LaTex files.
# Example:
#
# % @BEGINFILE example.txt
# ...
# % @ENDFILE
#
# The contents of the file example.txt will be inserted between
# the @BEGINFILE and @ENDFILE lines, replacing the original content
# in there.
#

def process_files(glob_pattern):
    for fname in glob.glob(glob_pattern):
        process_file(fname)

def process_file(fname):
    with open(fname) as f:
        content = f.read()
        updated = process_content(content, fname)
    if content != updated:
        with open(fname+".NEW", "w") as f:
            f.write(updated)
        if os.path.exists(fname+".bak"):
            os.remove(fname+".bak")
        os.rename(fname, fname+".bak")
        os.rename(fname+".NEW", fname)

def process_content(content, contentfname):
    dir = os.path.dirname(contentfname)
    def do_subst(m):
        fname = m.group(1)
        print("substituting", fname, "in", contentfname)
        with open(os.path.join(dir,fname)) as f:
            content = f.read().strip()
        return f"% @BEGINFILE {fname}\n{content}\n% @ENDFILE"
    pattern = r'%+\s*@\s*BEGINFILE\s+(.*?)\s*\n([\s\S]*?)\n%+\s*@\s*ENDFILE' # [\s\S]: any char including newline
    return re.sub(pattern, do_subst, content)

files = sys.argv[1:]
if files:
    for file in files:
        process_files(file)
else:
    print("no files processed")

#process_files("*.tex")