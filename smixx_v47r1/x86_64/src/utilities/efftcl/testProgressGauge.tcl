
source dialog.tcl
source notice.tcl
source progressGauge.tcl
#============================= Example  =====================================
proc updateScript {idGauge} {
	global i idNotice
	
	incr i
        if { $i >= 100} {
		set cntls [dialog_controls $idNotice]
		set info [dialog_info $idNotice]
		pack forget $idGauge
		pack $cntls.dismiss
		$info.mesg configure -text Finished 
		return
	}	
	gauge_value $idGauge $i
        after 1000 [list updateScript $idGauge]
        return
}
global idNotice
set idNotice [notice_show " Starting process"]
set cntls [dialog_controls $idNotice]
pack forget $cntls.dismiss

#global idGauge
set idGauge [gauge_create $cntls.gauge PaleGreen]
pack $idGauge -expand yes -fill both -padx 10 -pady 10

global i
set i 0
#after 1000 [list updateScript $idGauge]
update
gauge_value $idGauge 10 Blue
after 5000
gauge_value $idGauge 20 Yellow
after 1000
gauge_value $idGauge 30 Pink
after 1000
gauge_value $idGauge 40
after 1000
gauge_value $idGauge 50
after 1000
gauge_value $idGauge 120 Red
