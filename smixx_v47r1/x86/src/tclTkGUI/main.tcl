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

source $GUIdir/actionParPanel_display.tcl
source $GUIdir/objectParPanel_create.tcl
source $GUIdir/parameterGroups.tcl
source $GUIdir/../utilities/efftcl/my_dialog.tcl
source $GUIdir/../utilities/efftcl/confirm.tcl
source $GUIdir/simpleEntry_show.tcl
source $GUIdir/topbar.tcl
source $GUIdir/mainPanel.tcl
source $GUIdir/listBox_create.tcl
source $GUIdir/selectFromListPanel_show.tcl
source $GUIdir/convertHEXtoEASCII.tcl
source $GUIdir/tcl-smi.tcl
source $GUIdir/dnsinfo.tcl
source $GUIdir/canvasObjects.tcl
source $GUIdir/../utilities/efftcl/balloon1.tcl
source $GUIdir/Scrolled_Listbox.tcl
source $GUIdir/objectSelectionPanel.tcl
source $GUIdir/textPanel_show.tcl
source $GUIdir/parameterHelp_show.tcl
source $GUIdir/objectSetDisplayPanel.tcl
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
