namespace eval ::tkdock {
    proc load_package {dir} {
        if {"AppKit" ni [winfo server .]} {error "TkAqua Cocoa required"}
        load [file join $dir libtkdock1.0.dylib]
    }
}

package ifneeded tkdock 1.0 [list ::tkdock::load_package $dir]

