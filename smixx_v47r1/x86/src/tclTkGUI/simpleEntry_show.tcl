#====================================================================
#
#                           Boda Franek    Dec 2006
# Copyright Information:
#       Copyright (C) 2005-2006 CCLRC. All Rights Reserved
#
#  $Id: simpleEntry_show.tcl,v 1.1 2006/12/18 06:50:37 franek Exp $
#====================================================================
#===============================================================================
proc simpleEntry_show {text {ok "OK"} {cancel "Cancel"} } {
# The panel is modal and has 2 sections :
#  1 - Entry
#  3 - controls  OK and Cancel buttons
#===============================================================================
	global simpleEntryWinId simpleEntryAnswer simpleEntryResult
    
	if { [info exists simpleEntryWinId] } {
		bell
		wm deiconify $simpleEntryWinId
		raise $simpleEntryWinId
		update idletasks
		return
}
# create the Window 
	set simpleEntryWinId [dialog_create "SimpleEntry"]
    
# get the window path for the information part of the dialog   
	set info [dialog_info $simpleEntryWinId]

# position it in the right place relative to the parent window
	set parentWin .
 
	set parentPosX [winfo rootx $parentWin]
	set parentPosY [winfo rooty $parentWin]
	
	set x [expr $parentPosX+20]
	set y [expr $parentPosY+20]
    
	wm geometry $simpleEntryWinId "+$x+$y"  
	
	set lab [label $info.lab -text $text -bg Yellow]
	set ent [entry $info.ent -textvariable simpleEntryResult ]
	
	pack $lab $ent -pady 5

	set cntls [dialog_controls $simpleEntryWinId]
	button $cntls.ok -text $ok -command "executeOK "

	pack $cntls.ok -side left -expand yes

	button $cntls.cancel -text $cancel -command { set simpleEntryAnswer 0}
	pack $cntls.cancel -side left -expand yes

	bind $info.ent <Return> "$cntls.ok invoke"
	focus $info.ent

	wm protocol $simpleEntryWinId WM_DELETE_WINDOW "$cntls.cancel invoke"

	set simpleEntryResult {}
	dialog_wait $simpleEntryWinId simpleEntryAnswer
	
#	puts stdout $simpleEntryAnswer
#	puts stdout $simpleEntryResult
	simpleEntry_destroy

	if { $simpleEntryAnswer == 0 } {
		return ""
	} else {
		return $simpleEntryResult
	} 
     
}
#===========================================================================
proc simpleEntry_destroy {} {
    global simpleEntryWinId

    destroy $simpleEntryWinId
    unset simpleEntryWinId

    return
}
#==============================================================================
proc executeOK {} {
    global simpleEntryWinId simpleEntryAnswer
    
    set simpleEntryAnswer 1


    return

}
#===================================================================================
