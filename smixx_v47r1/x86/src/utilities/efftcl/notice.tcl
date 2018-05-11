# ----------------------------------------------------------------------
#  EXAMPLE: simple notice dialog
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

option add *Notice*dismiss.text "Dismiss" widgetDefault

proc notice_show {mesg {icon "info"}} {
    set top [dialog_create Notice]

    set x [expr [winfo rootx .]+50]
    set y [expr [winfo rooty .]+50]
    wm geometry $top "+$x+$y"

    set info [dialog_info $top]
    label $info.icon -bitmap $icon
    pack $info.icon -side left -padx 8 -pady 8
    label $info.mesg -text $mesg -wraplength 4i
    pack $info.mesg -side right -expand yes -fill both -padx 8 -pady 8

    set cntls [dialog_controls $top]
    button $cntls.dismiss -command "destroy $top"
    pack $cntls.dismiss -pady 4

    focus $cntls.dismiss
    return $top
}
