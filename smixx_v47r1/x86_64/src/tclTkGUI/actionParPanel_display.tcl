#====================================================================
#
#                           Boda Franek    Dec 2005
# Copyright Information:
#       Copyright (C) 2005 CCLRC. All Rights Reserved
#
#  $Id: actionParPanel_display.tcl,v 1.15 2010/06/24 12:21:01 franek Exp $
#====================================================================
#===============================================================================
proc actionParPanel_display {object action parameters} {
# The panel is modal and has 3 sections :
#  1 - Heading
#  2 - parameters
#  3 - controls  OK and Cancel buttons
#===============================================================================
	global objectId
	global actionParPanelid actionParPanelidAnswer
	global actionParPanelAction actionParPanelObject
    
	if { [info exists actionParPanelid] } {
		bell
		wm deiconify $actionParPanelid
		raise $actionParPanelid
		update idletasks
		return
	}
    
	set actionParPanelAction $action
	set actionParPanelObject $object

# create the Panel 
	set actionParPanelid [dialog_create "Action Parameters"]
    
# get the window path for the information part of the dialog   
	set info [dialog_info $actionParPanelid]

# position it in the right place relative to the parent window
#	set parentWin $objectId($object)
        set parentWin .
	set parentPosX [winfo rootx $parentWin]
	set parentPosY [winfo rooty $parentWin]
	
	set x [expr $parentPosX+20]
	set y [expr $parentPosY+20]
    
#    puts stdout "Placing $actionParPanelid  at x $x  y $y"
	wm geometry $actionParPanelid "+$x+$y"  
	
	set infoOnlyId [actionParInfoPanel $info.settings $object $action $parameters]
	pack $infoOnlyId

    set cntls [dialog_controls $actionParPanelid]
    
	button $cntls.ok -text ok -command { set actionParPanelidAnswer 1}
	pack $cntls.ok -side left -expand yes

    button $cntls.cancel -text Cancel -command { set actionParPanelidAnswer 0}
    pack $cntls.cancel -side left -expand yes

    focus $cntls.ok

    wm protocol $actionParPanelid WM_DELETE_WINDOW "$cntls.cancel invoke"

	while { "1" == "1" } {
		dialog_wait $actionParPanelid actionParPanelidAnswer
	
		if { $actionParPanelidAnswer == "0" } {
			break;
		}
		set flg [actionParPanelOK]
		if { $flg == 1 } {
			break;
		}
	}
	actionParPanel_destroy

    return 
}
#===========================================================================
proc actionParPanel_destroy {} {
    global actionParPanelid

    destroy $actionParPanelid
    unset actionParPanelid

    ParameterGroups::group_delete 1
    return
}
#===========================================================================
proc actionParHeadingPanel_create {win object action} {

	frame $win        
        	label $win.title -text "object: $object  action: $action" \
			-relief flat
        	pack $win.title
	return $win
}
#===========================================================================
proc actionParOnlyPanel_create {win parameters} {

	frame $win
	        actionParameterGroup_setup $parameters

        	ParameterGroups::panel_create $win.parameters  1
        	pack $win.parameters
	return $win
}
#===========================================================================
proc actionParInfoPanel  {win object action parameters}	{
	
	frame $win
		set head [actionParHeadingPanel_create $win.head $object $action]
		set parOnly [actionParOnlyPanel_create $win.par $parameters]
		
		pack $head
	        
		frame $win.sep -height 2 -borderwidth 1 -relief sunken
        	pack $win.sep -fill x -pady 4
		
		pack $parOnly
	return $win
}		
#==============================================================================
proc actionParPanelOK {} {
    global actionParPanelid actionParPanelidAnswer
    global actionParPanelObject actionParPanelAction

    if { [ParameterGroups::allValExists 1] == "0" } {
        tk_messageBox -message "All parameters have to have values assigned"
        return 0
    }

    set parameters [ ParameterGroups::getGroupParValues 1]

# check the par values so that correction can be made before par panel goes
	set errmsg ""
	if { $parameters != "none" } {
		foreach par $parameters {
			set parname [lindex $par 0]
			set partype [lindex $par 1]
			set parval  [lindex $par 2]
			if { $partype == "S" } {
				set res [ convertEASCIItoHEX $parval ]
				if { [lindex $res 0] != "" } {
					set errmsg "Parameter ${parname} :\n \
					[lindex $res 0] "
				}   
			} elseif { $partype == "I" } {
				if { [string is integer $parval] != 1 } {
					set errmsg "Parameter ${parname} :\n \
					must be integer"
				}
			} elseif { $partype == "F" } {
				if { [string is double $parval] != 1 \
				|| [string first "." $parval] <0 \
				&& [string first "e" $parval] <0 \
				&& [string first "E" $parval] <0} {
					set errmsg "Parameter ${parname} :\n \
					must be floating point number"
				}
			}

		}
	} 
 
	if { $errmsg != "" } {
		set ans [tk_messageBox -icon error -type yesno -default no \
		-message "$errmsg \nDo you need help ?"]
		if { $ans == "yes" } {
			set helpId [parameterHelp_show]
			grab release $actionParPanelid
			focus $helpId
			grab set $helpId
			dialog_safeguard $helpId
			tkwait window $helpId
			focus $actionParPanelid
			grab set $actionParPanelid
		}	
		
		return 0
	}

	
	dispatchAction $actionParPanelObject $actionParPanelAction $parameters

	return 1

}
#===================================================================================

#======================================================================
proc actionParameterGroup_setup {parameters} {

# There is only 1 action param panel so I choose parGroupId to be 1

	set parGroupId 1
	
    foreach parm $parameters {	
	set name [lindex $parm 0]
	set type [lindex $parm 1]

        set id [string tolower $name]    

	ParameterGroups::parameter_create $parGroupId $id $name $type -1
	
	if { [llength $parm] == 3 } {
		set val [lindex $parm 2]        
		ParameterGroups::parameter_set $parGroupId $id $val
	}

    }
}   
