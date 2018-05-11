#==============================================================================
# Generic SMI GUI
#
#                                  Boda Franek    December 2006
# Copyright Information:
#      Copyright (C) 2005 - 2006 CCLRC.   All Rights Reserved
#
#    $Id: mainPanel.tcl,v 1.13 2007/02/16 10:48:01 franek Exp $
#==============================================================================
proc mainPanel_create {win} {
	
	frame $win
		set titId [title_create $win.tit]
		pack $titId
		
		set buttonsId [mainPanelButtons_create $win.buts]
		
		frame $win.sep -height 2 -borderwidth 1 -relief sunken
		pack $win.sep -fill x -pady 4

		label $win.butslab -text "Object Selection" -relief flat -pady 4
		pack $win.butslab		
		pack $buttonsId 
	return $win
}
#============================================================================
proc title_create {win} {
	global GUIdir
	global hel32font

	frame $win
		set logoGif $GUIdir/SMIXXLogo.gif
		set im [image create photo -file $logoGif]
		label $win.pic -image $im -relief flat
		label $win.gui -text "Generic GUI" -font hel32font  -padx 20 -relief flat
		pack $win.pic $win.gui -side left
	return $win
}
#=========================================================================================
proc mainPanelButtons_create {win} {
	
	frame $win	
		
		set labentry $win.labentry
		frame $labentry
			label $labentry.l -text "Enter object manually:" -relief flat
			entry $labentry.e -textvariable entObject -bg white
			pack $labentry.l $labentry.e -side top
			bind $labentry.e <Return> {addObject $entObject}
			bind $labentry.e <FocusIn> "$labentry.e configure -bg azure2"
			bind $labentry.e <FocusOut> "$labentry.e configure -bg white"

		button $win.addObjects -text "Browse" -command browseForObjects
		pack $labentry -side left -padx 20
		pack $win.addObjects -side left -padx 20
		
		button $win.fromFile -text "From File" -command retrieveConfiguration
		pack $win.fromFile -side left -padx 20
	return $win
}
#===========================================================================================
proc retrieveConfiguration {} {
	
	set filename [tk_getOpenFile -title "Objects from Configuration File"]
	puts stdout $filename
	if { $filename == "" } {
		return
	}
	if [catch {open $filename r} chanId] {
		tk_messageBox -message "cannot open $filename : $chanId"
		return
	}
	
	while { [ gets $chanId objName ] >= 0} {
		addObject $objName	
	}
	
	close $chanId
	return
}
#===========================================================================================
proc browseForObjects {} {
	set selection [ objectSelectionPanel_show ]
	
	if { $selection != "" } { 
		foreach obj $selection {
			addObject $obj
		}
	}

	return
}
	
