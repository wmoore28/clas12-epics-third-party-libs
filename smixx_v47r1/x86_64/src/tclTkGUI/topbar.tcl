#==============================================================================
#
#                                  Boda Franek    December 2006
# Copyright Information:
#      Copyright (C) 2005-2006 CCLRC.   All Rights Reserved
#
#    $Id: topbar.tcl,v 1.7 2010/09/01 13:58:41 franek Exp $
#==============================================================================
proc topbar_create {win} {
	global hel10font
	
	frame $win -relief flat -relief ridge -borderwidth 2 -background gray70
		set fileId [fileButton_create $win.file]
		pack $fileId -side left -padx 8
		
		set toolsId [toolsButton_create $win.tools]
		pack $toolsId -side left -padx 8

	return $win
}
#==============================================================================
proc fileButton_create {win} {
	global hel10font
	
	menubutton $win -text "File" -relief flat \
	           -background gray70 -menu $win.menu
	
	menu $win.menu
	$win.menu add command -label "Save the configuration" \
				-command saveConfiguration
	$win.menu add command -label "Exit" -command exitProcedure
	return $win
}
#==============================================================================
proc toolsButton_create {win} {
	global hel10font
	
	menubutton $win -text "Tools" -relief flat \
	           -background gray70 -menu $win.menu
	
	menu $win.menu
	$win.menu add command -label "Object Sets Panel" \
				-command objectSetDisplayPanel_show
	return $win
}
#=============================================================================
proc saveConfiguration {} {
	global objectsOnCanvas
	
	set filename [tk_getSaveFile -title "Configuration File"]
	puts stdout $filename
	if { $filename == "" } {
		return
	}
	if [catch {open $filename w} chanId] {
		tk_messageBox -message "cannot open $filename : $chanId"
		return
	}
	
	set i 0
	while { "1" == "1" } {
		set elem [lindex $objectsOnCanvas $i]
		if { $elem == "" } {
			break
		}
		puts $chanId $elem
		incr i
	}
	
	close $chanId
	return
}
#=============================================================================
proc exitProcedure {} {

	stopMonitoring
	return
}
