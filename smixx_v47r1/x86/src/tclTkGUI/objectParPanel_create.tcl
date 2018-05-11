#====================================================================
#
#                           Boda Franek    Nov 2006
# Copyright Information:
#       Copyright (C) 2005 CCLRC. All Rights Reserved
#
#  $Id: objectParPanel_create.tcl,v 1.12 2007/08/03 14:03:08 franek Exp $
#====================================================================
#===============================================================================
proc objectParPanel_create {win parameters title} {
# The panel has 3 sections :
#  1 - Heading
#  2 - parameters
#  3 - controls  OK and Cancel buttons
#===============================================================================
	set parGroupId $win

	frame $win    
		set info [objectParInfoPanel $win.info $parGroupId $parameters $title]
		pack $info

		set cntls $win.dismiss
		button $cntls -text Dismiss -command "objectParPanel_destroy $win"

		pack $cntls -side left -expand yes

	return $win 
}
#===========================================================================
proc objectParPanel_display {win parameters title} {

	if { [winfo exists $win] } {
		bell
		wm deiconify $win
		raise $win
		update idletasks
		return
	}
	
	set parGroupId $win	
	toplevel $win
		set info [objectParInfoPanel $win.info $parGroupId $parameters $title]
		pack $info

		set cntls $win.dismiss
		button $cntls -text Dismiss -command "objectParPanel_destroy $win"

		pack $cntls -side left -expand yes
		
		wm protocol $win WM_DELETE_WINDOW "$cntls invoke"
		
	return $win
}	
#===========================================================================
proc objectParPanel_destroy {win} {
	
	destroy $win
	
	set parGroupId $win
	ParameterGroups::group_delete $parGroupId
	return
}
#===========================================================================
proc objectParOnlyPanel_create {win parGroupId parameters} {

	frame $win
	        objectParameterGroup_setup $parGroupId $parameters

        	ParameterGroups::panel_create $win.parameters  $parGroupId
        	pack $win.parameters
	return $win
}
#===========================================================================
proc objectParInfoPanel  {win parGroupId parameters title}	{
	

	frame $win

		set parOnly [objectParOnlyPanel_create $win.parOnly $parGroupId $parameters]

		frame $win.sep1 -height 2 -borderwidth 1 -relief sunken
        	pack $win.sep1 -fill x -pady 4
		
		label $win.title -text $title -relief flat
        	pack $win.title
	        
		frame $win.sep2 -height 2 -borderwidth 1 -relief sunken
        	pack $win.sep2 -fill x -pady 4
		
		pack $parOnly
		
		frame $win.sep3 -height 2 -borderwidth 1 -relief sunken
        	pack $win.sep3 -fill x -pady 4

	return $win
}		
#======================================================================
proc objectParameterGroup_setup {parGroupId parameters} {

    foreach parm $parameters {	
	set name [lindex $parm 0]
	set type [lindex $parm 1]
	
        set id [string tolower $name]    

	ParameterGroups::parameter_create $parGroupId $id $name $type 9
	ParameterGroups::parameter_setInhibit $parGroupId $id
 	
	if { [llength $parm] == 3 } {
		set val [lindex $parm 2]        
		ParameterGroups::parameter_set $parGroupId $id $val
	}
    }
}   
#======================================================================
proc objectParameterGroup_update {parGroupId parameters} {

	foreach parm $parameters {	
		set name [lindex $parm 0]
		set type [lindex $parm 1]

		set id [string tolower $name]    
 	
		if { [llength $parm] == 3 } {
			set val [lindex $parm 2]        
			ParameterGroups::parameter_set $parGroupId $id $val
		}

	}
	return
}   
