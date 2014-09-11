namespace eval ::tkpath {
    proc load_package {dir} {
        switch -glob "$::tcl_platform(os)/$::tcl_platform(machine)" {
            "Windows NT/*" {set sharedlib tkpath032.dll}
            "Darwin/*"     {set sharedlib libtkpath0.3.1.dylib}
            "Linux/i?86"   {set sharedlib libtkpath0.3.1.so}
            "Linux/x86_64" {set sharedlib libtkpath0.3.2.64.so}
            default        {error "tkpath: unsupported OS or architecture"}
        }
        load [file join $dir $sharedlib]
        source $dir/tkpath.tcl
    }
}

#HACK: we currently have version 0.3.2 binaries for Windows and 64 bit Linux, but version 0.3.1 for other platforms
if {$::tcl_platform(os)=="Windows NT" || "$::tcl_platform(os)/$::tcl_platform(machine)"=="Linux/x86_64"} {
   package ifneeded tkpath 0.3.2 [list ::tkpath::load_package $dir]
} else {
   package ifneeded tkpath 0.3.1 [list ::tkpath::load_package $dir]
}

#SHOULD BE JUST THIS LINE: package ifneeded tkpath 0.3.2 [list ::tkpath::load_package $dir]

