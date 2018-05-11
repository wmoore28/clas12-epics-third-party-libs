/*======================== Parametr String handling functions ============

                                                   Author: Boda Franek
                                                   Date : July 2006
 Copyright Information:
      Copyright (C) 1996-2006 CCLRC. All Rights Reserved.
==========================================================================*/

#ifndef SMIXX_PARSTRING_UTIL
#define SMIXX_PARSTRING_UTIL
/*=============================================================================*/
int smixx_escape_slash(char* outstr, char* instr, int max);
/*  Description:
   The function is meant to process string parameters encountered in SML code.
   They should follow the normal C/C++ rules for escaping non-printable and
   other special characters. In addition to these, there are other special
   characters with a special meaning in SMI system and so these have to be
   protected by escaping. They are:
   '/', '|',  '=',  '(', ')' and ' '. They are escaped by \ followed by
   their octal code.
   Extra special case is doublequote. This is already escaped in the input
   string by \", but this is not enough and \" has to be converted to \042.
   

  At the same time it also checks that string does not contain
  any non-printable characters. If it does, it returns -1.
   
    Input:
      instr ... pointer to the input string.
	      
      max ... max no of characters allowed to be output into outstr. When exceeded,
              message is printed and zero returned. max is ignored when
	      outstr == NULL
      
    Output:
      outstr ... pointer to the resulting string.
      		when outstr == NULL, then there is no output string. However,
		the number of characters in the potential output string
		is calculated and returned
			
      return value ... when everything is ok, number of output characters. Otherwise
                       -1.
*/
int smixx_unescape(char* outstr, char* instr, int max);
/*
  The function processes the input string and unescapes all the escape sequences
 Input:
      instr ... a string that is assumed to contain only escaped special characters.
               (both C and SMI). Also the unprintable characters must be
	       escaped, If a solitary special or unprintable character is found,
	        -1 is returned
	      If instr string contains enclosing ", these will be removed.
	      
      max ... max no of characters allowed to be output into outstr. When exceeded,
              message is printed and -1 returned. Do not forget to reserve space for
	      terminating zero. max is ignored when outstr == NULL
      
   Output:
      outstr ... resulting unescaped string
      		when outstr == NULL, then there is no output string. However,
		the number of characters in the potential output string
		is calculated and returned
      
      return value ... when everything ok, number of output characters. Otherwise
                       -1 is returned.
*/
/*=============================================================================*/
int smixx_escape(char* outstr, char* instr, int max);
/* Input:
      instr ... a string that is assumed to contain only the original characters.
	      When the value of the character is greater then 127, -1 is returned and 
	      a message printed.
	      
      max ... max no of characters allowed to be output into outstr. When exceeded,
              message is printed and -1 returned. Do not forget to reserve space for
	      terminating zero.max is ignored when outstr == NULL
      
   Output:
      outstr ... resulting smixx (in addition to C escaping, / is escaped by \s) escaped
                 string.
		when outstr == NULL, then there is no output string. However,
		the number of characters in the potential output string
		is calculated and returned
		
      return value ... when everything ok, number of output characters. Otherwise
                       -1.
*/
#endif
