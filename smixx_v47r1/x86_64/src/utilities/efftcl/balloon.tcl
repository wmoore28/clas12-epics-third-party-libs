# ----------------------------------------------------------------------
#  EXAMPLE: use "wm" commands to manage a balloon help window
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

option add *Balloonhelp*background white widgetDefault
option add *Balloonhelp*foreground black widgetDefault
option add *Balloonhelp.info.wrapLength 3i widgetDefault
option add *Balloonhelp.info.justify left widgetDefault
option add *Balloonhelp.info.font \
    -*-lucida-medium-r-normal-sans-*-120-* widgetDefault

toplevel .balloonhelp -class Balloonhelp \
    -background black -borderwidth 1 -relief flat
bind .balloonhelp <Button-1> {balloonhelp_cancel}
bind .balloonhelp <Button-2> {balloonhelp_cancel}    
bind .balloonhelp <Button-3> {balloonhelp_cancel}

#label .balloonhelp.arrow -anchor nw \
#    -bitmap @[file join $env(EFFTCL_LIBRARY) images arrow.xbm]
#pack .balloonhelp.arrow -side left -fill y

label .balloonhelp.info -background white
pack .balloonhelp.info -side left -fill y 

wm overrideredirect .balloonhelp 1
wm withdraw .balloonhelp

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_for <win> <mesg>
#
#  Adds balloon help to the widget named <win>.  Whenever the mouse
#  pointer enters this widget and rests within it for a short delay,
#  a balloon help window will automatically appear showing the
#  help <mesg>.
# ----------------------------------------------------------------------
proc balloonhelp_for {win mesg} {
    global bhInfo
    set bhInfo($win) $mesg

#    bind $win <Enter> {balloonhelp_pending %W}
    bind $win <Button-3> {balloonhelp_pending %W}
#    bind $win <Leave> {balloonhelp_cancel}
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_control <state>
#
#  Turns balloon help on/off for the entire application.
# ----------------------------------------------------------------------
set bhInfo(active) 1

proc balloonhelp_control {state} {
    global bhInfo

    if {$state} {
        set bhInfo(active) 1
    } else {
        balloonhelp_cancel
        set bhInfo(active) 0
    }
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_pending <win>
#
#  Used internally to mark the point in time when a widget is first
#  touched.  Sets up an "after" event so that balloon help will appear
#  if the mouse remains within the current window.
# ----------------------------------------------------------------------
proc balloonhelp_pending {win} {
    global bhInfo

    balloonhelp_cancel
    set bhInfo(pending) [after 200 [list balloonhelp_show $win]]
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_cancel
#
#  Used internally to mark the point in time when the mouse pointer
#  leaves a widget.  Cancels any pending balloon help requested earlier
#  and hides the balloon help window, in case it is visible.
# ----------------------------------------------------------------------
proc balloonhelp_cancel {} {
    global bhInfo

    if {[info exists bhInfo(pending)]} {
        after cancel $bhInfo(pending)
        unset bhInfo(pending)
    }
    wm withdraw .balloonhelp
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_show <win>
#
#  Used internally to display the balloon help window for the
#  specified <win>.
# ----------------------------------------------------------------------
proc balloonhelp_show {win} {
    global bhInfo

    if {$bhInfo(active)} {
        .balloonhelp.info configure -text $bhInfo($win)

        set x [expr [winfo rootx $win]+10]
        set y [expr [winfo rooty $win]+[winfo height $win]]
        wm geometry .balloonhelp +$x+$y
        wm deiconify .balloonhelp
        raise .balloonhelp
    }
    unset bhInfo(pending)
}
