option add *Gauge.borderWidth 2 widgetDefault
option add *Gauge.relief sunken widgetDefault
option add *Gauge.length 200 widgetDefault
option add *Gauge.color gray widgetDefault
 
proc gauge_create {win {color ""}} {
    frame $win -class Gauge
    
    set len [option get $win length Length]
    canvas $win.display -borderwidth 0 -background white \
         -highlightthickness 0 -width $len -height 20
    pack $win.display -expand yes
    
    if {$color == ""} {
       set color [option get $win color Color]
    }
    
    $win.display create rectangle 0 0 0 20 \
         -outline "" -fill $color -tags bar
	 
    $win.display create text [expr 0.5*$len] 10 \
          -anchor c -text "0%" -tags value
	  
    return $win
}
proc gauge_value {win val {color ""}} {
    if { $val < 0 } {
        puts stdout "bad value for gauge"
	return
    }
    
    set msg [format "%3.0f%%" $val]
#    puts stdout $msg
    $win.display itemconfigure value -text $msg
    if { $color != "" } {
        $win.display itemconfigure bar -fill $color
    }
    
    set w [expr 0.01*$val*[winfo width $win.display]]
    set h [winfo height $win.display]
    $win.display coords bar 0 0 $w $h
    
    update
}

