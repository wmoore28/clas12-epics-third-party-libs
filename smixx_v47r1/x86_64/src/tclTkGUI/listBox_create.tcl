#==============================================================================
#
#                                  Boda Franek    December 2006
# Copyright Information:
#      Copyright (C) 2005-2006 CCLRC.   All Rights Reserved
#
#    $Id: listBox_create.tcl,v 1.5 2007/02/18 08:07:55 franek Exp $
#==============================================================================
#===================================================================================
proc listBox_create {win title} {
     
	frame $win -borderwidth 1
		set labelId [label $win.label -text $title]
		pack $labelId
		
		set listBoxId $win.listb
		set scrollId $win.scroll
		
		listbox $listBoxId -yscroll "$scrollId set" -setgrid 1 -height 5 -width 20 -bg white
		scrollbar $scrollId -command "$listBoxId yview"

		pack $scrollId -side right -fill y -pady 5 

		pack $listBoxId -side left -fill both -expand 1 -pady 5
	return $win
}
#=============================================================================
proc listBox_input {win} {
	return "$win.listb"
}
