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
