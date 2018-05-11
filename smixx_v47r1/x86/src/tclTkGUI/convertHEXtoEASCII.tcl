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
