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
