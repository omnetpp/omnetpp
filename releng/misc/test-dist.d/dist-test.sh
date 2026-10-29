#!/usr/bin/env bash
#
# Installs and tests one OMNeT++ distribution archive as an ordinary user. Shared by the Linux
# containers, the Mac and the MSYS2 shell under Wine, so it must stay bash 3.2 compatible.
#
# Usage: dist-test.sh <archive|-> <workdir> <outdir> [install.sh args...]
#   archive  .tgz to unpack into workdir; "-" means the tree is already unpacked (Windows)
#   STEPS    environment variable, default "install smoke"; add "quick" for test/Makefile test_quick
#
ARCHIVE=$1; WORK=$2; OUT=$3; shift 3
STEPS=${STEPS:-install smoke}
mkdir -p "$OUT"
S=$OUT/summary.txt
touch "$S"

# replaces the key's previous value, so steps can be rerun
res() {
    grep -v "^$1:" "$S" > "$S.tmp"; mv "$S.tmp" "$S"
    echo "$1: $2" | tee -a "$S"
}
has_step() { case " $STEPS " in *" $1 "*) return 0;; esac; return 1; }
is_msys() { case "$(uname -o 2>/dev/null)" in *Msys*) return 0;; esac; return 1; }

if [ "$ARCHIVE" = "-" ]; then
    TREE=$WORK
else
    TREE=$WORK/$(tar tzf "$ARCHIVE" | head -1 | cut -d/ -f1)
fi

if has_step install; then
    if [ "$ARCHIVE" != "-" ]; then
        (cd "$WORK" && tar xzf "$ARCHIVE") || { res install_exit untar-failed; exit 1; }
    fi
    cd "$TREE" || exit 1
    res buildid "$(sed -n 's/^OMNETPP_BUILDID="\(.*\)"/\1/p' configure)"
    if is_msys; then
        # the Windows archive bundles its dependencies; install.sh would only run pacman
        (source setenv -q && ./configure && make -j${MAKE_JOBS:-16}) > "$OUT/install.log" 2>&1
    else
        ./install.sh -y "$@" > "$OUT/install.log" 2>&1
    fi
    rc=$?
    res install_exit $rc
    grep -E '\.[a-z]+:[0-9]+:[0-9]+: warning: ' "$OUT/install.log" | sed 's/^\(.*\/\)\{0,1\}\([^/:]*\):[0-9]*:[0-9]*: warning: /\2: /' | sort | uniq -c | sort -rn > "$OUT/build-warnings.txt"
    [ $rc -ne 0 ] && exit 1
fi

cd "$TREE" || exit 1
source setenv -q

res os "$( (. /etc/os-release 2>/dev/null && echo "$PRETTY_NAME") || (sw_vers -productVersion 2>/dev/null | sed 's/^/macOS /') || uname -sr)"
res arch "$(uname -m)"
res compiler "$(clang --version | head -1)"
res python "$(python3 --version 2>&1)"
qtver=$(pkg-config --modversion Qt6Core 2>/dev/null || qmake6 -query QT_VERSION 2>/dev/null || qmake -query QT_VERSION 2>/dev/null)
res qt "${qtver:-n/a}"
for v in WITH_QTENV WITH_OSG WITH_OSGEARTH; do
    res "$(echo $v | tr A-Z a-z)" "$(sed -n "s/^$v *= *//p" Makefile.inc)"
done
libdw=$(grep -ho 'libdw for enhanced backtrace support.*\.\.\. [a-z]*' "$OUT/install.log" 2>/dev/null | tail -1 | sed 's/.*\.\.\. //')
res libdw "${libdw:-n/a}"

if has_step smoke; then
    fail=0
    : > "$OUT/smoke.log"
    chk() {
        local n=$1; shift
        echo "=== $n" >> "$OUT/smoke.log"
        if ( "$@" ) >> "$OUT/smoke.log" 2>&1; then res "smoke_$n" ok; else res "smoke_$n" FAIL; fail=1; fi
    }
    chk aloha bash -c 'cd samples/aloha && ./aloha -u Cmdenv -c PureAloha1 --sim-time-limit=60s'
    chk fifo bash -c 'cd samples/fifo && ./fifo -u Cmdenv -c Fifo1 --sim-time-limit=1000s'
    chk tictoc bash -c 'cd samples/tictoc && ./tictoc -u Cmdenv -c Tictoc9 --sim-time-limit=100s'
    chk fifo_dbg bash -c 'cd samples/fifo && ./fifo_dbg -u Cmdenv -c Fifo1 --sim-time-limit=100s'
    chk scavetool bash -c 'set -o pipefail; cd samples/fifo && opp_scavetool q -l results/*.sca | tail -5'
    chk scave_py python3 -c 'import omnetpp.scave.results'
    chk charttool bash -c 'set -o pipefail; opp_charttool --help | tail -3'
    NP=$WORK/newproject
    chk makemake bash -c "rm -rf '$NP' && mkdir -p '$NP' && cd '$NP' &&
        printf 'simple Gen { gates: output out; }\nsimple Sink { gates: input in; }\nnetwork Net { submodules: g: Gen; s: Sink; connections: g.out --> s.in; }\n' > net.ned &&
        printf '#include <omnetpp.h>\nusing namespace omnetpp;\nstruct Gen : cSimpleModule { void initialize() override { send(new cMessage(\"m\"), \"out\"); } };\nstruct Sink : cSimpleModule { void handleMessage(cMessage *m) override { delete m; } };\nDefine_Module(Gen);\nDefine_Module(Sink);\n' > m.cc &&
        printf '[General]\nnetwork = Net\n' > omnetpp.ini &&
        opp_makemake -f -o newproject && make -j8 && ./newproject -u Cmdenv | grep -q 'End\.'"
    chk jre bash -c 'j=$(find ide -path "*jre*/bin/*" \( -name java -o -name java.exe \) -type f | head -1) && test -n "$j" && "$j" -version'
    [ $fail = 0 ] && res smoke PASS || res smoke FAIL
fi

if has_step quick; then
    # -k: a failing suite (e.g. fingerprint) must not hide the results of the later ones
    make -k -C test test_quick > "$OUT/test_quick.log" 2>&1
    res test_quick_exit $?
    awk '
        /^cd [^ ]+ && (\.\/runtest|make)$/ { suite = $2 }
        /^PASS: [0-9]+ +FAIL/ { gsub(/  +/, " "); print "tq_" suite ": " $0 }
        /^Ran [0-9]+ tests/ { ran = $2 }
        /^OK$/ && ran { print "tq_" suite ": " ran " tests, OK"; ran = "" }
        /^FAILED \(/ && ran { print "tq_" suite ": " ran " tests, " $0; ran = "" }
    ' "$OUT/test_quick.log" | while IFS= read -r line; do res "${line%%: *}" "${line#*: }"; done
    # suites without a PASS/FAIL summary line (e.g. scave) only show up as failed make targets
    failed=$(sed -n 's/^make: \*\*\* \[Makefile:[0-9]*: \(test_[a-z0-9_]*\)\].*/\1/p' "$OUT/test_quick.log" | tr '\n' ' ')
    res tq_failed_targets "${failed:-none}"
    grep -E 'FAILED \(should be|^FAIL: ' "$OUT/test_quick.log" > "$OUT/test_quick-failures.txt"
    grep -h 'WARNING:' "$OUT/test_quick.log" | sort -u > "$OUT/test_quick-warnings.txt"
fi
res done "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
