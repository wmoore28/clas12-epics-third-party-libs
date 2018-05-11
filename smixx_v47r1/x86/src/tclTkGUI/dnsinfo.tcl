#==============================================================================
#
#                                  Boda Franek    December 2006
# Copyright Information:
#      Copyright (C) 2005-2006 CCLRC.   All Rights Reserved
#
#    $Id: dnsinfo.tcl,v 1.4 2007/01/19 14:03:14 franek Exp $
#==============================================================================
proc dnsinfo_create {win} {
	global hel10font
	
	frame $win -relief sunken -borderwidth 1 -background gray70
		label $win.l -text "Connecting to DNS"
		pack $win.l -fill x
		
	return $win
}
proc dnsinfo_update {win mess} {
	$win.l configure -text $mess
	return
}
