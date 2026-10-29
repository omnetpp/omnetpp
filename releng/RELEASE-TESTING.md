# Testing the distribution archives

Before a release, the archives that `build-omnetpp-dist` puts into `releng/dist` are installed and
tested on every supported Linux distro, on macOS and on Windows. This file is the procedure, for
people and for coding agents. `misc/test-dist` does the mechanical part; triage, the install guide
cross-check and the report need judgement.

## Prerequisites

- **podman** with enough disk space: about 6 GB per distro image and 5 GB for the Windows tree.
- **7z** on the host, to unpack the Windows archive.
- **A Mac** reachable over ssh as `$OPP_MAC_HOST` (default `mini.local`), with Homebrew in
  `/opt/homebrew` and about 15 GB free. The ssh key must be in the agent (`ssh-add`); the scripts
  use `BatchMode` and cannot ask for a passphrase. If the plain `ssh` command does not work in your
  environment, set `OPP_SSH` to a wrapper script, which is also used for scp. It must be a single
  executable, because `scp -S` takes no arguments. For example, when ssh rejects a system config file
  ("Bad owner or permissions", as with the libvirt ssh proxy config on NixOS), use a script that
  runs `exec ssh -F "$HOME/.ssh/config" "$@"`.
- Archives of a single build in `releng/dist`. The build id is read from the Linux archive, and
  results go to `releng/dist/test-results/<buildid>/`.

## Procedure

1. **Update the distro matrix**, `misc/test-dist.distros`. List the supported releases of each
   distro family: the Ubuntu LTS and newer releases, the latest Debian stable release, the current Fedora release only, AlmaLinux 9
   and 10, the latest openSUSE Leap and Tumbleweed (images from registry.opensuse.org, which are
   more up to date than docker.io's), and Arch. Only the newest Ubuntu LTS runs
   `test_quick`; the other distros get installation and the smoke test. Windows and macOS always
   run `test_quick`.
2. **Run the tests.** The three platforms are independent and can run at the same time. Each
   takes up to an hour; run them in the background and wait for them to exit.

        releng/misc/test-dist linux all
        releng/misc/test-dist windows
        releng/misc/test-dist macos

   `linux` installs each distro in a fresh container from its base image, as a non-root user
   with sudo, using `./install.sh -y`. It then commits the container as
   `opprel-<name>:installed` and runs the smoke test (and `test_quick` where enabled) on that
   image. `linux --resume <name>` reruns only the second part, for example after fixing a
   test. `TEST_DIST_JOBS` (default 2) and `TEST_DIST_CPUS` (default 16) set the parallelism.
3. **Triage** with `releng/misc/test-dist summary` and the files in each platform directory:
   `summary.txt`, `install.log`, `build-warnings.txt`, `smoke.log`, `test_quick.log`,
   `test_quick-failures.txt` and `test_quick-warnings.txt`. Compare with the previous report
   (see Known issues below). Before calling something a product bug, rule out the test harness:
   rerun the failing command by hand in the installed image
   (`podman run --rm -it localhost/opprel-<name>:installed sudo -iu opp`).
4. **Cross-check the install guide** (`doc/src/installguide`), described below.
5. **Write the report**, described below.
6. **Fix findings only with the release manager's approval.** Put each fix in its own jj change,
   verify it in the installed images or on the Mac, and mark it "fixed, not in this build" in
   the report until a rebuilt archive is tested.
7. **Clean up** after the release manager accepts the results:

        releng/misc/test-dist clean          # prints the commands
        releng/misc/test-dist clean --run    # runs them

   This removes the `opprel-*` containers and images, `releng/dist/test-work` and
   `~/opp-dist-test` on the Mac. It keeps the pulled base images and `releng/dist/test-results`.
   Packages that install.sh installed on the Mac with Homebrew stay installed.

## What is tested

- **Install:** `install.sh -y` with the arguments from the matrix. On Windows, the archive bundles its
  toolchain, so the test runs `./configure && make` in the MSYS2 shell instead.
- **Smoke test:** aloha, fifo and tictoc in Cmdenv (release), fifo_dbg, `opp_scavetool`,
  `omnetpp.scave` from the venv, `opp_charttool`, a new project built with `opp_makemake`, and the IDE's JRE.
  GUI programs are not started.
- **test_quick:** `make -k -C test test_quick`. `-k` keeps a failing suite from hiding the results of
  the later ones.

Not covered by the scripts: `test_build`, `test_toolchain`, the GUIs, native Windows, the
linux-aarch64, core and source archives, and the macOS archive for the other architecture.
List what was not covered in the report.

## Known issues and traps

- **MSYS2 runtime under Wine.** The toolchain bundled with the Windows archive (msys2-runtime
  3.6.1) crashes on every fork under Wine. `test-dist windows` first runs a fork probe on the
  shipped runtime and records the result (`msys2_runtime_fork` in `summary.txt`). If the probe fails, it
  replaces `msys-2.0.dll` with the newer one from the Wine image. Report the failing probe as a
  finding. Native Windows is not affected.
- **Platform dependent fingerprints.** Some histogram tests use transcendental functions, whose
  results differ slightly between Linux, macOS, Windows and Wine. `test/fingerprint/*.csv` lists one
  fingerprint per platform, in the order given by the comment at the top of each file. If a
  platform produces a new value, measure it, for example
  `./histograms -u Cmdenv -c Normal -r 0 --sim-time-limit=1000s --fingerprint=0000-0000/rsz`, and add it
  only after approval.
- **Expected test_quick results:** core has 1 expected fail (`NED_paramcrossref_5`) and about 11
  skipped tests (`WITH_LIBXML=no` and similar). `test/scave/multi_project` prints "Chart script
  silently failed to create data file", and it still passes.
- **Harness scripts must not use `set -u`:** `setenv` reads unset variables and the script would stop.
- **No `grep -q` at the end of a pipeline under `pipefail`:** `test-dist` sets `pipefail`. `grep -q`
  exits at the first match, the writer then dies of SIGPIPE, and the pipeline fails even though the
  text was found. Use `grep ... >/dev/null`.
- **Do not edit `test-dist` or `test-dist.d/` while a test runs:** bash reads a script while it
  executes it, and the Linux and Windows containers mount the harness instead of copying it.
- **macOS:** a plain `zsh -l` may find an Intel Homebrew in `/usr/local` before the arm64 one;
  `test-dist` uses `zsh -il`. install.sh runs `brew install`, which can upgrade unrelated packages
  and fail on stale files. Retry once after fixing the Mac before reporting an install failure.
- **RHEL family:** OpenSceneGraph is not packaged, so install.sh asks whether to continue without 3D
  and `-y` continues. The matrix runs alma9 with `--no-3d` and alma10 without it, so both paths are
  tested; both end with `with_osg: no`.

## Install guide cross-check

Compare the guide with what was actually tested, and report every mismatch as a finding with a
proposed fix:

- **Tested versions:** `ch-intro.rst`, `ch-supported-linux.rst`, the distro chapters
  (`ch-ubuntu.rst`, `ch-fedora.rst`, `ch-redhat.rst`, `ch-opensuse.rst`, `ch-archlinux.rst`,
  `ch-macos.rst`, `ch-windows-omnetpp.rst`), and the `tested versions` comments and the intro text in
  `install.sh`. All of them should match the matrix.
- **Package lists:** each chapter's package list against the matching branch of `install_deps` in
  `install.sh`, including the 3D and GUI packages and package names (libdw comes from elfutils,
  not libdwarf).
- **Described behaviour:** what the guide says install.sh does (prompts, flags, the Python version it
  uses) against what `install.log` shows.

## The report

The report is a claude.ai artifact, one per release version, updated in place for every tested
build. Ask the release manager for its URL, or list the artifacts to find it. The format is up to
you: choose whatever presents this build's results and findings best.
`misc/release-check.example.html` is the 6.5.0 report, an example of one that worked, not a
format to copy. Whatever the format, the report must make these clear:

- the build id and date, the overall outcome, and what changed since the previous build;
- for every platform: the environment (distro or OS, compiler, Python, Qt, OSG, libdw), and the
  install, smoke and `test_quick` results;
- the findings, each with its severity, its status (open, fixed in a jj change, verified in this
  build) and the evidence. Keep the numbers of findings from earlier builds stable;
- what was not covered.

Publish with the build id as the version label. Every number in the report must come from
`test-results`; do not carry over numbers from the previous build.
