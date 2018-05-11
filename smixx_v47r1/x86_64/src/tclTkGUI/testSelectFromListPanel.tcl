#! /usr/bin/wish -f
# 
source listBox_create.tcl
source $env(SMIDIR)/src/utilities/efftcl/my_dialog.tcl
source selectFromListPanel_show.tcl

#=====================================================================
proc settingsButton_createNew {win} {
    global infoGrey hel10font 

    
    button $win -text "Settings"  -relief flat \
  -command "selectFromListPanel_show Domains {DOMAIN1 DOMAIN2 DOMAIN3 DOMAIN4 DOMAIN5 DOMAIN6} "
    
    return $win
}
#==================================================================
  frame .d
  
   set settingsIdNew [settingsButton_createNew .d.settingsNew]

    pack $settingsIdNew -side left -padx 8
    
    pack .d

#	set listBoxId [listBox_create .d.listBox Test]
#		set listBoxId [label $info.listBox -text hey]
#	pack $listBoxId
	
