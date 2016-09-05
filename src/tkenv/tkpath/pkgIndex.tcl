namespace eval ::tkpath {
    proc load_package {dir} {
        switch -glob "$::tcl_platform(os)/$::tcl_platform(machine)" {
            "Windows NT/i?86" {set sharedlib libtkpath0.3.3.dll}
            "Windows NT/amd64" {set sharedlib libtkpath0.3.3-64.dll}
            "Darwin/*"     {set sharedlib libtkpath0.3.3.dylib}
            "Linux/i?86"   {set sharedlib libtkpath0.3.3.so}
            "Linux/x86_64" {set sharedlib libtkpath0.3.3-64.so}
            default        {error "tkpath: unsupported OS or architecture"}
        }
        load [file join $dir $sharedlib]
        source $dir/tkpath.tcl
    }
}

package ifneeded tkpath 0.3.3 [list ::tkpath::load_package $dir]
