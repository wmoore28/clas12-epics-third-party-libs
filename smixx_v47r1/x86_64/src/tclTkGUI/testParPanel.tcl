#! /usr/bin/wish -f
# 
source actionParPanel_display.tcl
source parameterGroups.tcl
source $env(SMIDIR)/src/utilities/efftcl/dialog.tcl
source dispatchAction.tcl

#=====================================================================
proc settingsButton_createNew {win} {
    global infoGrey hel10font 

    
    button $win -text "Settings"  -relief flat \
  -command {actionParPanel_display BODA::OBJECT GOTO_FIRST {{APAR S abcd} {BPAR I {}} {CPAR F 1.78}} }
    
    return $win
}
#==================================================================
#==================================================================
  
   set settingsIdNew [settingsButton_createNew .settingsNew]

    pack $settingsIdNew -side left -padx 8
