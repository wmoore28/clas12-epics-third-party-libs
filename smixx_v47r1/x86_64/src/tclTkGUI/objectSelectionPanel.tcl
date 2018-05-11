
#===============================================================================
proc getDomainsForList {} {
	set domainList [exec getDomains]
		
	set domains {}
	
	foreach item $domainList {
		set splititem [split $item /]
		set dom [lindex $splititem 0]
		lappend domains $dom
		set node($dom) [lindex $splititem 1]
	}
	set domainsSorted [lsort $domains]
	return $domainsSorted
}
#==============================================================================
proc getObjectsForList {domain} {
	set domobjects [ exec getDomainObjects $domain ]
	set domobjectsSorted [lsort $domobjects]
	return $domobjectsSorted
}
proc selectAll {win} {
}
#===============================================================================
proc objectSelectionLists_create { win } {
	# The procedure creates three labelled and scrollabel lists side by side
	global GUIdir
	
	frame $win
		# Domains list ------------------------------------------------------
		set domainsLabel [label $win.domainsLabel -text "Visible Domains"]
		set  domains [Scrolled_Listbox $win.domains \
			-width 20 -height 10 ]

		# Domain objects list -----------------------------------------------
		set objectsLabel [label $win.objectsLabel -text "Domain Objects"]
		set objects [Scrolled_Listbox $win.objects \
			-width 20 -height 10 -selectmode extended ]
#			-width 20 -height 10 -selectmode extended -activestyle none]
# older versions of Tcl/Tk do not support option '-activestyle'

		# Selecting a domain will get the domain's objects and display them in objects
		bind $domains <ButtonRelease-1> \
			[list showDomainObjects %W $objects]

		# Cntr-a will select all thw objects of the domain
		bind $objects <Control-Key-a> \
			[list %W selection set 0 end]

		# Selected objects list ----------------------------------------------
		set selObjectsLabel [label $win.selObjectsLabel -text "Selected Objects"]
		set selObjects [Scrolled_Listbox $win.selObjects \
			-width 20 -height 10 -selectmode extended]

		# Selecting objects will remove them from the list
		bind $selObjects <ButtonRelease-3> \
			[list ListDeleteSel %W ]

		# Right arrow button that will be placed in front of the selected objects list
		if { [lsearch [image names] "::img::rightArrow"] == "-1" } {
			set arrow [image create photo ::img::rightArrow -file $GUIdir/rightArrow.gif]
		} else {
			set arrow ::img::rightArrow
		}
		set selButton [button $win.selBut -image $arrow \
			 -command [list transferSelectedItems $objects $selObjects] ]

		set upDownButtons [upDownButtons_create $win.upDown $selObjects]

		# place the elements on the frame
		grid $win.domainsLabel -row 0 -column 0
		grid $win.domains -row 1 -column 0

		grid $win.objectsLabel -row 0 -column 1
		grid $win.objects -row 1 -column 1

		grid $selButton -row 1 -column 2 -padx 10

		grid $win.selObjectsLabel -row 0 -column 3
		grid $win.selObjects -row 1 -column 3

		grid $upDownButtons -row 1 -column 4 -sticky ns

		# Insert all the domains
		set values [getDomainsForList]
		foreach x $values {
			$domains insert end $x
		}

	return $win
}
#==================================================================================
proc upDownButtons_create {win selObjects} {
	global GUIdir
	
	frame $win

		# Up arrow button that will be placed behind of the selected objects list
		if { [lsearch [image names] "::img::upArrow"] == "-1" } {
			set uparrow [image create photo ::img::upArrow -file $GUIdir/upArrow.gif]
		} else {
			set uparrow ::img::upArrow
		}
		set upButton [button $win.upBut -image $uparrow \
			 -command [list moveSelectedItemsUp $selObjects] ]

		# down arrow button that will be placed behind of the selected objects list
		if { [lsearch [image names] "::img::downArrow"] == "-1" } {
			set downarrow [image create photo ::img::downArrow -file $GUIdir/downArrow.gif]
		} else {
			set downarrow ::img::downArrow
		}
		set downButton [button $win.downBut -image $downarrow \
			 -command [list moveSelectedItemsDown $selObjects] ]
		pack $upButton -anchor n -expand true
		pack $downButton -anchor s -expand true
	return $win
}		
#==================================================================================
proc moveSelectedItemsUp {win} {
	# the items selected
	set selection [$win curselection]
	
	if { $selection == "" } {
		bell
		tk_messageBox -message "Select something!" -parent $win
		return
	}

	set first [lindex $selection 0]
	set last  [lindex $selection end]
#tk_messageBox -message "$first  $last"

	if { $first == "0" } {
		bell
		return
	}

	set itemabove [$win get [expr $first - 1]]
#tk_messageBox -message $itemabove
	$win insert [expr $last + 1] $itemabove
	$win delete [expr $first - 1]
	
}
#==================================================================================
proc moveSelectedItemsDown {win} {
	# the items selected
	set selection [$win curselection]

	if { $selection == "" } {
		bell
		tk_messageBox -message "Select something!" -parent $win
		return
	}


	set first [lindex $selection 0]
	set last  [lindex $selection end]
#tk_messageBox -message "$first  $last  [$win size]"

	if { $last >= [expr [$win size] - 1] } {
		bell
		return
	}

	set itembellow [$win get [expr $last + 1]]
#tk_messageBox -message $itembellow
	$win delete [expr $last + 1]
	$win insert [expr $first] $itembellow

	$win see [expr $last + 1]	
}
#================================================================================
proc objectSelectionLists_selection {win} {
	# the procedure returns the selected objects in a list
	set src $win.selObjects.list

	return [$src get 0 end]
}
#================================================================================
proc showDomainObjects {src dst} {
	# The procedure will find out the list of the objects for the selected domain and
	# displays them in the appropriate list

	#First clear the previous object list
	$dst delete 0 end
	update idletasks

	# Find which domain was selected
	set i [$src curselection]
	set domain [$src get $i]

	# Now get the domain objects
	set objects [getObjectsForList $domain]
	foreach obj $objects {
		$dst insert end $obj
	}
	focus $dst
}
#================================================================================
proc transferSelectedItems {src dst} {
	# the procedure will transfer selected items from $src list to $dst list
	# so far transferred
	set all [$dst get 0 end]
	
	set selection [$src curselection]
	if { $selection == "" } {
		bell
		tk_messageBox -message "Select something!" -parent $src
		return
	}
	
	foreach i  $selection {
		set sel [$src get $i]
		# make sure that the item has not been already selected
		if { [lsearch $all $sel] == "-1" } {	
			$dst insert end [$src get $i]
		}
	}

	set size [$dst size]
	$dst see $size
	update idletasks
}
#================================================================================
proc ListDeleteSel {w} {
	foreach i [lsort -integer -decreasing [$w curselection]] {
		$w delete $i
	}
}
#=================================================================================
proc objectSelectionPanel_show {} {
	global objectSelectionPanelPrompt 

	set top [dialog_create ObjectSelectionPanel]
		set info [dialog_info $top]
		set cntls [dialog_controls $top]
		set win $top

		set listBoxesId [objectSelectionLists_create $info.listBoxes ]

		pack $listBoxesId -expand true -fill both	

		button $cntls.ok -text OK -command {set objectSelectionPanelPrompt 1}
		pack $cntls.ok -side left -expand yes

		button $cntls.cancel -text Cancel -command {set objectSelectionPanelPrompt 2}
		pack $cntls.cancel -side left -expand yes

		set objectSelectionPanelPrompt 0
		wm protocol $win WM_DELETE_WINDOW "$cntls.cancel invoke"

		dialog_wait $win objectSelectionPanelPrompt

		if { $objectSelectionPanelPrompt == "2" } {
			set selection {}
			return $selection
		}

		set selection [objectSelectionLists_selection $listBoxesId]
		return $selection
}
#=================================================================================

#button .b -text "Make Selection" -command {tk_messageBox -message [objectSelectionPanel_show]}
#pack .b
#==================================================================================================
