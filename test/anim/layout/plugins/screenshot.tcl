#
# Loaded automatically by Tkenv on startup.
#

registerPlugin "screenshot"

proc screenshot:inspectorUpdate {} {
}

#
# Takes a screenshot of the network module, then exits.
# Requires the "scrot" program for taking screenshots.
#
# To activate it, create a "screenshots/" directory. If this
# directory does not exist, the script does nothing.
#
proc screenshot:newNetwork {} {
    if {[file isdirectory "screenshots"]} {
        set w [opp_inspect [opp_object_systemmodule] (default)]
        graphmodwin_togglelabels $w
        graphmodwin_togglearrowheads $w

        update
        after 1000
        update

        set filename screenshots/[opp_getactiveconfigname].png
        file delete $filename
        if [catch { exec scrot $filename } err] {
            puts "Screenshot failed: $err"
        } else {
        puts "Screenshot $filename taken."
        }
        opp_exitomnetpp
    }
}

