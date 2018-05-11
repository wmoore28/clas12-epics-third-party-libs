#==================================================================
proc selectFromListPanel_show {title items} {
	global listSelectionPanelPrompt

	set top [dialog_create ListSelectionPanel]
	set info [dialog_info $top]
	set cntls [dialog_controls $top]
	set win $top
        
	set listBoxId [listBox_create $info.listBox $title]
	pack $listBoxId
	
	set inp [listBox_input $listBoxId]
	$inp configure -selectmode extended	
	
	foreach item $items {
		$inp insert end $item
	}
	

	button $cntls.ok -text OK -command {set listSelectionPanelPrompt 2}
	pack $cntls.ok -side left -expand yes
	
	button $cntls.cancel -text Dismiss -command {set listSelectionPanelPrompt -1}
	pack $cntls.cancel -side left -expand yes


	set listSelectionPanelPrompt 0
	focus $inp
	$inp selection set 0
	wm protocol $win WM_DELETE_WINDOW "$cntls.cancel invoke"	
	

	dialog_wait $win listSelectionPanelPrompt
		
	set selection {}
	set numselection {}

	if {$listSelectionPanelPrompt =="-1"} {
		set selection {}
	}
	if {$listSelectionPanelPrompt == "2"} {
		set numselection [$inp curselection]
		foreach nsel $numselection {
			lappend selection [$inp get $nsel]
		}
	}
	
	grab release $win
	wm withdraw $win
	destroy $win
	
	return  $selection
}

