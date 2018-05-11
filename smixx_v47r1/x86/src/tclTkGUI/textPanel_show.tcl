#==============================================================================
#
#                                  Boda Franek    July 2007
# Copyright Information:
#      Copyright (C) 2005-2007 CCLRC.   All Rights Reserved
#
#    $Id: textPanel_show.tcl,v 1.1 2007/07/19 10:59:45 franek Exp $
#==============================================================================
#==========================================================================
proc textPanel_show {title} {
#----------------------------------------------------------------
# Creates a top level dialog type panel.
# The 'info' part of the panel contains 'text Box' see below
# The 'control' part has just one 'Dismiss' button
# at the moment the argument 'title' is not used
#----------------------------------------------------------------
	set top [dialog_create TextPanel]
	set info [dialog_info $top]
	set cntls [dialog_controls $top]
	
	set textBoxId [textBox_create $info.textBox ]
	pack $textBoxId -fill both -expand true
	
	button $cntls.dismiss -text Dismiss -command "destroy $top"
	pack $cntls.dismiss
	raise $top .
	return $top
}
#=========================================================================
proc textBox_create {win} {
#-------------------------------------------------------------------------
# Creates 'text Box' widget. This consists 'text' widget with a vertical
# scroll bar on the right
#-------------------------------------------------------------------------
	frame $win 
		set txt [text $win.t -setgrid true -wrap word \
		-width 42 -height 14 \
		-yscrollcommand "$win.sy set"]
		
		scrollbar $win.sy -orient vert -command "$win.t yview"
		pack $win.sy -side right -fill y
		pack $txt -side left -fill both -expand true
		
	return $win
}
#=======================================================================
proc textPanel_text {top} {
#-------------------------------------------------------------------------
# given the window id of a textPanel, it returns widget id of the text
# widget. This can be then used to manipulate the text widget, like
# inputing text, changing background color etc.
#-------------------------------------------------------------------------
	return [dialog_info $top].textBox.t
}
#=========================================================================	
