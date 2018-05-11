# ----------------------------------------------------------------------
#  EXAMPLE: simple confirmation dialog
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================
# modified by B.Franek on 10-Jun-2002. Adding access to the window

option add *Confirm*icon.bitmap questhead widgetDefault
option add *Confirm*mesg.wrapLength 4i widgetDefault

proc confirm_ask {mesg {ok "OK"} {cancel "Cancel"}} {
    global confirmStatus
#
#--- B.Franek 10-Jun-2002    
    global confirmWinId
    if {[info exists confirmWinId]} {
        puts stdout \
         "can not pop up another confirm window untill the curent is destroyed"
        return
    }
#----

    set top [dialog_create Confirm]
    set confirmWinId $top

    set info [dialog_info $top]
    label $info.icon
    pack $info.icon -side left -padx 8 -pady 8
    label $info.mesg -text $mesg
    pack $info.mesg -side right -expand yes -fill both -padx 8 -pady 8

    set cntls [dialog_controls $top]
    button $cntls.ok -text $ok -command {set confirmStatus 1}
#---- B. Franek   21 Dec 2004
    if { $ok != "nil" } {
        pack $cntls.ok -side left -expand yes
    }
    button $cntls.cancel -text $cancel -command {set confirmStatus 0}
#---- B. Franek   06 Dec 2004
    if { $cancel != "nil" } {
        pack $cntls.cancel -side left -expand yes
    }
    focus $cntls.ok

    wm protocol $top WM_DELETE_WINDOW "$cntls.cancel invoke"

    dialog_wait $top confirmStatus
    destroy $top
#---- B.Franek 10-Jun-2002
    unset confirmWinId
#----
    return $confirmStatus
}
#---- B.Franek 10-Jun-2002
#============================================================================
proc confirm_destroy {} {
    global confirmStatus
    set confirmStatus destroyed
    return
}
#============================================================================
proc confirm_controls {} {
    global confirmWinId
    return [dialog_controls $confirmWinId]
}
#============================================================================
proc confirm_info {} {
    global confirmWinId
    return [dialog_info $confirmWinId]
}
#----
   