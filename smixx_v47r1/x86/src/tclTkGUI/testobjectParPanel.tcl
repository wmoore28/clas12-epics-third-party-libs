#! /usr/bin/wish -f
# 
source objectParPanel_create.tcl
source parameterGroups.tcl

#=====================================================================
proc settingsButton_createNew {win} {
    global infoGrey hel10font 

    
    button $win -text "display"  -relief flat \
  -command {display}
    
    return $win
}
#==================================================================
proc display {} {
	global wintop
#	  set panelId [objectParPanel_create $wintop.panel  {{APAR S abcd} {BPAR I {}} {CPAR F 1.78}} Hovno]	
#	  pack $panelId -side top
	  set panelId [objectParPanel_display .parms  {{APAR S abcd} {BPAR I {}} {CPAR F 1.78}} Hovno]	
	puts stdout [winfo toplevel $panelId]
	puts stdout $panelId
}
#==================================================================
  
   set settingsIdNew [settingsButton_createNew .settingsNew]
   
   global panelId
   global wintop
#    set wintop .object_BODA::OBJECT
 
     set wintop .tt:tt  
    toplevel $wintop
    	frame $wintop.f
    		label $wintop.f.l1 -text XXXXXXXX
    		set but [settingsButton_createNew $wintop.f.but]
		pack $wintop.f.l1 $but -side left
		
 
 	pack $wintop.f
