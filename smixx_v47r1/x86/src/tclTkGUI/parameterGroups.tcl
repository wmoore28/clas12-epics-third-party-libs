#====================================================================
#
#                           Boda Franek    Jan 2006
# Copyright Information:
#       Copyright (C) 2006 CCLRC. All Rights Reserved
#
#  $Id: parameterGroups.tcl,v 1.13 2007/08/07 09:42:32 franek Exp $
#====================================================================
namespace eval ParameterGroups {
    namespace export group_delete parameter_create parameter_set panel_create 
    namespace export allValExists getGroupParValues parameter_setInhibit
    namespace export group_exists

#======================================================================
proc parameter_create {groupId id name type {width 1}} {
# list of parameters belonging to the group
    global grpPars
# param data    
    global grpParam

    lappend grpPars($groupId) $id
    set grpParam($groupId/$id/name) $name
    set grpParam($groupId/$id/type) $type
    set grpParam($groupId/$id/width) $width
    set grpParam($groupId/$id/val) ""
    set grpParam($groupId/$id/valExists) 0
    set grpParam($groupId/$id/state) normal

    return
}
#==================================================================
proc group_exists {groupId} {
	global grpPars
	
	return [info exists grpPars($groupId)]
}
#==================================================================
proc parameter_setInhibit {groupId id} {
	global grpPars
	global grpParam
    
	if { [lsearch $grpPars($groupId) $id] == -1 } {
        	tk_messageBox -message "No such parameter : $id"
        	return
	}
 
	set grpParam($groupId/$id/state) disabled
	return
}	
#==================================================================
proc group_delete {groupId } {
    global grpPars
    global grpParam

    foreach id $grpPars($groupId) {
        unset grpParam($groupId/$id/name) 
        unset grpParam($groupId/$id/type)
        unset grpParam($groupId/$id/width)
        unset grpParam($groupId/$id/val)
        unset grpParam($groupId/$id/valExists)
        unset grpParam($groupId/$id/state)
    } 
    unset grpPars($groupId)

    return
}
#==================================================================
proc parameter_set { groupId id val} {
    global grpPars grpParWin
    global grpParam
    
    if { [lsearch $grpPars($groupId) $id] == -1 } {
        tk_messageBox -message "No such parameter : $id"
        return
    }
    
	set grpParam($groupId/$id/valExists) 1
	set grpParam($groupId/$id/val) $val
	if {[info exists grpParWin($groupId)]} {
		set win $grpParWin($groupId)
		if {[winfo exists $win]} {
			$win.$id/val configure -bg white
			$win.$id/val configure -relief sunken
		}
	}
    
    return
}
#==================================================================
proc getParm_name { groupId id } {
    global grpPars
    global grpParam
    return $grpParam($groupId/$id/name)
} 
#==================================================================
proc getParm_type { groupId id } {
    global grpPars
    global grpParam
    return $grpParam($groupId/$id/type)
} 
#==================================================================
proc getParm_width { groupId id } {
    global grpPars
    global grpParam
    return $grpParam($groupId/$id/width)
} 
#==================================================================
proc getParm_val { groupId id } {
    global grpPars
    global grpParam
    return $grpParam($groupId/$id/val)
} 
#==================================================================
proc getParm_valExists { groupId id } {
    global grpPars
    global grpParam
    return $grpParam($groupId/$id/valExists)
} 

#==================================================================
proc getParm_state { groupId id } {
    global grpPars
    global grpParam
    return $grpParam($groupId/$id/state)
} 
#==================================================================
proc panel_create {win groupId} {
    global grpPars grpParWin
    global grpParam
#puts stdout " Creating parpanel of group $groupId   $win"

	set grpParWin($groupId) $win
	
    frame $win
        set winColor [$win cget -bg]
		label $win.namTit -text "Name" -relief flat
		grid $win.namTit -row 0 -column 0 -pady 3 -padx 2


        label $win.valTit -text "Value" -relief flat
        grid $win.valTit -row 0 -column 1 -pady 3 -padx 2

        label $win.typTit -text "Type" -relief flat
        grid $win.typTit -row 0 -column 2 -pady 3 -padx 2

        set i 0
        foreach id $grpPars($groupId) {
            set name [getParm_name $groupId $id]
            set type [getParm_type $groupId $id]
            set width [getParm_width $groupId $id]
            set state [getParm_state $groupId $id]
            set valExists [getParm_valExists $groupId $id]
            set entryColor White
	    set relief sunken
            if { $valExists == 0 } {
                set entryColor $winColor
		if { $state == "disabled" } {
			set relief flat
		}		
            }

            incr i
            label $win.$id/name -text $name -relief flat
            grid $win.$id/name -row $i -column 0 -sticky e -padx 5
            if { $type != "c" } {
	    	if { $state == "disabled" } {
			label $win.$id/val -textvariable grpParam($groupId/$id/val)  \
				-bg $entryColor -relief $relief
		} else {
                	entry $win.$id/val -textvariable grpParam($groupId/$id/val)  \
                       -borderwidth 1 -highlightthickness 1 \
                      -state $state -bg $entryColor -relief $relief
                	bind $win.$id/val <Key> "$win.$id/val configure -background white
                                         set grpParam($groupId/$id/valExists) 1"
		}
		grid $win.$id/val -row $i -column 1  -padx 5 -pady 3 -sticky w

                label $win.$id/type -text $type -relief flat
                grid $win.$id/type -row $i -column 2 -padx 5 -pady 3
            } else {
                checkbutton $win.$id/val -variable grpParam($groupId/$id/val) \
                           -onvalue yes -offvalue no  -state $state \
                           -selectcolor Green -bg $entryColor
                grid $win.$id/val -row $i -column 1 -sticky w -padx 5 -pady 3

            }
        }

    return $win
}
#===================================================================================
proc getGroupParValues {grId } {
    global grpPars
    global grpParam

    set parameters {}

    foreach id $grpPars($grId) {
        lappend parameters [list $grpParam($grId/$id/name) $grpParam($grId/$id/type) $grpParam($grId/$id/val)]
    }

    return $parameters
}
#==================================================================================
proc allValExists {groupId} {
    global grpPars
    global grpParam

    foreach id $grpPars($groupId) {
        if { $grpParam($groupId/$id/valExists) == "0" } {
            return 0
        }
    }
    return 1
}
}

