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
