# mac-box

`opp-mac-shell` opens a shell on a Mac in a copy of the OMNeT++ tree you are working on, for
building and testing it on macOS while editing on Linux. Only source files are synced, one way
(Linux to Mac), so the build outputs of the two machines do not mix and the Mac builds on its
local disk.

## Setup

On the Mac: enable Remote Login (and Screen Sharing for `--gui`), authorize your ssh key, then

    brew install bison flex qt@6 pkgconf make

On Linux: `ssh` and `rsync`, and a VNC viewer for `--gui` (KRDC by default; set
`OPP_MAC_VNC_VIEWER` to use another, e.g. `"remmina -c"`; the `vnc://` URL is appended).

`opp-mac-shell --check` verifies both sides.

## Usage

    cd ~/w/omnetpp-dev
    opp-mac-shell
    ./configure && make -j8

- `opp-mac-shell` - sync, then open an interactive zsh on the Mac with `setenv` sourced, in the
  current subdirectory
- `opp-mac-shell <command> [args...]` - sync, then run a command there instead (e.g.
  `opp-mac-shell make -j8` after each edit)
- `opp-mac-shell --sync` - only sync, e.g. from another terminal while a shell is open
- `opp-mac-shell --gui` - also show the Mac desktop in the VNC viewer (macOS Screen Sharing tunnelled to
  `localhost:15900`); GUI apps started from the shell appear there
- `opp-mac-shell --find-root` - print the detected tree

The tree is found by walking up from the current directory to a directory containing `setenv`
and `configure.user.dist`, falling back to `$OPP_MAC_ROOT`. The Mac is `$OPP_MAC_HOST`
(default `mini.local`, as the local user unless set in `~/.ssh/config` or as
`user@host`); the copy is at `~/opp-trees/<path relative to your home>` there
and is kept between sessions.

The synced files are those under version control (`jj file list`, or `git ls-files` including
untracked but not ignored files), or all files if the tree has no VCS. Files deleted or renamed
on Linux are deleted on the Mac at the next sync. Ignored files, such as build outputs,
`configure.user` and `Makefile.inc`, are never synced; the Mac gets its own `configure.user`
from `configure.user.dist` on first use.

Changes made on the Mac are not synced back, and are overwritten by the next sync if the file
also exists on Linux.
