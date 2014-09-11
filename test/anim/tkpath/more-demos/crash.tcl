# How this file was recorded from a live tkenv session:
#
# 1. add the following to proc createGraphicalModuleViewer (moduleinspector.tcl), after the canvas ($c) is created:
#    set c $insp.c
#    tkp::canvas $c -background "#a0e0a0" -relief raised -closeenough 2
#
#    rename $c $c-old
#    proc $c args {
#      set procname [lindex [info level [info level]] 0]
#      puts "$procname $args";  # <------------- does the logging
#      uplevel 1 $procname-old $args
#    }
#
# 2. run tictoc from a console window -> choose Tictoc16 -> hit F7 -> hit F8 -> will crash!
#
# 3. save the output to a file, and postprocess it:
#     - add 'image create' command to create image missed by the code, plus canvas creation, plus loading tkpath etc.
#     - surround 'delete all' statement with catch, because they complain that the root item cannot be deleted

package require tkpath

frame .network
tkp::canvas .network.c
pack .network
pack .network.c
image create photo i280routing -height 16 -width 16

#.network.c configure -xscrollcommand {.network.hsb set} -yscrollcommand {.network.vsb set}
.network.c bind mod <1> {ModuleInspector:click .network}
.network.c bind submod <1> {ModuleInspector:click .network}
.network.c bind conn <1> {ModuleInspector:click .network}
.network.c bind msg <1> {ModuleInspector:click .network}
.network.c bind msgname <1> {ModuleInspector:click .network}
.network.c bind qlen <1> {ModuleInspector:qlenClick .network}
.network.c bind submod <Double-1> {ModuleInspector:dblClick .network}
.network.c bind conn <Double-1> {ModuleInspector:dblClick .network}
.network.c bind msg <Double-1> {ModuleInspector:dblClick .network}
.network.c bind msgname <Double-1> {ModuleInspector:dblClick .network}
.network.c bind qlen <Double-1> {ModuleInspector:qlenDblclick .network}
.network.c bind submod <3> {ModuleInspector:rightClick .network %X %Y %x %y}
.network.c bind conn <3> {ModuleInspector:rightClick .network %X %Y %x %y}
.network.c bind msg <3> {ModuleInspector:rightClick .network %X %Y %x %y}
.network.c bind msgname <3> {ModuleInspector:rightClick .network %X %Y %x %y}
.network.c bind mod <3> {ModuleInspector:rightClick .network %X %Y %x %y}
.network.c bind modname <3> {ModuleInspector:rightClick .network %X %Y %x %y}
.network.c bind qlen <3> {ModuleInspector:qlenRightClick .network %X %Y %x %y}
catch {.network.c delete all}
.network.c delete dx
.network.c create image 50 50 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B14C8}
.network.c create text 50 73 -text {tic[0]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 50 50 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1588}
.network.c create text 50 73 -text {tic[1]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 50 50 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1708}
.network.c create text 50 73 -text {tic[2]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 50 50 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B17C8}
.network.c create text 50 73 -text {tic[3]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 50 50 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1948}
.network.c create text 50 73 -text {tic[4]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 50 50 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1AC8}
.network.c create text 50 73 -text {tic[5]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c lower range
.network.c bbox submod
.network.c create rect -1 -1 101 101 -fill grey82 -width 2 -outline black -tags {dx mod ptr0099D050}
.network.c create text 3 3 -text Tictoc16 -anchor nw -tags {dx tooltip modname ptr0099D050} -font CanvasFont
.network.c lower mod
.network.c find withtag ptr009B14C8
.network.c bbox ptr009B14C8
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F07E8}
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c find withtag ptr009B14C8
.network.c bbox ptr009B14C8
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F0908}
.network.c lower ptr009F0908 dx
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c find withtag ptr009B1708
.network.c bbox ptr009B1708
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1178}
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1328}
.network.c find withtag ptr009B1708
.network.c bbox ptr009B1708
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1250}
.network.c lower ptr009F1250 dx
.network.c find withtag ptr009B17C8
.network.c bbox ptr009B17C8
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F14D8}
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1400}
.network.c lower ptr009F1400 dx
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c find withtag ptr009B17C8
.network.c bbox ptr009B17C8
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F15B0}
.network.c lower ptr009F15B0 dx
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c find withtag ptr009B1AC8
.network.c bbox ptr009B1AC8
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F3078}
.network.c find withtag ptr009B1AC8
.network.c bbox ptr009B1AC8
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c create line {50 30 50 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F3150}
.network.c lower ptr009F3150 dx
.network.c raise bubble
.network.c bbox all
.network.c config -scrollregion {-12 -12 112 112}
.network.c delete fig
.network.c delete nexteventmarker
.network.c delete msg msgname
catch {.network.c delete all}
.network.c delete dx
.network.c create image 183.674 50 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B14C8}
.network.c create text 183.674 73 -text {tic[0]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 181.604 182.263 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1588}
.network.c create text 181.604 205.263 -text {tic[1]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 50 195.708 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1708}
.network.c create text 50 218.708 -text {tic[2]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 274.43 405.378 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B17C8}
.network.c create text 274.43 428.378 -text {tic[3]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 279.409 273.207 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1948}
.network.c create text 279.409 296.207 -text {tic[4]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c create image 411.431 264.673 -image i280routing -anchor center -tags {dx tooltip submod submodext ptr009B1AC8}
.network.c create text 411.431 287.673 -text {tic[5]} -anchor n -tags {dx submodext} -font CanvasFont
.network.c lower range
.network.c bbox submod
.network.c create rect -1 -1 462 456 -fill grey82 -width 2 -outline black -tags {dx mod ptr0099D050}
.network.c create text 3 3 -text Tictoc16 -anchor nw -tags {dx tooltip modname ptr0099D050} -font CanvasFont
.network.c lower mod
.network.c find withtag ptr009B14C8
.network.c bbox ptr009B14C8
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c create line {183.697 70 182.303 162} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F07E8}
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c find withtag ptr009B14C8
.network.c bbox ptr009B14C8
.network.c create line {182.303 162 183.697 70} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F0908}
.network.c lower ptr009F0908 dx
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c find withtag ptr009B1708
.network.c bbox ptr009B1708
.network.c create line {162 184.121 70 193.879} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1178}
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c create line {202 200.763 259 254.237} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1328}
.network.c find withtag ptr009B1708
.network.c bbox ptr009B1708
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c create line {70 193.879 162 184.121} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1250}
.network.c lower ptr009F1250 dx
.network.c find withtag ptr009B17C8
.network.c bbox ptr009B17C8
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c create line {274.758 385 278.242 293} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F14D8}
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c create line {259 254.237 202 200.763} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F1400}
.network.c lower ptr009F1400 dx
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c find withtag ptr009B17C8
.network.c bbox ptr009B17C8
.network.c create line {278.242 293 274.758 385} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F15B0}
.network.c lower ptr009F15B0 dx
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c find withtag ptr009B1AC8
.network.c bbox ptr009B1AC8
.network.c create line {299 271.788 391 266.212} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F3078}
.network.c find withtag ptr009B1AC8
.network.c bbox ptr009B1AC8
.network.c find withtag ptr009B1948
.network.c bbox ptr009B1948
.network.c create line {391 266.212 299 271.788} -arrow none -fill black -dash {} -width 1 -tags {dx tooltip conn submodext ptr009F3150}
.network.c lower ptr009F3150 dx
.network.c raise bubble
.network.c bbox all
.network.c config -scrollregion {-12 -12 473 467}
.network.c delete fig
.network.c delete nexteventmarker
.network.c delete msg msgname
.network.c bbox mod
.network.c config -width 495.0
.network.c config -height 489.0
.network.c bbox all
.network.c config -scrollregion {-12 -12 473 467}
.network.c bbox mod
.network.c xview moveto 0.0
.network.c yview moveto 0.0
.network.c delete nexteventmarker
.network.c delete msg msgname
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 49
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 50 49
.network.c move 50 10.430999999999983 -27.326999999999998
.network.c move 49 10.430999999999983 -27.326999999999998
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 51
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 52 51
.network.c move 52 9.674000000000007 -27
.network.c move 51 9.674000000000007 -27
.network.c create text 274.43 405.378 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 53
.network.c create polygon {177 395 179 393 274 393 274 393 370 393 372 395 372 405 372 405 372 415 370 417 274 417 274 417 264 432 264 432 264 417 264 417 179 417 177 415 177 405 177 405} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 54 53
.network.c move 54 10.430000000000007 -26.622000000000014
.network.c move 53 10.430000000000007 -26.622000000000014
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 55
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 56 55
.network.c move 56 10.430999999999983 -27.326999999999998
.network.c move 55 10.430999999999983 -27.326999999999998
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 57
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 58 57
.network.c move 58 10.408999999999992 -26.793000000000006
.network.c move 57 10.408999999999992 -26.793000000000006
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 59
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 60 59
.network.c move 60 10.430999999999983 -27.326999999999998
.network.c move 59 10.430999999999983 -27.326999999999998
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 61
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 62 61
.network.c move 62 10.408999999999992 -26.793000000000006
.network.c move 61 10.408999999999992 -26.793000000000006
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 63
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 64 63
.network.c move 64 10.430999999999983 -27.326999999999998
.network.c move 63 10.430999999999983 -27.326999999999998
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 65
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 66 65
.network.c move 66 9.674000000000007 -27
.network.c move 65 9.674000000000007 -27
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 67
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 68 67
.network.c move 68 9.604000000000013 -26.736999999999995
.network.c move 67 9.604000000000013 -26.736999999999995
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 69
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 70 69
.network.c move 70 10.408999999999992 -26.793000000000006
.network.c move 69 10.408999999999992 -26.793000000000006
.network.c create text 50 195.708 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 71
.network.c create polygon {-47 186 -45 184 50 184 50 184 146 184 148 186 148 196 148 196 148 206 146 208 50 208 50 208 40 223 40 223 40 208 40 208 -45 208 -47 206 -47 196 -47 196} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 72 71
.network.c move 72 10 -27.292
.network.c move 71 10 -27.292
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 73
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 74 73
.network.c move 74 9.674000000000007 -27
.network.c move 73 9.674000000000007 -27
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 75
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 76 75
.network.c move 76 10.408999999999992 -26.793000000000006
.network.c move 75 10.408999999999992 -26.793000000000006
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 77
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 78 77
.network.c move 78 9.674000000000007 -27
.network.c move 77 9.674000000000007 -27
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 79
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 80 79
.network.c move 80 9.604000000000013 -26.736999999999995
.network.c move 79 9.604000000000013 -26.736999999999995
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 81
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 82 81
.network.c move 82 10.408999999999992 -26.793000000000006
.network.c move 81 10.408999999999992 -26.793000000000006
.network.c create text 274.43 405.378 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 83
.network.c create polygon {177 395 179 393 274 393 274 393 370 393 372 395 372 405 372 405 372 415 370 417 274 417 274 417 264 432 264 432 264 417 264 417 179 417 177 415 177 405 177 405} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 84 83
.network.c move 84 10.430000000000007 -26.622000000000014
.network.c move 83 10.430000000000007 -26.622000000000014
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 85
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 86 85
.network.c move 86 9.674000000000007 -27
.network.c move 85 9.674000000000007 -27
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 87
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 88 87
.network.c move 88 9.604000000000013 -26.736999999999995
.network.c move 87 9.604000000000013 -26.736999999999995
.network.c create text 50 195.708 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 89
.network.c create polygon {-47 186 -45 184 50 184 50 184 146 184 148 186 148 196 148 196 148 206 146 208 50 208 50 208 40 223 40 223 40 208 40 208 -45 208 -47 206 -47 196 -47 196} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 90 89
.network.c move 90 10 -27.292
.network.c move 89 10 -27.292
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 91
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 92 91
.network.c move 92 9.674000000000007 -27
.network.c move 91 9.674000000000007 -27
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 93
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 94 93
.network.c move 94 10.430999999999983 -27.326999999999998
.network.c move 93 10.430999999999983 -27.326999999999998
.network.c create text 50 195.708 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 95
.network.c create polygon {-47 186 -45 184 50 184 50 184 146 184 148 186 148 196 148 196 148 206 146 208 50 208 50 208 40 223 40 223 40 208 40 208 -45 208 -47 206 -47 196 -47 196} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 96 95
.network.c move 96 10 -27.292
.network.c move 95 10 -27.292
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 97
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 98 97
.network.c move 98 9.674000000000007 -27
.network.c move 97 9.674000000000007 -27
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 99
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 100 99
.network.c move 100 9.604000000000013 -26.736999999999995
.network.c move 99 9.604000000000013 -26.736999999999995
.network.c delete nexteventmarker
.network.c delete msg msgname
.network.c delete 49 50
.network.c delete 51 52
.network.c delete 53 54
.network.c delete 55 56
.network.c delete 57 58
.network.c delete 59 60
.network.c delete 61 62
.network.c delete 63 64
.network.c delete 65 66
.network.c delete 67 68
.network.c delete 69 70
.network.c delete 71 72
.network.c create text 274.43 405.378 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 101
.network.c create polygon {177 395 179 393 274 393 274 393 370 393 372 395 372 405 372 405 372 415 370 417 274 417 274 417 264 432 264 432 264 417 264 417 179 417 177 415 177 405 177 405} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 102 101
.network.c move 102 10.430000000000007 -26.622000000000014
.network.c move 101 10.430000000000007 -26.622000000000014
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 103
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 104 103
.network.c move 104 9.604000000000013 -26.736999999999995
.network.c move 103 9.604000000000013 -26.736999999999995
.network.c create text 274.43 405.378 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 105
.network.c create polygon {177 395 179 393 274 393 274 393 370 393 372 395 372 405 372 405 372 415 370 417 274 417 274 417 264 432 264 432 264 417 264 417 179 417 177 415 177 405 177 405} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 106 105
.network.c move 106 10.430000000000007 -26.622000000000014
.network.c move 105 10.430000000000007 -26.622000000000014
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 107
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 108 107
.network.c move 108 10.408999999999992 -26.793000000000006
.network.c move 107 10.408999999999992 -26.793000000000006
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 109
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 110 109
.network.c move 110 9.604000000000013 -26.736999999999995
.network.c move 109 9.604000000000013 -26.736999999999995
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 111
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 112 111
.network.c move 112 10.408999999999992 -26.793000000000006
.network.c move 111 10.408999999999992 -26.793000000000006
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 113
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 114 113
.network.c move 114 9.674000000000007 -27
.network.c move 113 9.674000000000007 -27
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 115
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 116 115
.network.c move 116 10.408999999999992 -26.793000000000006
.network.c move 115 10.408999999999992 -26.793000000000006
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 117
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 118 117
.network.c move 118 9.674000000000007 -27
.network.c move 117 9.674000000000007 -27
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 119
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 120 119
.network.c move 120 9.604000000000013 -26.736999999999995
.network.c move 119 9.604000000000013 -26.736999999999995
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 121
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 122 121
.network.c move 122 10.430999999999983 -27.326999999999998
.network.c move 121 10.430999999999983 -27.326999999999998
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 123
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 124 123
.network.c move 124 10.408999999999992 -26.793000000000006
.network.c move 123 10.408999999999992 -26.793000000000006
.network.c create text 50 195.708 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 125
.network.c create polygon {-47 186 -45 184 50 184 50 184 146 184 148 186 148 196 148 196 148 206 146 208 50 208 50 208 40 223 40 223 40 208 40 208 -45 208 -47 206 -47 196 -47 196} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 126 125
.network.c move 126 10 -27.292
.network.c move 125 10 -27.292
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 127
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 128 127
.network.c move 128 9.674000000000007 -27
.network.c move 127 9.674000000000007 -27
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 129
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 130 129
.network.c move 130 10.430999999999983 -27.326999999999998
.network.c move 129 10.430999999999983 -27.326999999999998
.network.c create text 279.409 273.207 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 131
.network.c create polygon {182 263 184 261 279 261 279 261 375 261 377 263 377 273 377 273 377 283 375 285 279 285 279 285 269 300 269 300 269 285 269 285 184 285 182 283 182 273 182 273} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 132 131
.network.c move 132 10.408999999999992 -26.793000000000006
.network.c move 131 10.408999999999992 -26.793000000000006
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 133
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 134 133
.network.c move 134 9.604000000000013 -26.736999999999995
.network.c move 133 9.604000000000013 -26.736999999999995
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 135
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 136 135
.network.c move 136 10.430999999999983 -27.326999999999998
.network.c move 135 10.430999999999983 -27.326999999999998
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 137
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 138 137
.network.c move 138 9.674000000000007 -27
.network.c move 137 9.674000000000007 -27
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 139
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 140 139
.network.c move 140 9.604000000000013 -26.736999999999995
.network.c move 139 9.604000000000013 -26.736999999999995
.network.c create text 183.674 50 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 141
.network.c create polygon {87 40 89 38 184 38 184 38 280 38 282 40 282 50 282 50 282 60 280 62 184 62 184 62 174 77 174 77 174 62 174 62 89 62 87 60 87 50 87 50} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 142 141
.network.c move 142 9.674000000000007 -27
.network.c move 141 9.674000000000007 -27
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 143
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 144 143
.network.c move 144 9.604000000000013 -26.736999999999995
.network.c move 143 9.604000000000013 -26.736999999999995
.network.c create text 274.43 405.378 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 145
.network.c create polygon {177 395 179 393 274 393 274 393 370 393 372 395 372 405 372 405 372 415 370 417 274 417 274 417 264 432 264 432 264 417 264 417 179 417 177 415 177 405 177 405} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 146 145
.network.c move 146 10.430000000000007 -26.622000000000014
.network.c move 145 10.430000000000007 -26.622000000000014
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 147
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 148 147
.network.c move 148 9.604000000000013 -26.736999999999995
.network.c move 147 9.604000000000013 -26.736999999999995
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 149
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 150 149
.network.c move 150 10.430999999999983 -27.326999999999998
.network.c move 149 10.430999999999983 -27.326999999999998
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 151
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 152 151
.network.c move 152 9.604000000000013 -26.736999999999995
.network.c move 151 9.604000000000013 -26.736999999999995
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 153
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 154 153
.network.c move 154 10.430999999999983 -27.326999999999998
.network.c move 153 10.430999999999983 -27.326999999999998
.network.c create text 274.43 405.378 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 155
.network.c create polygon {177 395 179 393 274 393 274 393 370 393 372 395 372 405 372 405 372 415 370 417 274 417 274 417 264 432 264 432 264 417 264 417 179 417 177 415 177 405 177 405} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 156 155
.network.c move 156 10.430000000000007 -26.622000000000014
.network.c move 155 10.430000000000007 -26.622000000000014
.network.c create text 411.431 264.673 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 157
.network.c create polygon {314 255 316 253 411 253 411 253 507 253 509 255 509 265 509 265 509 275 507 277 411 277 411 277 401 292 401 292 401 277 401 277 316 277 314 275 314 265 314 265} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 158 157
.network.c move 158 10.430999999999983 -27.326999999999998
.network.c move 157 10.430999999999983 -27.326999999999998
.network.c create text 181.604 182.263 -text { ARRIVED, starting new one! } -anchor c -tags bubble -font CanvasFont
.network.c bbox 159
.network.c create polygon {85 172 87 170 182 170 182 170 278 170 280 172 280 182 280 182 280 192 278 194 182 194 182 194 172 209 172 209 172 194 172 194 87 194 85 192 85 182 85 182} -outline black -fill #F8F8D8 -width 1 -smooth 1 -tags bubble
.network.c lower 160 159
.network.c move 160 9.604000000000013 -26.736999999999995
.network.c move 159 9.604000000000013 -26.736999999999995
.network.c delete nexteventmarker
.network.c delete msg msgname
.network.c delete 73 74
.network.c delete 75 76
.network.c delete 77 78
.network.c delete 79 80
.network.c delete 81 82
.network.c delete 83 84
.network.c delete 85 86
.network.c delete 87 88
.network.c delete 89 90
.network.c delete 91 92
.network.c delete 93 94
.network.c delete 95 96
.network.c delete 97 98
.network.c delete 99 100
.network.c delete 101 102
.network.c delete 103 104
.network.c delete 105 106
.network.c delete 107 108
.network.c delete 109 110
.network.c delete 111 112
.network.c delete 113 114
.network.c delete 115 116
.network.c delete 117 118
.network.c delete 119 120
.network.c delete 121 122
.network.c delete 123 124
.network.c delete 125 126
.network.c delete 127 128
.network.c delete 129 130
.network.c delete 131 132
.network.c delete nexteventmarker
.network.c find withtag ptr009B1588
.network.c bbox ptr009B1588
.network.c create rect 160 160 204 204 -tags nexteventmarker -outline red -width 2
.network.c delete msg msgname
.network.c find withtag ptr009F1250
.network.c gettags 43
.network.c coords 43
.network.c create oval -5 -5 5 5 -fill red -outline red -tags {dx tooltip msg ptr0099D1B0}
.network.c move 162 156.033467377527 184.7538415796749
.network.c create text 156.033467377527 184.7538415796749 -text (TicTocMsg16)tic-1-to-5 -anchor n -font CanvasFont -tags {dx msgname ptr0099D1B0}
.network.c raise bubble
.network.c delete 133 134
.network.c delete 135 136
.network.c delete 137 138
.network.c delete 139 140
.network.c delete 141 142
.network.c delete 143 144
.network.c delete 145 146
.network.c delete 147 148
.network.c delete 149 150
.network.c delete 151 152

