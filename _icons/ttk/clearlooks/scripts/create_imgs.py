#!/usr/bin/env python
# -*- mode: python; coding: koi8-r; -*-

import os
import gtk, gobject

imdir = 'images'
imtype = 'png'
background = '#efebe7'

#fill_color = 0xff000000                 # red
fill_color = int('ff000000', 16)

if not os.path.exists(imdir):
    os.mkdir(imdir)

gc = None
def draw_rect():
    global gc
    if gc is None:
        gc = drawing_area.window.new_gc()
        colormap = gtk.gdk.colormap_get_system()
        gc.set_colormap(colormap)
        color = gtk.gdk.color_parse('red')
        colormap.alloc_color(color)
        gc.set_rgb_fg_color(color)
    drawing_area.window.draw_rectangle(gc, True, 0,0, 800,800)


def save_image(fn, w, h, x=0, y=0):
    pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, True, 8, w, h)
    pixbuf.fill(fill_color)
    pb = pixbuf.get_from_drawable(drawing_area.window,
                                  drawing_area.get_colormap(),
                                  x,y, 0,0, w,h)
    pb.save(os.path.join(imdir, fn+"."+imtype), imtype)
    drawing_area.window.clear()
    draw_rect()


done = False
def save_callback(*args):
    global done



    if done: return
    done = True
    print 'create images'

    style = drawing_area.get_style()
    draw_rect()

    # separator
    w = 20
    style.paint_vline(drawing_area.window, gtk.STATE_NORMAL, None,
                       drawing_area, "frame", 0, w, 0)
    save_image('sep-v', 2, w)
    style.paint_hline(drawing_area.window, gtk.STATE_NORMAL, None,
                      drawing_area, "frame", 0, w, 0)
    save_image('sep-h', w, 2)


    # tree
    w, h = 32, 32
    w, h = 24, 24
    for fn, state, shadow in (
        ("tree-n", gtk.STATE_NORMAL,      gtk.SHADOW_OUT),
        ("tree-h", gtk.STATE_PRELIGHT,    gtk.SHADOW_OUT),
        ("tree-p", gtk.STATE_ACTIVE,      gtk.SHADOW_IN),
        ("tree-d", gtk.STATE_INSENSITIVE, gtk.SHADOW_IN),
        ):
        style.paint_box(drawing_area.window, state, shadow,
                        None, drawing_area, "stepper", 0,0, w,h)
        save_image(fn, w, h)


    # sizegrip
    w, h = 16, 16
    fn = 'sizegrip'
    style.paint_resize_grip(drawing_area.window, gtk.STATE_NORMAL, None,
                            drawing_area, "statusbar",
                            gtk.gdk.WINDOW_EDGE_SOUTH_EAST, 0,0, w,h)
    save_image(fn, w, h)


    # progress
    w, h = 37+3, 16+3
    progress_style = progress.get_style()
    fn = 'progress-h'
    progress_style.paint_box(drawing_area.window,
                             gtk.STATE_PRELIGHT, gtk.SHADOW_NONE,
                             None, progress, "bar", 0,0, w,h)
    save_image(fn, w, h)


    # button
    w, h = 32, 32
    w, h = 28, 28
    for fn, state, shadow in (
        ("button-n", gtk.STATE_NORMAL,      gtk.SHADOW_OUT),
        ("button-a", gtk.STATE_PRELIGHT,    gtk.SHADOW_OUT),
        ("button-p", gtk.STATE_ACTIVE,      gtk.SHADOW_IN),
        ("button-d", gtk.STATE_INSENSITIVE, gtk.SHADOW_OUT),
        ):
        style.paint_box(drawing_area.window, state, shadow,
                        None, drawing_area, "buttondefault", 0,0, w,h)
        save_image(fn, w, h)

    style.paint_box(drawing_area.window, gtk.STATE_PRELIGHT, gtk.SHADOW_IN,
                    None, togglebutton, "buttondefault", 0,0, w,h)
    save_image("button-pa", w, h)


    # toolbar
    w, h = 16, 16
    w, h = 24, 24

    fn = "blank"
    pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, True, 8, w, h)
    pixbuf.fill(fill_color)
    pixbuf.save(os.path.join(imdir, fn+"."+imtype), imtype)

    for fn, state, shadow in (
        ("toolbutton-n", gtk.STATE_NORMAL,      gtk.SHADOW_OUT),
        ("toolbutton-a", gtk.STATE_PRELIGHT,    gtk.SHADOW_OUT),
        ("toolbutton-p", gtk.STATE_ACTIVE,      gtk.SHADOW_IN),
        ("toolbutton-d", gtk.STATE_INSENSITIVE, gtk.SHADOW_IN),
        ):
        style.paint_box(drawing_area.window, state, shadow,
                        None, drawing_area, "buttondefault", 0,0, w,h)
        save_image(fn, w, h)

    style.paint_box(drawing_area.window, gtk.STATE_PRELIGHT, gtk.SHADOW_IN,
                    None, togglebutton, "buttondefault", 0,0, w,h)
    save_image("toolbutton-pa", w, h)


    # slider
    msl = hscroll.style_get_property("min_slider_length")
    msl = 20
    sw = hscroll.style_get_property("slider_width")
    print '>>', msl, sw
    for t, w, h, state, orient in (
        ('hn', msl,sw, gtk.STATE_NORMAL,      gtk.ORIENTATION_HORIZONTAL),
        ('ha', msl,sw, gtk.STATE_PRELIGHT,    gtk.ORIENTATION_HORIZONTAL),
        ('hp', msl,sw, gtk.STATE_NORMAL,      gtk.ORIENTATION_HORIZONTAL),
        ('hd', msl,sw, gtk.STATE_INSENSITIVE, gtk.ORIENTATION_HORIZONTAL),

        ('vn', sw,msl, gtk.STATE_NORMAL,      gtk.ORIENTATION_VERTICAL),
        ('va', sw,msl, gtk.STATE_PRELIGHT,    gtk.ORIENTATION_VERTICAL),
        ('vp', sw,msl, gtk.STATE_NORMAL,      gtk.ORIENTATION_VERTICAL),
        ('vd', sw,msl, gtk.STATE_INSENSITIVE, gtk.ORIENTATION_VERTICAL),
        ):
        fn = 'sbthumb-'+t
        if 0:
            style.paint_slider(drawing_area.window, state, gtk.SHADOW_OUT,
                               None, drawing_area, "slider", 0,0, w,h, orient)
        else:
            if orient == gtk.ORIENTATION_VERTICAL:
                w, h = h, w
            style.paint_box(drawing_area.window, state, shadow,
                            None, drawing_area, "stepper", 0,0, w,h)
        save_image(fn, w, h)


    msl = hscroll.style_get_property("min_slider_length")
    sw = hscroll.style_get_property("slider_width")
    # scale
    for t, w, h, state, orient in (
        ('hn', msl,sw, gtk.STATE_NORMAL,      gtk.ORIENTATION_HORIZONTAL),
        ('ha', msl,sw, gtk.STATE_PRELIGHT,    gtk.ORIENTATION_HORIZONTAL),
        ('hd', msl,sw, gtk.STATE_INSENSITIVE, gtk.ORIENTATION_HORIZONTAL),
        ('vn', sw,msl, gtk.STATE_NORMAL,      gtk.ORIENTATION_VERTICAL),
        ('va', sw,msl, gtk.STATE_PRELIGHT,    gtk.ORIENTATION_VERTICAL),
        ('vd', sw,msl, gtk.STATE_INSENSITIVE, gtk.ORIENTATION_VERTICAL),
        ):
        fn = 'scale-'+t
        if orient == gtk.ORIENTATION_HORIZONTAL:
            detail = "hscale"
        else:
            detail = "vscale"
        style.paint_slider(drawing_area.window, state, gtk.SHADOW_OUT,
                           None, drawing_area, detail, 0,0, w+2,h+2, orient)
        save_image(fn, w, h, 1, 1)

    w, h = msl, sw
    fn = 'scaletrough-h'
    style.paint_box(drawing_area.window, gtk.STATE_ACTIVE, gtk.SHADOW_IN,
                    None, scale, "trough", 0,0, w,h)
    save_image(fn, w, h)


    # arrow
    w = h = hscroll.style_get_property("stepper_size")
    #w = h = 15
    arrow_width = w / 2
    arrow_height = h / 2
    arrow_x = (w - arrow_width) / 2
    arrow_y = (h - arrow_height) / 2

    alloc = hscroll.get_allocation()
    x0 = alloc.x
    x1 = alloc.x+alloc.width-w
    alloc = vscroll.get_allocation()
    y0 = alloc.y
    y1 = alloc.y+alloc.height-h

    sn = gtk.STATE_NORMAL
    sp = gtk.STATE_PRELIGHT
    sa = gtk.STATE_ACTIVE
    si = gtk.STATE_INSENSITIVE

    for fn, x, y, state, shadow, arrow_type, widget in (
        ("arrowleft-n", x0, 0, sn, gtk.SHADOW_OUT, gtk.ARROW_LEFT, hscroll),
        ("arrowleft-a", x0, 0, sp, gtk.SHADOW_OUT, gtk.ARROW_LEFT, hscroll),
        ("arrowleft-p", x0, 0, sa, gtk.SHADOW_IN,  gtk.ARROW_LEFT, hscroll),
        ("arrowleft-d", x0, 0, si, gtk.SHADOW_OUT, gtk.ARROW_LEFT, hscroll),

        ("arrowright-n", x1, 0, sn, gtk.SHADOW_OUT, gtk.ARROW_RIGHT, hscroll),
        ("arrowright-a", x1, 0, sp, gtk.SHADOW_OUT, gtk.ARROW_RIGHT, hscroll),
        ("arrowright-p", x1, 0, sa, gtk.SHADOW_IN,  gtk.ARROW_RIGHT, hscroll),
        ("arrowright-d", x1, 0, si, gtk.SHADOW_OUT, gtk.ARROW_RIGHT, hscroll),

        ("arrowup-n", 0, y0, sn, gtk.SHADOW_OUT, gtk.ARROW_UP, vscroll),
        ("arrowup-a", 0, y0, sp, gtk.SHADOW_OUT, gtk.ARROW_UP, vscroll),
        ("arrowup-p", 0, y0, sa, gtk.SHADOW_IN,  gtk.ARROW_UP, vscroll),
        ("arrowup-d", 0, y0, si, gtk.SHADOW_OUT, gtk.ARROW_UP, vscroll),

        ("arrowdown-n", 0, y1, sn, gtk.SHADOW_OUT, gtk.ARROW_DOWN, vscroll),
        ("arrowdown-a", 0, y1, sp, gtk.SHADOW_OUT, gtk.ARROW_DOWN, vscroll),
        ("arrowdown-p", 0, y1, sa, gtk.SHADOW_IN,  gtk.ARROW_DOWN, vscroll),
        ("arrowdown-d", 0, y1, si, gtk.SHADOW_OUT, gtk.ARROW_DOWN, vscroll),
        ):
        if 0:
            detail = 'hscrollbar'
            if widget is vscroll:
                detail = 'vscrollbar'
        else:
            x, y = 0, 0
            detail = 'stepper'
            widget = drawing_area
        style.paint_box(drawing_area.window, state, shadow,
                        None, widget, detail, x,y, w,h)
        style.paint_arrow(drawing_area.window, state, shadow,
                          None, widget, detail, arrow_type, True,
                          x+arrow_x, y+arrow_y, arrow_width, arrow_height)
        save_image(fn, w, h, x, y)


    # combobox
    w, h = w, 24
    w, h = 16, 24
    alloc = hscroll.get_allocation()
    x1 = alloc.x+alloc.width-w
    arrow_width = w / 2
    arrow_height = h / 2
    arrow_x = (w - arrow_width) / 2
    arrow_y = (h - arrow_height) / 2
    detail = 'hscrollbar'
    widget = hscroll
    for fn, state, shadow, arrow_type in (
        ("comboarrow-n", gtk.STATE_NORMAL,     gtk.SHADOW_OUT, gtk.ARROW_DOWN),
        ("comboarrow-a", gtk.STATE_PRELIGHT,   gtk.SHADOW_OUT, gtk.ARROW_DOWN),
        ("comboarrow-p", gtk.STATE_ACTIVE,      gtk.SHADOW_IN, gtk.ARROW_DOWN),
        ("comboarrow-d", gtk.STATE_INSENSITIVE, gtk.SHADOW_IN, gtk.ARROW_DOWN),
        ):
        style.paint_box(drawing_area.window, state, shadow,
                        None, widget, detail, x1,0, w,h)
        style.paint_arrow(drawing_area.window, state, shadow,
                          None, drawing_area, "stepper", arrow_type, True,
                          x1+arrow_x, arrow_y, arrow_width, arrow_height)
        save_image(fn, w, h, x1, 0)

    w = 24
    for fn, state, shadow in (
        ("combo-rn", gtk.STATE_NORMAL,      gtk.SHADOW_OUT),
        ("combo-ra", gtk.STATE_PRELIGHT,    gtk.SHADOW_OUT),
        ("combo-rp", gtk.STATE_ACTIVE,      gtk.SHADOW_IN),
        ("combo-rd", gtk.STATE_INSENSITIVE, gtk.SHADOW_OUT),
        ):
        style.paint_box(drawing_area.window, state, shadow,
                        None, drawing_area, "button", 0,0, w+2,h)
        save_image(fn, w, h)

    style.paint_box(drawing_area.window, gtk.STATE_NORMAL, gtk.SHADOW_OUT,
                    None, drawing_area, "button", 0,0, w+2,h)
    d = 3
    style.paint_focus(drawing_area.window, gtk.STATE_NORMAL,
                      None, drawing_area, "button", d,d, w-2*d,h-2*d)
    save_image('combo-rf', w, h)

    style.paint_shadow(drawing_area.window, gtk.STATE_NORMAL, gtk.SHADOW_IN,
                       None, drawing_area, "entry", 0,0, w+2,h)
    save_image('combo-n', w, h)


    # checkbutton
    #define INDICATOR_SIZE     13
    #define INDICATOR_SPACING  2
    x, y = 2, 2
    w, h = 13, 13
    #w = h = checkbutton.style_get_property("indicator_size")
    for fn, state, shadow in (
        ("check-nc", gtk.STATE_NORMAL,      gtk.SHADOW_IN),
        ("check-nu", gtk.STATE_NORMAL,      gtk.SHADOW_OUT),
        ("check-ac", gtk.STATE_PRELIGHT,    gtk.SHADOW_IN),
        ("check-au", gtk.STATE_PRELIGHT,    gtk.SHADOW_OUT),
        ("check-pc", gtk.STATE_ACTIVE,      gtk.SHADOW_IN),
        ("check-pu", gtk.STATE_ACTIVE,      gtk.SHADOW_OUT),
        ("check-dc", gtk.STATE_INSENSITIVE, gtk.SHADOW_IN),
        ("check-du", gtk.STATE_INSENSITIVE, gtk.SHADOW_OUT),
        ):
##     style.paint_flat_box(drawing_area.window,
##                          gtk.STATE_PRELIGHT,
##                          gtk.SHADOW_ETCHED_OUT,
##                          gtk.gdk.Rectangle(0,0,w,h), drawing_area,
##                          "checkbutton", 0,0, w,h)

        style.paint_check(drawing_area.window, state, shadow,
                          None, drawing_area, "checkbutton", x,y, w,h)
        save_image(fn, w+2*x, h+2*y)


    # radiobutton
    for fn, state, shadow in (
        ("radio-nc", gtk.STATE_NORMAL,      gtk.SHADOW_IN),
        ("radio-nu", gtk.STATE_NORMAL,      gtk.SHADOW_OUT),
        ("radio-ac", gtk.STATE_PRELIGHT,    gtk.SHADOW_IN),
        ("radio-au", gtk.STATE_PRELIGHT,    gtk.SHADOW_OUT),
        ("radio-pc", gtk.STATE_ACTIVE,      gtk.SHADOW_IN),
        ("radio-pu", gtk.STATE_ACTIVE,      gtk.SHADOW_OUT),
        ("radio-dc", gtk.STATE_INSENSITIVE, gtk.SHADOW_IN),
        ("radio-du", gtk.STATE_INSENSITIVE, gtk.SHADOW_OUT),
        ):
##     style.paint_flat_box(drawing_area.window,
##                          gtk.STATE_PRELIGHT,
##                          gtk.SHADOW_ETCHED_OUT,
##                          gtk.gdk.Rectangle(0,0,w,h), drawing_area,
##                          "checkbutton", 0,0, w,h)
        style.paint_option(drawing_area.window, state, shadow,
                           None, drawing_area, "radiobutton", x,y, w,h)
        save_image(fn, w+2*x, h+2*y)


    # notebook
    w, h = 28, 22
    state = gtk.STATE_NORMAL
    shadow = gtk.SHADOW_OUT

    for fn, gap_h, state in (
        ("tab-n", 0, gtk.STATE_NORMAL),
        ("tab-a", 2, gtk.STATE_ACTIVE),
        ):
##         style.paint_box_gap(drawing_area.window, state, shadow,
##                             gtk.gdk.Rectangle(0,0,w,gap_h), drawing_area,
##                             "notebook", 0,0, w,gap_h, gtk.POS_TOP, 0, w)
        y = gap_h
        hh = h - y
        style.paint_extension(drawing_area.window, state, gtk.SHADOW_OUT,
                              None, drawing_area, "tab",
                              0,y, w,hh, gtk.POS_BOTTOM)
        save_image(fn, w, h+2)



    print 'done'

    gtk.main_quit()


def pack(w, row, col):
    table.attach(w,
                 col, col+1,              row, row+1,
                 gtk.EXPAND | gtk.FILL,   gtk.EXPAND | gtk.FILL,
                 0,                       0)

win = gtk.Window()
win.connect("destroy", gtk.main_quit)

table = gtk.Table()
win.add(table)

row, col = 0, 0

drawing_area = gtk.DrawingArea()
#drawing_area.set_size_request(100, 100)
pack(drawing_area, row, col)
row += 1

vscroll = gtk.VScrollbar()
pack(vscroll, 0, 1)

hscroll = gtk.HScrollbar()
pack(hscroll, row, col)
row += 1


notebook = gtk.Notebook()
label = gtk.Label("Label")
notebook.append_page(label)
label = gtk.Label("Label")
notebook.append_page(label)
pack(notebook, row, col)
row += 1

button = gtk.Button("Button")
pack(button, row, col)
row += 1

checkbutton = gtk.CheckButton("CheckButton")
pack(checkbutton, row, col)
row += 1

progress = gtk.ProgressBar()
pack(progress, row, col)
row += 1

scale = gtk.HScale()
pack(scale, row, col)
row += 1

entry = gtk.Entry()
pack(entry, row, col)
row += 1

togglebutton = gtk.ToggleButton()
pack(togglebutton, row, col)
togglebutton.set_active(True)
row += 1


drawing_area.connect("expose-event", save_callback)
#gobject.timeout_add(2000, save_callback)

win.show_all()
#drawing_area.modify_bg(gtk.STATE_NORMAL, gtk.gdk.color_parse('red'))

gtk.main()



