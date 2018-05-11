# This is completely self sufficient program. To incorporate it into
# the main GUI, comment the following 4 lines and also the button .b
# and pack .b line at the end
set comment1 {
global GUIdir
set GUIdir .
source ./../utilities/efftcl/my_dialog.tcl
source ./Scrolled_Listbox.tcl
}
#===============================================================================
proc getDomainsForList_sets {} {
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
proc getObjectSetsForList {domain} {
	
	set domsets [ exec getDomainObjectSets $domain ]
	
	set domsetsSorted [lsort $domsets]
	return $domsetsSorted
}
#==============================================================================
proc getSetObjectsForList {objset} {
	set objects [ exec getSetObjects $objset ]

	set objectsSorted [lsort $objects]
	return $objectsSorted
}
#===============================================================================
proc setDisplayLists_create { win } {
	# The procedure creates three labelled and scrollabel lists side by side
	global GUIdir
	
	frame $win
		# Domains list ------------------------------------------------------
		set domainsLabel [label $win.domainsLabel -text "Visible Domains"]
		set  domains [Scrolled_Listbox $win.domains \
			-width 20 -height 10 -selectmode single]

		# Domain Object Sets list -----------------------------------------------
		set objSetsLabel [label $win.objSetsLabel -text "Domain Object Sets" \
		    -bg white -borderwidth 3 -relief sunken]
		set objSets [Scrolled_Listbox $win.objSets \
			-width 20 -height 10 -selectmode single]


		# object list ----------------------------------------------
		set objectsLabel [label $win.objectsLabel -text "Set Objects" \
		    -bg white -borderwidth 3 -relief sunken]
		set objects [Scrolled_Listbox $win.objects \
			-width 20 -height 10 -selectmode single]
		set objectsUpdate [label $win.objectsUpdate -text "updated\n " \
		      -bg white -borderwidth 3 -relief sunken]



		set but_showsets [button $win.showsets -text "Show Domain sets" \
	-command "showDomainSets  $domains $objSets $objects $objSetsLabel $objectsLabel $objectsUpdate"]

		set but_showobjects [button $win.showobjects -text "Show Set Objects" \
		      -command "showSetObjects  $objSets $objects $objectsLabel $objectsUpdate"]

		# place the elements on the frame
		grid $domainsLabel -row 0 -column 0
		grid $win.domains -row 1 -column 0
		grid $but_showsets -row 2 -column 0

		grid $objSetsLabel -row 0 -column 1
		grid $win.objSets -row 1 -column 1
		grid $but_showobjects -row 2 -column 1

		grid $objectsLabel -row 0 -column 3
		grid $win.objects -row 1 -column 3
		grid $objectsUpdate  -row 2 -column 3


		# Insert all the domains
		set values [getDomainsForList_sets]
		foreach x $values {
			$domains insert end $x
		}

	return $win
}
#================================================================================
proc showDomainSets {src setsdisp objsdisp setsLabel objsLabel objsUpdate} {
	# The procedure will find out the list of the Sets for the selected domain and
	# displays them in the appropriate list

	#First clear the previous set list
	
#	puts stdout "showDomainSets called"
#	puts stdout "$src   $setsdisp"
	
	$setsdisp delete 0 end
	$objsdisp delete 0 end
	$setsLabel configure -text "   \nSets"
	$objsLabel configure -text "   \nObjects"
	$objsUpdate configure -text "updated\n "
	
	update idletasks

	# Find which domain was selected
	set i [$src curselection]
	
	if { $i == {} } {
		tk_messageBox -message "Select domain first!" -parent $src
		return
	}
	
	set domain [$src get $i]

	# Now get the domain Sets
	set sets [getObjectSetsForList $domain]
	foreach objset $sets {
		$setsdisp insert end $objset
	}
	
	$setsLabel configure -text "$domain\nSets"
	focus $setsdisp
}
#================================================================================
proc showSetObjects {src dst objsLabel objsUpdate} {
	# The procedure will find out the list of the Objects for the selected
	# Set and displays them in the appropriate list

	#First clear the previous object list
	
#	puts stdout "showSetObjects called"
	
	$dst delete 0 end
	update idletasks

	# Find which Object Set was selected
	set i [$src curselection]
	
	if { $i == {} } {
		tk_messageBox -message "Select Set first!" -parent $src
		return
	}

	set objset [$src get $i]

	# Now get the Set's Objects
	set objects [getSetObjectsForList $objset]
	
	foreach object $objects {
		$dst insert end $object
	}
	
	$objsLabel configure -text "$objset\nObjects"
	set timeString [clock format [clock seconds] -format "%d-%h-%y %T"]
	$objsUpdate configure -text "updated\n$timeString"
	focus $dst
}
#=================================================================================
proc objectSetDisplayPanel_show {} {
 

	set top [dialog_create ObjectSetsPanel]
		set info [dialog_info $top]
		set cntls [dialog_controls $top]
		set win $top

		set listBoxesId [setDisplayLists_create $info.listBoxes ]

		pack $listBoxesId -expand true -fill both	

		button $cntls.dismiss -text Dismiss -command "destroy $win"
		pack $cntls.dismiss -side left -expand yes

		wm protocol $win WM_DELETE_WINDOW "$cntls.dismiss invoke"

		return 
}
#=================================================================================
set comment2 {
button .b -text "Make Selection" -command {objectSetDisplayPanel_show}
pack .b
}
#==================================================================================================
