#! /usr/bin/wish -f
#==============================================================================
# Generic SMI GUI
#
#                                  Boda Franek    December 2005
# Copyright Information:
#      Copyright (C) 2005 CCLRC.   All Rights Reserved
#
#    $Id: main.tcl,v 1.39 2010/09/01 13:53:35 franek Exp $
#==============================================================================

set hel10font [font create hel10font -family helvetica -size 10]
set helbold10font [font create helbold10font -family helvetica -size 10 -weight bold]

set hel12font [font create hel12font -family helvetica -size 12]
set helbold12font [font create helbold12font -family helvetica -size 12 -weight bold]

set hel32font [font create hel32font -family helvetica -size 32]
set helbold32font [font create helbold32font -family helvetica -size 32 -weight bold]

option add *Label.font $hel12font
option add *Label.relief flat

option add *Entry.background white
option add *Entry.font $hel12font
option add *Entry.width 20

option add *Button.relief raised
option add *Button.font $helbold12font

option add *Menubutton.relief raised
option add *Menubutton.font $hel12font

option add *Listbox.background white
#==============================================================================

# root Tcl/Tk directory. Its value is picked up from the command that executes
# this file
global GUIdir
set GUIdir [file dirname $argv0]

#puts stdout $argv0
#puts stdout [file dirname $argv0]
#puts stdout [file tail $argv0]
#puts stdout "GUIdir $GUIdir"	


#sourced-files-beg==============================================================
# This assumes that
#	1) $GUIdir is the source directory, where also main.tcl resides
# 	2) Also it assumes that the position of efftcl directory is fixed
#	   relative to GUIdir.
# If these assumtions are correct, then the GUI can be simply run by 
# giving command <source directory>/main.tcl (eg ./main.tcl if that is the
# current directory.)
# For 'pruduction' purposes the GUI is then built into a single file which
# is then placed into 'linux' directory or such together with a few gif files
# of graphical widgets (such as downarrow and such)
# During the build process the files bellow get picked up from
# the source directory and replace the source commands below. The
# resulting file does not rely any more on the assumption 1 and 2.

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/actionParPanel_display.tcl
#====================================================================
#
#                           Boda Franek    Dec 2005
# Copyright Information:
#       Copyright (C) 2005 CCLRC. All Rights Reserved
#
#  $Id: actionParPanel_display.tcl,v 1.15 2010/06/24 12:21:01 franek Exp $
#====================================================================
#===============================================================================
proc actionParPanel_display {object action parameters} {
# The panel is modal and has 3 sections :
#  1 - Heading
#  2 - parameters
#  3 - controls  OK and Cancel buttons
#===============================================================================
	global objectId
	global actionParPanelid actionParPanelidAnswer
	global actionParPanelAction actionParPanelObject
    
	if { [info exists actionParPanelid] } {
		bell
		wm deiconify $actionParPanelid
		raise $actionParPanelid
		update idletasks
		return
	}
    
	set actionParPanelAction $action
	set actionParPanelObject $object

# create the Panel 
	set actionParPanelid [dialog_create "Action Parameters"]
    
# get the window path for the information part of the dialog   
	set info [dialog_info $actionParPanelid]

# position it in the right place relative to the parent window
#	set parentWin $objectId($object)
        set parentWin .
	set parentPosX [winfo rootx $parentWin]
	set parentPosY [winfo rooty $parentWin]
	
	set x [expr $parentPosX+20]
	set y [expr $parentPosY+20]
    
#    puts stdout "Placing $actionParPanelid  at x $x  y $y"
	wm geometry $actionParPanelid "+$x+$y"  
	
	set infoOnlyId [actionParInfoPanel $info.settings $object $action $parameters]
	pack $infoOnlyId

    set cntls [dialog_controls $actionParPanelid]
    
	button $cntls.ok -text ok -command { set actionParPanelidAnswer 1}
	pack $cntls.ok -side left -expand yes

    button $cntls.cancel -text Cancel -command { set actionParPanelidAnswer 0}
    pack $cntls.cancel -side left -expand yes

    focus $cntls.ok

    wm protocol $actionParPanelid WM_DELETE_WINDOW "$cntls.cancel invoke"

	while { "1" == "1" } {
		dialog_wait $actionParPanelid actionParPanelidAnswer
	
		if { $actionParPanelidAnswer == "0" } {
			break;
		}
		set flg [actionParPanelOK]
		if { $flg == 1 } {
			break;
		}
	}
	actionParPanel_destroy

    return 
}
#===========================================================================
proc actionParPanel_destroy {} {
    global actionParPanelid

    destroy $actionParPanelid
    unset actionParPanelid

    ParameterGroups::group_delete 1
    return
}
#===========================================================================
proc actionParHeadingPanel_create {win object action} {

	frame $win        
        	label $win.title -text "object: $object  action: $action" \
			-relief flat
        	pack $win.title
	return $win
}
#===========================================================================
proc actionParOnlyPanel_create {win parameters} {

	frame $win
	        actionParameterGroup_setup $parameters

        	ParameterGroups::panel_create $win.parameters  1
        	pack $win.parameters
	return $win
}
#===========================================================================
proc actionParInfoPanel  {win object action parameters}	{
	
	frame $win
		set head [actionParHeadingPanel_create $win.head $object $action]
		set parOnly [actionParOnlyPanel_create $win.par $parameters]
		
		pack $head
	        
		frame $win.sep -height 2 -borderwidth 1 -relief sunken
        	pack $win.sep -fill x -pady 4
		
		pack $parOnly
	return $win
}		
#==============================================================================
proc actionParPanelOK {} {
    global actionParPanelid actionParPanelidAnswer
    global actionParPanelObject actionParPanelAction

    if { [ParameterGroups::allValExists 1] == "0" } {
        tk_messageBox -message "All parameters have to have values assigned"
        return 0
    }

    set parameters [ ParameterGroups::getGroupParValues 1]

# check the par values so that correction can be made before par panel goes
	set errmsg ""
	if { $parameters != "none" } {
		foreach par $parameters {
			set parname [lindex $par 0]
			set partype [lindex $par 1]
			set parval  [lindex $par 2]
			if { $partype == "S" } {
				set res [ convertEASCIItoHEX $parval ]
				if { [lindex $res 0] != "" } {
					set errmsg "Parameter ${parname} :\n \
					[lindex $res 0] "
				}   
			} elseif { $partype == "I" } {
				if { [string is integer $parval] != 1 } {
					set errmsg "Parameter ${parname} :\n \
					must be integer"
				}
			} elseif { $partype == "F" } {
				if { [string is double $parval] != 1 \
				|| [string first "." $parval] <0 \
				&& [string first "e" $parval] <0 \
				&& [string first "E" $parval] <0} {
					set errmsg "Parameter ${parname} :\n \
					must be floating point number"
				}
			}

		}
	} 
 
	if { $errmsg != "" } {
		set ans [tk_messageBox -icon error -type yesno -default no \
		-message "$errmsg \nDo you need help ?"]
		if { $ans == "yes" } {
			set helpId [parameterHelp_show]
			grab release $actionParPanelid
			focus $helpId
			grab set $helpId
			dialog_safeguard $helpId
			tkwait window $helpId
			focus $actionParPanelid
			grab set $actionParPanelid
		}	
		
		return 0
	}

	
	dispatchAction $actionParPanelObject $actionParPanelAction $parameters

	return 1

}
#===================================================================================

#======================================================================
proc actionParameterGroup_setup {parameters} {

# There is only 1 action param panel so I choose parGroupId to be 1

	set parGroupId 1
	
    foreach parm $parameters {	
	set name [lindex $parm 0]
	set type [lindex $parm 1]

        set id [string tolower $name]    

	ParameterGroups::parameter_create $parGroupId $id $name $type -1
	
	if { [llength $parm] == 3 } {
		set val [lindex $parm 2]        
		ParameterGroups::parameter_set $parGroupId $id $val
	}

    }
}   

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/objectParPanel_create.tcl
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

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/parameterGroups.tcl
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


# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/../utilities/efftcl/my_dialog.tcl
# ----------------------------------------------------------------------
#  EXAMPLE: procedures to create dialogs
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================
# Modified (bug fixed) by B.Franek on 04-Dec-2002

proc dialog_create {class {win "auto"}} {
    if {$win == "auto"} {
        set count 0
        set win ".dialog[incr count]"
        while {[winfo exists $win]} {
            set win ".dialog[incr count]"
        }
    }
    toplevel $win -class $class

    frame $win.info
    pack $win.info -expand yes -fill both -padx 4 -pady 4

    frame $win.sep -height 2 -borderwidth 1 -relief sunken
    pack $win.sep -fill x -pady 4

    frame $win.controls
    pack $win.controls -fill x -padx 4 -pady 4

    wm title $win $class
    wm group $win .

#    after idle [format {
#        update idletasks
#        wm minsize %s [winfo reqwidth %s] [winfo reqheight %s]
#    } $win $win $win]

    return $win
}

proc dialog_info {win} {
    return "$win.info"
}

proc dialog_controls {win} {
    return "$win.controls"
}

proc dialog_wait {win varName} {
    dialog_safeguard $win

#    set x [expr [winfo rootx .]+50]
#    set y [expr [winfo rooty .]+50]
#    wm geometry $win "+$x+$y"

    wm deiconify $win
#-------- B.Franek  04-Dec-2002 ----------------
    update idletasks
#----------
    
    grab set $win
    vwait $varName

    grab release $win
    wm withdraw $win
}

bind modalDialog <ButtonPress> {
    wm deiconify %W
    raise %W
}
proc dialog_safeguard {win} {
    if {[lsearch [bindtags $win] modalDialog] < 0} {
        bindtags $win [linsert [bindtags $win] 0 modalDialog]
    }
}

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/../utilities/efftcl/confirm.tcl
# ----------------------------------------------------------------------
#  EXAMPLE: simple confirmation dialog
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================
# modified by B.Franek on 10-Jun-2002. Adding access to the window

option add *Confirm*icon.bitmap questhead widgetDefault
option add *Confirm*mesg.wrapLength 4i widgetDefault

proc confirm_ask {mesg {ok "OK"} {cancel "Cancel"}} {
    global confirmStatus
#
#--- B.Franek 10-Jun-2002    
    global confirmWinId
    if {[info exists confirmWinId]} {
        puts stdout \
         "can not pop up another confirm window untill the curent is destroyed"
        return
    }
#----

    set top [dialog_create Confirm]
    set confirmWinId $top

    set info [dialog_info $top]
    label $info.icon
    pack $info.icon -side left -padx 8 -pady 8
    label $info.mesg -text $mesg
    pack $info.mesg -side right -expand yes -fill both -padx 8 -pady 8

    set cntls [dialog_controls $top]
    button $cntls.ok -text $ok -command {set confirmStatus 1}
#---- B. Franek   21 Dec 2004
    if { $ok != "nil" } {
        pack $cntls.ok -side left -expand yes
    }
    button $cntls.cancel -text $cancel -command {set confirmStatus 0}
#---- B. Franek   06 Dec 2004
    if { $cancel != "nil" } {
        pack $cntls.cancel -side left -expand yes
    }
    focus $cntls.ok

    wm protocol $top WM_DELETE_WINDOW "$cntls.cancel invoke"

    dialog_wait $top confirmStatus
    destroy $top
#---- B.Franek 10-Jun-2002
    unset confirmWinId
#----
    return $confirmStatus
}
#---- B.Franek 10-Jun-2002
#============================================================================
proc confirm_destroy {} {
    global confirmStatus
    set confirmStatus destroyed
    return
}
#============================================================================
proc confirm_controls {} {
    global confirmWinId
    return [dialog_controls $confirmWinId]
}
#============================================================================
proc confirm_info {} {
    global confirmWinId
    return [dialog_info $confirmWinId]
}
#----
   

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/simpleEntry_show.tcl
#====================================================================
#
#                           Boda Franek    Dec 2006
# Copyright Information:
#       Copyright (C) 2005-2006 CCLRC. All Rights Reserved
#
#  $Id: simpleEntry_show.tcl,v 1.1 2006/12/18 06:50:37 franek Exp $
#====================================================================
#===============================================================================
proc simpleEntry_show {text {ok "OK"} {cancel "Cancel"} } {
# The panel is modal and has 2 sections :
#  1 - Entry
#  3 - controls  OK and Cancel buttons
#===============================================================================
	global simpleEntryWinId simpleEntryAnswer simpleEntryResult
    
	if { [info exists simpleEntryWinId] } {
		bell
		wm deiconify $simpleEntryWinId
		raise $simpleEntryWinId
		update idletasks
		return
}
# create the Window 
	set simpleEntryWinId [dialog_create "SimpleEntry"]
    
# get the window path for the information part of the dialog   
	set info [dialog_info $simpleEntryWinId]

# position it in the right place relative to the parent window
	set parentWin .
 
	set parentPosX [winfo rootx $parentWin]
	set parentPosY [winfo rooty $parentWin]
	
	set x [expr $parentPosX+20]
	set y [expr $parentPosY+20]
    
	wm geometry $simpleEntryWinId "+$x+$y"  
	
	set lab [label $info.lab -text $text -bg Yellow]
	set ent [entry $info.ent -textvariable simpleEntryResult ]
	
	pack $lab $ent -pady 5

	set cntls [dialog_controls $simpleEntryWinId]
	button $cntls.ok -text $ok -command "executeOK "

	pack $cntls.ok -side left -expand yes

	button $cntls.cancel -text $cancel -command { set simpleEntryAnswer 0}
	pack $cntls.cancel -side left -expand yes

	bind $info.ent <Return> "$cntls.ok invoke"
	focus $info.ent

	wm protocol $simpleEntryWinId WM_DELETE_WINDOW "$cntls.cancel invoke"

	set simpleEntryResult {}
	dialog_wait $simpleEntryWinId simpleEntryAnswer
	
#	puts stdout $simpleEntryAnswer
#	puts stdout $simpleEntryResult
	simpleEntry_destroy

	if { $simpleEntryAnswer == 0 } {
		return ""
	} else {
		return $simpleEntryResult
	} 
     
}
#===========================================================================
proc simpleEntry_destroy {} {
    global simpleEntryWinId

    destroy $simpleEntryWinId
    unset simpleEntryWinId

    return
}
#==============================================================================
proc executeOK {} {
    global simpleEntryWinId simpleEntryAnswer
    
    set simpleEntryAnswer 1


    return

}
#===================================================================================

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/topbar.tcl
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

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/mainPanel.tcl
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
	

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/listBox_create.tcl
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

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/selectFromListPanel_show.tcl
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


# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/convertHEXtoEASCII.tcl
#==============================================================================
#
#                                  Boda Franek    November 2006
# Copyright Information:
#      Copyright (C) 2005 CCLRC.   All Rights Reserved
#
#    $Id: convertHEXtoEASCII.tcl,v 1.4 2007/07/16 10:45:26 franek Exp $
#==============================================================================
proc convertHEXtoEASCII {instring} {
#=====================================================================
#   every pair of characters in the 'instring' (starting with character 0
#   and chacter 1 will be interpreted as a HEX number representing
#    an ASCII character. This will be converted to the correspondig ASCII
#   character if printable. If non-printable, \x will be pre-pended
#   to the pair. An exception is '\'. This will be converted to '\\'
#=====================================================================
	set strlen [string length $instring]
	set outstring ""

	for {set i 0} {$i<$strlen} {incr i 2} {
		set i1 [expr $i+1]
		if { $i1 >= $strlen } {
			bgerror "the input string has odd number of characters"
			return ""
		}
		set a [string range $instring $i $i1]
#		puts stdout " $a"
		set ahex "0x$a"
		
		if { [catch {format "%c" $ahex} b] } {
			bgerror $b
			return ""
		}

		if { $b == "\\" } {
			append outstring "\\\\"
			continue
		} 
				
		if { $b == " " } {
			append outstring $b
			continue
		}
				
		if { [string is graph $b] } {
			append outstring $b
			continue
		}
		
		if { $b == "\a" } {
			append outstring "\\" a
		} elseif { $b == "\b" } {
			append outstring "\\" b
		} elseif { $b == "\t" } {
			append outstring "\\" t
		} elseif { $b == "\n" } {
			append outstring "\\" n
		} elseif { $b == "\v" } {
			append outstring "\\" v
		} elseif { $b == "\f" } {
			append outstring "\\" f
		} elseif { $b == "\r" } {
			append outstring "\\" r
		} else {
			append outstring "\\x"
			append outstring $a
		}
	}
	
	return $outstring
}
#===========================================================================
proc convertEASCIItoHEX {instring} {
#==========================================================================
#  Input string should consist should follow the rules governing inputting
#  C/C++ constants. It means that non-printing characters require escape
#  sequence. Out of the 3 special printing characters <\>, <"> and <\>,
#  only <\> has to be escaped. The other two can be but do not have to be
#  escaped.
#  An escape sequence begins with \ and is followed by either a special
#  character (such as \n  for new line) or by x and 2 hex digits
#  representing the character.
#==========================================================================
	set strlen [string length $instring]
	set outstring ""

	set specials "a\\br\"ftn\'v"	
	for {set i 0} {$i<$strlen} {incr i} {
		set ch [string range $instring $i $i]
		if { $ch == "\\" } {
			set i1 [expr $i+1]	
			set ch1 [string range $instring $i1 $i1]
			if { [string first $ch1 $specials] >= 0 } {
				if { $ch1 == "\\" } {
					binary scan "\\" "H2" bout					
				} elseif { $ch1 == "a" } {
					binary scan "\a" "H2" bout
				} elseif { $ch1 == "b" } {
					binary scan "\b" "H2" bout
				} elseif { $ch1 == "t" } {
					binary scan "\t" "H2" bout
				} elseif { $ch1 == "n" } {
					binary scan "\n" "H2" bout
				} elseif { $ch1 == "v" } {
					binary scan "\v" "H2" bout
				} elseif { $ch1 == "f" } {
					binary scan "\f" "H2" bout
				} elseif { $ch1 == "r" } {
					binary scan "\r" "H2" bout
				} elseif { $ch1 == "\"" } {
					binary scan "\"" "H2" bout
				} elseif { $ch1 == "\'" } {
					binary scan "\'" "H2" bout
				}
				append outstring $bout
				incr i 
				continue
			} 

			if { $ch1 == "x" || $ch1 == "X" } {
				set i2 [expr $i1+1]
				set ch2 [string range $instring $i2 $i2]
				set i3 [expr $i2+1]
				if { $i3 >= $strlen } {
					return [list "the value string <$instring> \n contains \
					illegal shortened escape sequence \\$ch1$ch2" ""]
				}
				set ch3 [string range $instring $i3 $i3]
				if { [string first $ch2 "01234567"] < 0  || [string is xdigit $ch3] == 0} {
					return [list "the value string <$instring> \n contains \
					illegal hex escape sequence \\$ch1$ch2$ch3" ""]
				}
					
				append outstring [string range $instring $i2 $i3]
				incr i 3
				continue
			} else {
				set errmsg "the value string <$instring> \n contains \
				unknown escape sequence <\\$ch1>"
					return [list $errmsg ""]
			}
		} else {
			binary scan $ch "H2" bout
			append outstring $bout
		}
				
	}
	
	return [list "" $outstring]
}

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/tcl-smi.tcl
#==============================================================================
# Tcl/Tk  -  SMI  Interface
#
#                                  Boda Franek    December 2005
# Copyright Information:
#      Copyright (C) 2006 CCLRC.   All Rights Reserved
#
#    $Id: tcl-smi.tcl,v 1.24 2007/08/03 13:47:37 franek Exp $
#==============================================================================
proc registerObject { name } {
	global guiPID
	global objectList

	if { [info exists objectList] } {
		if { [lsearch $objectList $name] != -1 } {
			puts stdout " object $name is already registered in tcl-smi"
			return 1
		}
	}
    
	lappend objectList $name
	exec tellMonObjects $guiPID register/$name    
	return 0
}
#===========================================================================
proc requestObjectRemoval {objName} {
	global guiPID
	global objectList
#	tk_messageBox -message " removal of object $objName not implemented yet"
	if { [lsearch $objectList $objName] == "-1"  } {
		tk_messageBox -message "non-existing object $objName can not be removed"
	}
	exec tellMonObjects $guiPID remove/$objName
	return
}
#===========================================================
proc startMonitoring {} {
	global guiPID
    global pipe
	global monObjectsPID
    global objectList
    global oLfileName
    global tcl_platform env

    set platform $tcl_platform(platform)
    
    if { $platform == "windows" } {
        set tempDir $env(TEMP)
    } else {
        set tempDir /tmp
    }

	set guiPID [pid]

	set logfile $tempDir/monObjects.log
	set errfile $tempDir/monObjects.err

	if [catch {open "| monObjects $guiPID"} pipe] {
    		tk_messageBox -message "cannot open pipe : $pipe"
    		exit
	}

	set monObjectsPID [pid $pipe]
#tk_messageBox -message $monObjectsPID
	after 2000

    fileevent $pipe readable [list Reader $pipe]
    return
}
#=======================================================================
proc stopMonitoring {} {
	global guiPID
	global monObjectsPID
	global tcl_platform

	set platform $tcl_platform(platform)

	catch {exec tellMonObjects $guiPID exit/}
            
#	if { $platform == "windows" } {
#		catch {exec TASKKILL /F /PID $monObjectsPID}
#	} else {
#		catch {exec kill -9 $monObjectsPID}  
#	}

	return
}
#=======================================================================
proc Reader { pipe } {
    global inString inStringTime    
    global objectState objectActions objectParameters

#tk_messageBox -message reader
    if [eof $pipe] {
        if { [catch {close $pipe} result] } {
        	puts stderr "close $pipe ... failed"
                puts stderr $result
        }
#	tk_messageBox -message "process monObjects exited"
	puts stdout "process monObjects exited"
        exit
    }
    gets $pipe inString
    set type [getInStringType]
#tk_messageBox -message "time:$inStringTime   Type:$type"
#    puts stdout $inString

    if {$type == "*time-tick"} {
#        timeTickUpdate
        return
    }

    if {$type != "*object"} {
        puts stdout "Proc Reader : Unknown Type |$type|"
        puts stdout $inString
        return
    }           
    set object [getInStringObject]

#    set state [getInStringObjectState]

    unpackInString $object
    
#    puts stdout [getObjectActions $object [getState $object]]
    
    userUpdate $type $object

    return    
}
###################################################################

proc getInStringType {} {
    global inString inStringList inStringTime 
    set inStringList [split $inString /]
    set inStringTime [lindex $inStringList 0]
    set type [lindex $inStringList 1]
    return $type
}
###################################################################
proc getInStringObject {} {
    global inStringList
    set object [lindex $inStringList 2]
    return $object
}
#===========================================================================
proc unpackInString {object} {
    global inStringList
    global objectState objectParameters objectActions actionParameters

	
#	puts stdout $inStringList
	
    set noElements [llength $inStringList]

    set point 4
    set state [lindex $inStringList 4]
    set objectState($object) $state
   
#                    Object Parameters
    incr point
# The next element is either *pars if object has parameters or *end_state
    if {[lindex $inStringList $point] == "*pars" } {
    	set begpars $point
        set parameters ""
        set endpars [findElement [lrange $inStringList $point end] "*end_pars"]
	if { $endpars <0 } {
		tk_messageBox -message "Wrong Syntax - processing object parameters,\n \
		      *end_pars not found:\n $inStringList"
		exit
	}
#   this points to *end_pars
	incr point $endpars
	incr begpars
	set endpars $point
	incr endpars -1
	set objparList [lrange $inStringList $begpars $endpars]
#	puts stdout " "
#	puts stdout "begpars $begpars   endpars  $endpars"
#	puts stdout $objparList
	set objectParameters($object) [processParameters $objparList]
#	puts stdout $objectParameters($object)
# this points to *end_state
	incr point
    } 
    
	if {[lindex $inStringList $point] != "*end_state" } {
		tk_messageBox -message "Wrong syntax - *end_state not found:\n $inStringList"
		exit
	}	
    
    
#      
#                 Object Actions
#tk_messageBox -message "point $point   noElements $noElements"
# now it looks : *action......*end_action *action....*end_action

    set actions ""
    
    
    incr point 
    if {$noElements <= $point} {
        return
    }
#tk_messageBox -message "point $point  [lindex $inStringList $point]"
    while { 1 == 1 } {
        set element [lindex $inStringList $point]
	if { $element != "*action" } {
		tk_messageBox -message "Wrong Syntax :$inStringList"
		exit
	}
	set iend [findElement [lrange $inStringList $point end] "*end_action"]
	if { $iend < 0 } {
		tk_messageBox -message "Wrong Syntax - *end_action not found:$inStringList"
		exit
	}
	
	incr iend $point
	set first $point
	incr first
	set last $iend
	incr last -1
	set actionList [lrange $inStringList $first $last]
#	puts stdout $actionList
	set action [lindex $actionList 0]
	lappend actions $action
	if {[llength $actionList] > 1} {
		if { [lindex $actionList 1] != "*pars" || [lindex $actionList end] != "*end_pars" } {
			tk_messageBox -message "Wrong syntax - *pars or *end_pars not found: $inStringList"
			exit
		}
		set actionParameters($object/$state/$action) [processParameters [lrange $actionList 2 end-1] ]
#		puts stdout $actionParameters($object/$state/$action)
	}
	set point $iend
        incr point
	if {$noElements <= $point} {
		break
	}
    }
    
#    puts stdout $actions
    set objectActions($object/$state) $actions
#tk_messageBox -message $actions
    return 
}
#=======================================================================================
proc processParameters {parameters} {

	set noPar [llength $parameters]
	
	for {set i 0} {$i < $noPar} {incr i} {
		set parm [lindex $parameters $i]
		if {[string first = $parm] == -1} {
		# parameter has no value because equal sign is not present
			set nametype $parm
			if { [info exists def] == "1" } {
				unset def
			}
		} else {
			set splitted [split $parm =]
			set nametype [lindex $splitted 0]
			set def [lindex $splitted 1]
		}
		set brakinx [string first ( $nametype]
		set name [string range $nametype 0 [expr $brakinx - 1] ]
		set type [string range $nametype [expr $brakinx + 1] [expr [string length $nametype] - 2]]
		
		set parmNew [list $name $type]
		if { [info exists def] == 1 } {
			if { $type == "S" } {
                		set defEASCII [convertHEXtoEASCII $def]
				lappend parmNew $defEASCII
                	} else {
                		lappend parmNew $def
                	}
		}
		
		lappend parsNew $parmNew
	}
	return $parsNew
}
#==========================================================================
proc findElement {inpList elem} {
	set inx [lsearch $inpList $elem]
	return $inx
}
#==========================================================================
proc getState {object} {
    global objectState
    
    return $objectState($object)

}
#===========================================================================
proc getObjectParameters {object} {
	global objectParameters

	if { [info exists objectParameters($object)] == 0 } {
		return {}
	} else {
		return $objectParameters($object)
	}
}
#===========================================================================
proc getObjectActions {object state} {
	global objectActions

	if { [info exists objectActions($object/$state)] == 0 } {
		set actions {}
	} else {
		set actions $objectActions($object/$state)
	}

	return $actions
}
#==========================================================================
proc getActionParameters {object state action} {
	global actionParameters

	if { [info exists actionParameters($object/$state/$action)] == 0 } {
		set parameters {}
	} else {
		set parameters $actionParameters($object/$state/$action)
	}
	
	return $parameters
}
#============================= dispatchAction =============================
proc dispatchAction {object action {parameters none} } {
	global guiPID

    set dbg 0

   set actionString $action
  
	if { $parameters != "none" } {
		foreach par $parameters {
			set parname [lindex $par 0]
			set partype [lindex $par 1]
			set parval  [lindex $par 2]
			if { $partype == "S" } {
				set res [ convertEASCIItoHEX $parval ]
				set parvalConv [lindex $res 1]
			} elseif { $partype == "I" } {
				set parvalConv [format "%d" $parval]
			} elseif { $partype == "F" } {
				set parvalConv [format "%e" $parval]
			}

            append actionString "/" $parname " ${partype} " $parvalConv
        }
    } 
   
    if {$dbg != "0"} {
        set confirm [tk_messageBox -type okcancel -icon warning \
        -message \
	"Object: $object 
Action string:  $actionString"]

        if {$confirm =="cancel"} {
            return
        }
     }


	catch {exec tellMonObjects $guiPID "command/$object/$actionString" &} 
#     logMessage "Sent Command : $actionString  to Object : $object"
     return
}

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/dnsinfo.tcl
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

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/canvasObjects.tcl
#=========================================================================
proc getObjectId {object} {
	set objId [string tolower $object]

	set inx [string first :: $objId]
	set inx1 [expr $inx+1]
	set objId [string replace $objId $inx $inx1 /]


	set inx [string first :: $objId]
	if { $inx == "-1" } {
		return $objId
	}
	set inx1 [expr $inx+1]
	set objId [string replace $objId $inx $inx1 /]
	return $objId
}
#=========================================================================
proc updateObjectOnCanvas {win object state} {
	global stateColor objectsOnCanvas

	if { [ lsearch $objectsOnCanvas $object ] == "-1" } {
	# the object is not displayed
		return
	}
	
        set body $win.canvas.frame       

	set obId [getObjectId $object]
        $body.state$obId configure -text $state
	

	if { $state == "DEAD" || $state == "&DEAD" } {
		set fgcol white
		set bgcol black
	} else {
	        set fgcol black
		set bgcol white
	}
	
	if { [info exists stateColor($state)] } {
		set bgcol $stateColor($state)
	}
	
	$body.state$obId configure -bg $bgcol -fg $fgcol
				
	return
} 
#=========================================================================
proc removeObjectFromCanvas {win object} {
	global objectsOnCanvas
	
	set canvas $win.canvas
        set body $win.canvas.frame
	set obId [getObjectId $object]

	destroy $body.menubut$obId $body.domain$obId $body.object$obId \
		$body.state$obId $body.actions$obId $body.parameters$obId

	set inx [lsearch $objectsOnCanvas $object]
	set objectsOnCanvas [lreplace $objectsOnCanvas $inx $inx]
        
	update
	set botpos [winfo height $body]
	set rightpos [winfo width $body]
	$canvas configure -scrollregion "0 0 $rightpos $botpos"        

	return
}	
#=========================================================================
proc addObjectToCanvas {win object} {
	global imA imP imX
	global objectsOnCanvas

	if { [info exists objectsOnCanvas] } {
		set inx [lsearch $objectsOnCanvas $object]
		if { $inx != "-1" } {
		# object is already there
			bell
			return
		}
	}
	lappend objectsOnCanvas $object

	set inx [string first :: $object]
	set domain [string range $object 0 [expr $inx-1] ]
	set objectNm [string range $object [expr $inx+2] end ]
	
	set canvas $win.canvas
        set body $win.canvas.frame
        
	set lineNo [lrange [grid size $body] 1 1]
	set obId [getObjectId $object]
	
	if { [lsearch [font names] hel1font] == -1 } {	
		font create hel1font -family helvetica -size 2
	}

	menubutton $body.menubut$obId -image $imX  \
                   -menu $body.menubut$obId.m -font hel1font -background white
	menu $body.menubut$obId.m
	$body.menubut$obId.m add command \
		 -label "Remove Object" -command [list removeObjectFromCanvas $win $object]
	grid $body.menubut$obId -row $lineNo -padx 5 -pady 0 -column 5

        label $body.domain$obId -text $domain \
                                -background white -relief flat -font helbold12font
        grid $body.domain$obId -row $lineNo -padx 10 -pady  0 -column 2 -sticky w
        
        label $body.object$obId -text $objectNm  \
		-background white -relief flat -font helbold12font
        grid $body.object$obId -row $lineNo -padx 10 -column 3 -sticky w
        
        label $body.state$obId -text state \
		-background white -relief groove -font helbold12font
        grid $body.state$obId -row $lineNo -padx 10 -column 4 -sticky w


        menubutton $body.actions$obId -image $imA  \
	     -menu $body.actions$obId.menu \
	     -background white -font hel1font
	menu $body.actions$obId.menu -postcommand [list actionMenu_update $body.actions$obId.menu $object]
        grid  $body.actions$obId -row $lineNo -padx 2 -column 0
        
        button $body.parameters$obId -image $imP \
	     -command [list paramPanel_display $object] \
	     -background white -font hel1font
        grid  $body.parameters$obId -row $lineNo -padx 1 -column 1

	balloonhelp_for $body.actions$obId "click to display available actions"
	balloonhelp_for $body.parameters$obId "click to display object parameters"
	balloonhelp_for $body.menubut$obId "Remove object from display"
        
	update
	set botpos [winfo height $body]
	set rightpos [winfo width $body]
	$canvas configure -scrollregion "0 0 $rightpos $botpos"        
        return
}
#=============================================================================
proc objectPanel_create {win} {
	frame $win

		set height 200
		set width  400
		set borderwidth 2

		set hscroll $win.hscroll
		set vscroll $win.vscroll
		set canvas $win.canvas

		scrollbar $hscroll -orient horiz -command "$canvas xview"
		scrollbar $vscroll               -command "$canvas yview"
		canvas $canvas -relief sunken -borderwidth $borderwidth \
                              -width $width -height $height \
			      -background white \
                              -xscrollcommand "$hscroll set" \
                              -yscrollcommand "$vscroll set"
        	# Ensure that window resizings retain scroll bars.
		pack $hscroll -side bottom -fill x 
		pack $vscroll -side right  -fill y
		pack $canvas -fill both -expand yes

		frame $canvas.frame
       			$canvas create window 0 0 \
			       -window $canvas.frame -anchor nw 
			       
		$canvas.frame configure -background white

	set objectsOnCanvas {}
	return $win
}

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/../utilities/efftcl/balloon1.tcl
# ----------------------------------------------------------------------
#  EXAMPLE: use "wm" commands to manage a balloon help window
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

option add *Balloonhelp*background yellow widgetDefault
option add *Balloonhelp*foreground black widgetDefault
option add *Balloonhelp.info.wrapLength 3i widgetDefault
option add *Balloonhelp.info.justify left widgetDefault
option add *Balloonhelp.info.font \
    -*-lucida-medium-r-normal-sans-*-120-* widgetDefault

toplevel .balloonhelp -class Balloonhelp \
    -background black -borderwidth 1 -relief flat
bind .balloonhelp <Button-1> {balloonhelp_cancel}
bind .balloonhelp <Button-2> {balloonhelp_cancel}    
bind .balloonhelp <Button-3> {balloonhelp_cancel}

#label .balloonhelp.arrow -anchor nw \
#    -bitmap @[file join $env(EFFTCL_LIBRARY) images arrow.xbm]
#pack .balloonhelp.arrow -side left -fill y

label .balloonhelp.info
pack .balloonhelp.info -side left -fill y

wm overrideredirect .balloonhelp 1
wm withdraw .balloonhelp

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_for <win> <mesg>
#
#  Adds balloon help to the widget named <win>.  Whenever the mouse
#  pointer enters this widget and rests within it for a short delay,
#  a balloon help window will automatically appear showing the
#  help <mesg>.
# ----------------------------------------------------------------------
proc balloonhelp_for {win mesg} {
    global bhInfo
    set bhInfo($win) $mesg

    bind $win <Enter> {balloonhelp_pending %W}
    bind $win <Leave> {balloonhelp_cancel}
    bind $win <Button-1> {balloonhelp_cancel}
    bind $win <Button-2> {balloonhelp_cancel}    
    bind $win <Button-3> {balloonhelp_cancel}
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_control <state>
#
#  Turns balloon help on/off for the entire application.
# ----------------------------------------------------------------------
set bhInfo(active) 1

proc balloonhelp_control {state} {
    global bhInfo

    if {$state} {
        set bhInfo(active) 1
    } else {
        balloonhelp_cancel
        set bhInfo(active) 0
    }
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_pending <win>
#
#  Used internally to mark the point in time when a widget is first
#  touched.  Sets up an "after" event so that balloon help will appear
#  if the mouse remains within the current window.
# ----------------------------------------------------------------------
proc balloonhelp_pending {win} {
    global bhInfo

    balloonhelp_cancel
    set bhInfo(pending) [after 1000 [list balloonhelp_show $win]]
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_cancel
#
#  Used internally to mark the point in time when the mouse pointer
#  leaves a widget.  Cancels any pending balloon help requested earlier
#  and hides the balloon help window, in case it is visible.
# ----------------------------------------------------------------------
proc balloonhelp_cancel {} {
    global bhInfo

    if {[info exists bhInfo(pending)]} {
        after cancel $bhInfo(pending)
        unset bhInfo(pending)
    }
    wm withdraw .balloonhelp
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_show <win>
#
#  Used internally to display the balloon help window for the
#  specified <win>.
# ----------------------------------------------------------------------
proc balloonhelp_show {win} {
    global bhInfo

    if {$bhInfo(active)} {
        .balloonhelp.info configure -text $bhInfo($win)

        set x [expr [winfo rootx $win]+10]
        set y [expr [winfo rooty $win]+[winfo height $win]]
        wm geometry .balloonhelp +$x+$y
        wm deiconify .balloonhelp
        raise .balloonhelp
    }
    unset bhInfo(pending)
}

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/Scrolled_Listbox.tcl
# 
# Example 30-2
# Scroll_Set manages optional scrollbars.
#

proc Scroll_Set {scrollbar geoCmd offset size} {
	if {$offset != 0.0 || $size != 1.0} {
		eval $geoCmd					;# Make sure it is visible
	}
	$scrollbar set $offset $size
}

#======================================================================
#
# Example 30-3
# Listbox with optional scrollbars.
#

proc Scrolled_Listbox { f args } {
	frame $f
	listbox $f.list \
		-xscrollcommand [list Scroll_Set $f.xscroll \
			[list grid $f.xscroll -row 1 -column 0 -sticky we]] \
		-yscrollcommand [list Scroll_Set $f.yscroll \
			[list grid $f.yscroll -row 0 -column 1 -sticky ns]]
	eval {$f.list configure} $args
	scrollbar $f.xscroll -orient horizontal \
		-command [list $f.list xview]
	scrollbar $f.yscroll -orient vertical \
		-command [list $f.list yview]
	grid $f.list -sticky news
	grid rowconfigure $f 0 -weight 1
	grid columnconfigure $f 0 -weight 1
	return $f.list
}

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/objectSelectionPanel.tcl

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

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/textPanel_show.tcl
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

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/parameterHelp_show.tcl
#==============================================================================
#
#                                  Boda Franek    July 2007
# Copyright Information:
#      Copyright (C) 2005-2007 CCLRC.   All Rights Reserved
#
#    $Id: parameterHelp_show.tcl,v 1.1 2007/07/19 11:00:33 franek Exp $
#==============================================================================
proc parameterHelp_show {} {
#-----------------------------------------------------------------------------
# Creates and displays top level 'text Panel' describing how parameters are
# input into the GUI
#-----------------------------------------------------------------------------
	set textPanelId [textPanel_show Parameters]
	set t [textPanel_text $textPanelId]
	$t configure -width 75 -height 20 -bg white
	$t tag configure head -font {times 14 bold} -justify center

$t insert end "Input of Parameters\n\n" head \
"   There are 3 types of parameters, integers, floating points and strings.
When inputing the parameters ,the following rules apply:

	Integers
	
  These can be input as decimal constants. They can also be input
as octal or hexadecimal constants following the C/C++ rules. An octal
constant is written by preceding the octal value with digit zero.
A hexadecimal constant is written by preceding the value with a zero
and x or X. Negative numbers are preceded with minus sign. Positive
numbers may be preceded by an optional plus sign.
	
	Floating points
	
They must follow the C/C++ rules. It is either simple notation such as
5.67 or scientific notation 4E12   It has to have either decimal point
or E or e.

	
	Strings
	
A string value is input as a contiguous array of characters. It is neither
preceded nor ended with double quote, unless this is part of the string.
Any printable character can be input 'as is'. An exception is backslash
character which has to be input as two consecutive backslash characters.
In analogy with C/C++, double and single quote can be preceded by backslash
character. So <\\\"> is equivalent to <\"> and <\\'> to <'>.
	Any ascii character can be represented by its escape sequence.
This is particularly usefull for inputing nonprinting characters. The
escape sequence consists of backspace character followed by two-digit
hexadecimal code of that character. For example <A> could be represented
as <\\x41>, or <TAB> as <\\X09>.
	In analogy with C/C++, some nonprinting and special characters
have their own special escape sequences. These are:

	<\\a>	alert
	<\\b>	backspace
	<\\r>	carriage return
	<\\f> 	formfeed
	<\\t>	tab
	<\\n>	newline
	<\\v>	vertical tab
	
	 \n"

	return $textPanelId
}

# Sourced File : /usr/clas12/devel/R3.14.12.5/libs/smixx_v47r1-x86_64/src/tclTkGUI/objectSetDisplayPanel.tcl
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

#sourced-files-end===========================================================

proc objectWidgBasic_create { win object widthd widtho } {

	frame $win
		set domobj [split $object :]
		set domain [lindex $domobj 0]
		set name [lindex $domobj 2]
		set lendomain [string length $domain]
		set lenname [string length $name]
		incr lendomain 3
		incr lenname 3
		label $win.domain -width $lendomain -text $domain
        	label $win.name -width $lenname -text $name
        	label $win.state -width 20 -text "dummy"
        	button $win.param -text "Parameters" -width 10 -command "paramPanel_display $object"
        	menubutton $win.act -text "Actions" -width 7 -menu $win.act.menu
        	menu $win.act.menu -postcommand "actionMenu_update $win.act.menu $object "
    
        	pack $win.domain $win.name $win.state $win.act $win.param -side left -padx 2
	return $win
}
#==============================================================================
proc objectWidget_display { win object widthd widtho } {
	global objectX objectY lastDisplayedObject
		
	toplevel $win
		set basic [objectWidgBasic_create $win.basic $object $widthd $widtho ]
		pack $basic	

#	after idle [format {
#		update idletasks
#		set x [getObjectX %s]
#		set y [getObjectY %s]
	
#		wm geometry %s "+$x+$y"
#		} $object $object $win]
	
#	set lastDisplayedObject $object
	
	after idle [format {positionObject %s} $object]
	return $win
}
#===============================================================================
proc positionObject {object} {
	global SCREENWIDTH SCREENHEIGHT
	global objectId objectX objectY lastDisplayedObject
	
	set win $objectId($object)
	
	update idletasks
	set x [getObjectX $object]
	set y [getObjectY $object]
	puts stdout $SCREENHEIGHT
	if { [expr $y +100] > $SCREENHEIGHT } {
		set y 10
		set x [expr $objectX($lastDisplayedObject) +100]
	}
	wm geometry $win "+$x+$y"
	
		
	set objectX($object) $x
	set objectY($object) $y


	set lastDisplayedObject $object


puts stdout "  $object  $x  $y"
	return
}
#==============================================================================
proc getObjectX {object} {
	global SCREENWIDTH SCREENHEIGHT
	global objectX objectY lastDisplayedObject
#	global lastObjectX
	
	if { ![info exists lastDisplayedObject] } {
		set x [expr [winfo rootx . ] + [winfo width .] + 10 ]
	} else {
		set x $objectX($lastDisplayedObject)
	}
	
	return $x
}
#=======================================================================================
proc getObjectY {object} {
	global SCREENWIDTH SCREENHEIGHT
	global objectId objectX objectY lastDisplayedObject
#	global lastObjectY lastObjectHeight
	
	if { ![info exists lastDisplayedObject] } {
		set y [expr [winfo rooty . ]]
	} else {
		set lastObjectY $objectY($lastDisplayedObject)
		set lastObjectHeight [winfo reqheight $objectId($lastDisplayedObject)]
		set y [expr $lastObjectY + $lastObjectHeight + 30]
	}
	return $y
}	
	
#========================================================================================
proc objectWidget_destroy {object} {
	global objectId
	
	set objWidgId $objectId($object)
	
	paramPanel_remove $object
#	destroy $objWidgId
	
	return
}
#========================================================================================
proc paramPanel_display {object} {
	global objectId objectParPanelId
	set top 1
	# this is a private flag indicating wheather the panel is created
	# as toplevel window or part of the object panel
		
        if { [info exists objectParPanelId($object)] } {
		# the existence of the variable means that par panel was displayed
		# before. However it could have self distruct by user pressing 'dismiss
		# so we have to check if the window exists
		set panelId $objectParPanelId($object)
		if { [winfo exists $panelId] } {
			bell
			if { [winfo toplevel $panelId] == $panelId } {
			# it is a top level window
				wm deiconify $panelId
				raise $panelId
				update idletasks
			}
			return
		}
	}
	
	set parameters [getObjectParameters $object]
	if { $parameters == "" } {
		return
	}
	
	if { $top == 0 } {
		set panelId [objectParPanel_create $objectId($object).panel $parameters "Object Parameters" ]
		pack $panelId -side top
	} else {
		set panelId [objectParPanel_display $objectId($object)/panel $parameters "Object $object Parameters" ]
	}

	set objectParPanelId($object) $panelId	
	return
}
#===============================================================================
proc paramPanel_remove {object} {
	global objectId objectParPanelId

		
        if { [info exists objectParPanelId($object)] } {
		# the existence of the variable means that par panel was displayed
		# before. However it could have self distructed by user pressing 'dismiss
		# so we have to check if the window exists
		set winId $objectParPanelId($object)
		if { [winfo exists $winId] } {
			objectParPanel_destroy $winId
			unset objectParPanelId($object)
			return
		}
	}
}
#================================================================================================
proc actionMenu_update {win object} {

	set state [getState $object]
    set actions [getObjectActions $object $state]

    $win delete 0 last

#    $win add command -label "$object - actions"
#    $win add separator
    foreach action $actions {
        $win add command -label $action -command "actionCommand $object $state $action" 
    }
    return
}
#=======================================================================================
proc actionCommand {object state action} {

#tk_messageBox -message "object $object  action $action"
	set parameters [getActionParameters $object $state $action]
#tk_messageBox -message $parameters
	if { [llength $parameters] == 0 } {
		dispatchAction $object $action
		return
	}
	
	actionParPanel_display $object $action $parameters
	return
	
}
#=======================================================================================
proc update_objectWidg { object } {
    global objectId objectParPanelId
    
    set win $objectId($object)
#    $win.basic.state configure -text [getState $object]
    
	if { [info exists objectParPanelId($object)] } {
		set winId $objectParPanelId($object)
		if { [winfo exists $winId] } {
			set parameters [getObjectParameters $object]
			objectParameterGroup_update $winId $parameters
		}	
	}
    
    return
}
#============================================================================    
proc userUpdate { type object } {
    global objectId
    global objPanelWin
    if {![info exists objectId($object)]} {
        puts stdout "Object <$object> is not registered"
	return
    }
#    puts stdout "$type $object"
#    puts stdout [getState $object]
    update_objectWidg $object
    updateObjectOnCanvas $objPanelWin $object [getState $object]
    return
}
#==========================================================================   
proc addObject {objNameIn} {
	global regObjectsList 
	global objectId
	global objPanelWin
	
	set objName [string toupper $objNameIn]
	lappend regObjectsList $objName
# widget ID must not contain a double colon. In Tk this causes problems
# so I will take one : out
	set pos [string first "::" $objName]
	set objWidgId .object_[string replace $objName $pos $pos ""]

        set objectId($objName) $objWidgId
	
#	objectWidget_display $objWidgId $objName 0 0
#	wm protocol $objWidgId WM_DELETE_WINDOW "removeObject $objName"

	addObjectToCanvas $objPanelWin $objName	
	
	set flg [registerObject $objName]
	if { $flg == "1" } {
#		update_objectWidg $objName
		updateObjectOnCanvas $objPanelWin $objName [getState $objName]
	}
	return
}
#===========================================================================
proc removeObject {object} {
	global regObjectsList 
	global objectId
	
	objectWidget_destroy $object

	unset objectId($object)
	
	set inx [lsearch $regObjectsList $object]
	
	set regObjectsList [lreplace $regObjectsList $inx $inx]

	return
}
#=========================================================================== 
#===========================================================================
#======================== G L O B A L S ====================================
# Node address where DIM Name Server is running 
global DIM_DNS_NODE
set DIM_DNS_NODE {}

# Platform on which this GUI is running
global platform
set platform $tcl_platform(platform)
if { $platform != "windows" && $platform != "unix" } {
    tk_messageBox -message "Unsupported Platform $platform"
    exit
}


# List of full names of registered objects 
global regObjectsList

# Widget id of an object objectId('obj-name')
global objectId

# Widget id of object parameters panel objectParPanelId('obj-name')
global objectParPanelId

global actionParPanelid
global actionParPanelAction actionParPanelObject
global actionParPanelidAnswer


# widgets for letter 'A', 'P' and 'X'
global imA
set imA [image create photo -file $GUIdir/A.gif]
global imP
set imP [image create photo -file $GUIdir/P.gif]
global imX
set imX [image create photo -file $GUIdir/X.gif]


#---------------- Use LHCb colour conventions for some states ---------
global lhcb-green lhcb-blue lhcb-yellow lhcb-orange lhcb-red
global stateColor

set lhcbgreen  "#00CC99"
set lhcbblue   "#0099FF"
set lhcbyellow "#FFFF66"
set lhcborange "#FF9900"
set lhcbred    "#FF0000"

set stateColor(UNKNOWN) $lhcborange
set stateColor(NOT_READY) $lhcbyellow
set stateColor(READY) $lhcbblue
set stateColor(RUNNING) $lhcbgreen
set stateColor(OFF) $lhcbblue
set stateColor(ON) $lhcbgreen
set stateColor(ERROR) $lhcbred
set stateColor(PAUSED) $lhcbblue
set stateColor(CONFIGURING) $lhcbyellow
set stateColor(RAMPING) $lhcborange
set stateColor(STANBY) $lhcbblue
set stateColor(EMERGENCY) $lhcbred
#-------------------------------------------------------------------------

###  These 2 may not be needed, when some redundant code is in future deleted
global SCREENWIDTH SCREENHEIGHT

set SCREENWIDTH [winfo screenwidth .]
set SCREENHEIGHT [winfo screenheight .]

#puts stdout $SCREENHEIGHT


#===========================================================================
#                      Start of the execution
#===========================================================================
wm protocol . WM_DELETE_WINDOW exitProcedure

#----------------------  Display the Main Panel ----------------------------

set topBarId [topbar_create .topbar]
pack $topBarId -anchor w -fill x

set dnsinfoId [dnsinfo_create .dns]
pack $dnsinfoId -fill x

set mainPanelId [mainPanel_create .mainPanel]
pack $mainPanelId  -pady 10

global objPanelWin
set objPanelWin [objectPanel_create .objPanel]
pack $objPanelWin -fill both -expand 1


wm geometry . +10+10
update 
#----------------------------------------------------------------------------

#------------------ Establish contact with DIM Server ----------------------
if {[info exists env(DIM_DNS_NODE)]} {
	set DIM_DNS_NODE $env(DIM_DNS_NODE)
} else {
	set ans [simpleEntry_show \
"Environmental variable DIM_DNS_NODE is undefined
Enter the correct value bellow or exit" ok Exit]
	if { $ans == "" } {
	  	exit
	}
	set env(DIM_DNS_NODE) $ans
	set DIM_DNS_NODE $env(DIM_DNS_NODE)
}

set DNSRUNNING [exec dnsRunning]

while { "1" == "1" } {
	if { $DNSRUNNING == "YES" } {
		break
	}
	set ans [simpleEntry_show \
" There is no DNS running on $DIM_DNS_NODE
Enter an alternative DIM_DNS_NODE" ok Exit]
	
	if { $ans == "" } {
		exit
	}
	set env(DIM_DNS_NODE) $ans
	set DIM_DNS_NODE $env(DIM_DNS_NODE)
	set DNSRUNNING [exec dnsRunning]
}

dnsinfo_update $dnsinfoId "DNS is running on $DIM_DNS_NODE"
#-------------------------------------------------------------------------

#------------------- S t a r t i n g   M o n i t o r i n g -------------
startMonitoring
#-----------------------------------------------------------------------

