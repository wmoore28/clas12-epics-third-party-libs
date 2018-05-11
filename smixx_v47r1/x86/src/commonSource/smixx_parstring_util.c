/*======================== Parametr String handling functions ==========================

                                                      Author: Boda Franek
                                                      Date : July 2006
 Copyright Information:
      Copyright (C) 1996-2006 CCLRC. All Rights Reserved.
========================================================================================*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
/* C special characters that have to be escaped by \  */
static char special_C[] =           {'\a','\\','\b','\r','\"','\f','\t','\n','\'','\v','\0'}; 
static char special_indicator_C[] = { 'a','\\', 'b', 'r','\"', 'f', 't', 'n','\'', 'v','\0'};
 /* the character following \  */

/* characters that needs also escaping for shipping inside the SMI world */
static char special_SMI[] = {'\"','/', '|', '\0' };

/*====================================================================================*/
int smixx_check_special_C(char ch, char* esc_seq)
/* The function checks if the character is either non-printable or one of the special
   C characters. If no, return value is 0. If yes, return value is the length of 
   the escape sequence and esc_seq is its escape sequence. Its length is either 2 or 4 */
{
	char* pos; /* pointer to the special character in special_C array */
	int npos;  /* position in the array */
	char charind; /* the character indicator */
	
	pos = strchr(special_C,ch) ;
	if ( pos != NULL )
	{  /* it is one of the special characters */
		npos = pos - &special_C[0];
		charind = special_indicator_C[npos];
                esc_seq[0] = '\\'; esc_seq[1] = charind;
		return 2;
	} 	
/* check if non-printable */
	if ( !isprint(ch) ) 
	{
		esc_seq[0] = '\\';
		sprintf(&esc_seq[1],"%03o",ch);
		return 4;
	}
	return 0;
}
/*====================================================================================*/
int smixx_get_octal_code(char* ptr, char* code)
/*    the function will scan the input string in order to collect
      octal number. The scan will be terminated either by
         - a character that is not an octal digit
      or - the number of digits collected reached 3 
      On return the function value is the number of digits collected.
      When the resulting number is not an ASCII character,
      the function returns  -1.
      
      Input :   ptr pointer to the first character of the input string
      
      Outpu : code ...ASCII code
        
*/
{  
    int i;
    char ch;
    char buf[4];
    int ndig;
    int icode;
    
    for ( i=0; i<3; i = i + 1)
    {
        ch = *(ptr + i);
	if (!isdigit(ch) || ch == '8' || ch == '9' || ch == '\0')
	{
	    buf[i] = '\0'; break;
	}
	buf[i] = ch;
    }
    if ( i == 3 ) { buf[3] = '\0'; }
    ndig = strlen(buf);
    if ( ndig == 0 )
    {
        printf(" *** Error   no octal digits\n");
        return -1;
    }
    
    sscanf(buf,"%o",&icode);
    if ( icode > 0177 ) 
    {
        printf(" *** Error  octal escape sequence out of range\n");
        return -1;
    }
    *code = icode;
    return ndig;
}
/*====================================================================================*/
int smixx_get_hex_code(char* ptr, char* code)
/*    the function will scan the input string in order to collect
      hex number beginning with x. The scan will be terminated either by
         - a character that is not a hex digit
      or - the number of digits collected reached 2 
      On return the function value is the number of characters collected.
      When the resulting number is not an ASCII character,
      the function returns  -1.
      
      Input :   ptr pointer to the first character (x) of the input string
      
      Outpu : code ...ASCII code
        
*/
{  
	int i;
	char ch;
	char buf[5];
	int nchar;
	int icode;
    
	if ( *ptr != 'x' )
	{
		printf(" *** missing first x \n");
		return -1;
	}
    	
	buf[0] = '0'; buf[1] = 'x';
	
	for ( i=1; i<3; i = i + 1)
	{
		ch = *(ptr + i);
		if (!isxdigit(ch) )
		{ /* not hex digit */
			buf[i+1] = '\0'; break;
		}
		buf[i+1] = ch;
    }
    if ( i == 3 ) { buf[4] = '\0'; }
    nchar = strlen(buf) -1; /* the leading 0 is extra so have to subtract 1*/
    if ( nchar < 2 )
    {
        printf(" *** Error   no hex digits\n");
        return -1;
    }
    
    sscanf(buf,"%x",&icode);
    if ( icode > 0x7F ) 
    {
        printf(" *** Error  hex escape sequence out of range\n");
        return -1;
    }
    *code = icode;
    return nchar;
}
/*====================================================================================*/
int smixx_decode_escape_sequence(char* ptr, char* charcode)
{
/* the function decodes the escape sequence . For illegal sequence it returns -1.
    Otherwise it returns number of characters in the escape sequence (incl the first \ )
    
    Input :  ptr  the pointer to the first  \ 
    
    Output : charcode   the code of the decoded character */
	
	char chnext;  /* the first character after the backslash */
	char* pos; /* pointer to the indicator in special_indicator_C*/
	int npos;  /* position in the array*/
	int numoct; /* num of octal digits in the sequence */
	int numhex; /* num of hex digits incl the first x */

	chnext = *(ptr+1);
	
	pos = strchr(special_indicator_C,chnext) ;
	
	if ( pos != NULL )
	{  /* it is one of the special indicators */
		npos = pos - &special_indicator_C[0];
		*charcode = special_C[npos];
		 return 2;
	} 
	
	
	if ( chnext == 'x' ) 
	{    /* hex code */
		numhex = smixx_get_hex_code(ptr+1, charcode); 
		if ( numhex < 0 ) {return -1;}
		return numhex+1;

	}
	else if ( isdigit(chnext) &&  chnext != '8' && chnext != '9' )
	{    /*octal code*/		 
		numoct = smixx_get_octal_code(ptr+1, charcode); 
		if ( numoct < 0 ) {return -1;}
		return numoct+1;
	}	
	

	return -1; 
	
}
/*================================================================================*/
int smixx_check_special_SMI(char ch, char* esc_seq)
{
/* The function checks if the character ch is special SMI character. If not,
   it returns zero. If yes, then it returns 4 and the esc_seq is set to its
   octal escape sequence, whose length is always 4. e.g. character " is escaped
   as \042 */
  
	char* pos;
	int npos;
	
	pos = strchr(special_SMI,ch);

	if ( !pos ) { return 0; }
	
	npos = pos - &special_SMI[0];
	
	esc_seq[0] = '\\';
	sprintf(&esc_seq[1],"%03o",special_SMI[npos]);
	return 4;
}
/*===============================================================================*/
char* smixx_outchars(char* ptrout, char* chars, int nchar, char* ptrout_max) 
{
/*  the function output nchar characters from the string being pointed to 
    by chars into a string being pointed to by ptrout. ptrout_max is the last
    position into which it is allowed to write.
    The return value is the pointer to the following character in the output string 
    after chars were copied */
    
	char* newptrout;
	
	newptrout = ptrout + nchar;
	if ( (newptrout-1) > ptrout_max ) 
	{
		printf(" Error    output string is not big enough\n");	
	 	return NULL;
	}
	
	strncpy(ptrout,chars,nchar);
	
	
	return newptrout;
}
/*=============================================================================*/
int smixx_escape_slash(char* outstr, char* instr, int max)
{
	int inlen;	/* number of characters in the input string */
	int nout;	/* number of characters in the output string */
	char* ptrin;
	char* ptrout;
	char ch;
	int iin;	/* index to the input string */  
	char* outstr_max;
	int ncescape; /*number of characters in a escape sequence, incl. the first\ */
	char esc_seq[5];
	int icode;
	char charcode;

/*for (i=0; i<10; i=i+1) {
printf("%o\n",spec[i]); 
}*/
	outstr_max = outstr + max - 1;
	
	inlen = strlen(instr);
	if ( inlen == 0 ) 
	{
		if ( outstr != NULL ) {*outstr='\0';}
		return 0;
	}
    
    /* check if string is enclosed in double quotes - these will be preserved*/
	if ( (   (*instr == '\"') && ( *(instr+inlen-1) != '\"') ) ||
	     (   (*instr != '\"') && ( *(instr+inlen-1) == '\"') )      )
	{
		printf(" *** error   string has unbalanced double quotes\n");
		return -1;
	}		
		
	ptrout = outstr;
	nout = 0;
	
	for ( iin = 0; iin < inlen; iin = iin + 1 )
	{
		ptrin = instr + iin;
		ch = *ptrin;         /*printf("%c\n",ch);*/
				
		if ( (iin == 0 || iin == (inlen-1)) && ch == '\"' )
		{ /* first or last doublequote, just keep it*/
			nout = nout + 1;
			if ( outstr != NULL )
			{ 
				ptrout = smixx_outchars(ptrout, "\"", 1, outstr_max);
				if ( !ptrout ) {return -1;}
			}
			continue;

		}
		if ( smixx_check_special_C(ch,esc_seq) && ch != '\\')
		{  /* unescaped special character */
			printf(" *** error   string has un-escaped special or un-printable character\n");
			return -1;
		}	

		if ( ch == '\\' )
		{ /* begining of escape sequence*/
			ncescape = smixx_decode_escape_sequence(ptrin,&charcode);
			if ( ncescape < 0 )
			{
				printf(" *** error  string has an illegal escape sequence \n");
				return -1;
			}
			/* OK escape sequence */
			if ( *(ptrin+1) == '\"' )
			{ /* we have the sequence \" which we want to replace by oct esc sequence */
				iin = iin + 1;
				nout = nout + 4;
				if ( outstr != NULL )
				{ 
					ptrout = smixx_outchars(ptrout, "\\042", 4, outstr_max);
					if ( !ptrout  ) {return -1;}
				}
				continue;
			}
			iin = iin + ncescape - 1;
			nout = nout + ncescape;
			if ( outstr != NULL )
			{ 
				ptrout = smixx_outchars(ptrout, ptrin, ncescape, outstr_max);
				if ( !ptrout ) {return -1;}
			}
			continue;
		}
		icode = smixx_check_special_SMI(ch,esc_seq);
		if ( icode > 0 ) 
		{ /* special SMI character, has to be escaped */
			nout = nout + 4;
			if ( outstr != NULL )
			{ 
				ptrout = smixx_outchars(ptrout, esc_seq, 4, outstr_max);
				if ( !ptrout  ) {return -1;}
			}
			continue;
		}
		/* just some inocuous character */
		nout = nout + 1;
		if ( outstr != NULL )
		{ 
			ptrout = smixx_outchars(ptrout, ptrin, 1, outstr_max);
			if ( !ptrout ) {return -1;}
		}
		continue;
	}
		
	if (  outstr != NULL ) {*ptrout='\0';}
	return nout;
}
/*=============================================================================*/
int smixx_unescape(char* outstr, char* instr, int max)
{
	int inlen; /* number of characters in the input string */
	int numin; /* this is either inlen, or inlen-2 if the input string is enclosed in doublequotes */

	int nout; /* no of characters in the output string */
	
	char* ptrin;
	char* ptrout;
	char* outstr_max;
	
	char ch;
	int icode;
	int iin;  /* counter of input processed characters ... for index */
	char* ptrstart;
	char charcode;
	char esc_seq[5]; int ncescape;

/*for (i=0; i<10; i=i+1) {
printf("%o\n",spec[i]); 
}*/

	outstr_max = outstr + max - 1;
	
	inlen = strlen(instr);
	if ( inlen == 0 ) 
	{
		if ( outstr != NULL ) {
		*outstr = '\0';
		}
		return 0;
	}
	
	ptrstart = instr;
	numin = inlen;


    /* check if string is enclosed in double quotes - these will be taken out*/
	if ( (   (*instr == '\"') && ( *(instr+inlen-1) != '\"') ) ||
	     (   (*instr != '\"') && ( *(instr+inlen-1) == '\"') )      )
	{
		printf(" *** error   string has unbalanced double quotes\n");
		return -1;
	}		
    
	if ( *instr == '\"' )
	{
		ptrstart = instr + 1;	/* starting with the character following the " */
		numin = inlen - 2;	/* ending the character before the last " */
	} 
        
	ptrout = outstr;
	nout = 0;
    
	for ( iin = 0; iin < numin; iin = iin + 1) {
		ptrin = ptrstart + iin;
		ch = *ptrin;
		
		icode = smixx_check_special_SMI(ch,esc_seq);
		if (icode)
		{
			printf(" *** error the string has unescaped special SMI characters\n");
			return -1;
		}	
		if ( ch == '\\' )
		{ /* begining of escape sequence*/
			ncescape = smixx_decode_escape_sequence(ptrin,&charcode);
			if ( ncescape < 0 )
			{
				printf(" *** error  string has an illegal escape sequence \n");
				return -1;
			}
			/* OK escape sequence */
			iin = iin + ncescape - 1;
			nout = nout + 1;
			if ( outstr != NULL )
			{ 
				ptrout = smixx_outchars(ptrout, &charcode, 1, outstr_max);
				if ( !ptrout ) {return -1;}
			}
			continue;
		}
		icode = smixx_check_special_C(ch,esc_seq);
		if (icode)
		{
			printf(" *** error the string has unescaped special C characters\n");
			return -1;
		}
		/* just some inocuous character */
		nout = nout + 1;
		if ( outstr != NULL )
		{ 
			ptrout = smixx_outchars(ptrout, ptrin, 1, outstr_max);
			if ( !ptrout ) {return -1;}
		}
		continue;
	}
	if ( outstr != NULL ) {
		*ptrout = '\0';
	}
	return nout;
}
/*=============================================================================*/
int smixx_escape(char* outstr, char* instr, int max)
{
	int nout;
	char* ptrin;
	char* ptrout;
	char ch;
	int inlen; /* number of characters in the input string */ 
	int iin;  /* counter of input processed characters ... for index */
	char* ptrstart;
	char esc_seq[5]; int nesc;
	int icode;
	char* outstr_max;

/*for (i=0; i<11; i=i+1) {
printf("%c   %o\n",spec[i],codespec[i]); 
}
return 0;*/
	outstr_max = outstr + max - 1;
	inlen = strlen(instr);
	if ( inlen == 0 ) 
	{
		if ( outstr != NULL ) { *outstr = '\0'; }
		return 0;
	}
    
	ptrstart = instr;
        
	ptrout = outstr;
	nout = 0;
    
	for ( iin = 0; iin < inlen; iin = iin + 1) 
	{
		ptrin = ptrstart + iin;
		ch = *ptrin;
		icode = smixx_check_special_SMI(ch,esc_seq);
		if ( icode > 0 ) 
		{ /* special SMI character, has to be escaped */
			nout = nout + 4;
			if ( outstr != NULL )
			{ 
				ptrout = smixx_outchars(ptrout, esc_seq, 4, outstr_max);
				if ( !ptrout  ) {return -1;}
			}
			continue;
		}
		nesc = smixx_check_special_C(ch,esc_seq);
		if (nesc) 
		{ /* special C or unprintable character */
			nout = nout + nesc;
			if ( outstr != NULL )
			{ 
				ptrout = smixx_outchars(ptrout, esc_seq, nesc, outstr_max);
				if ( !ptrout  ) {return -1;}
			}
			continue;
		}
		/* just some inocuous character */
		nout = nout + 1;
		if ( outstr != NULL )
		{ 
			ptrout = smixx_outchars(ptrout, ptrin, 1, outstr_max);
			if ( !ptrout ) {return -1;}
		}
		continue;

	}
	if ( outstr != NULL ) {
		*ptrout = '\0';
	}
    return nout;
}
/*=============================================================================*/
/*
int main()
{
    int code, i, j;
    int bytes_read;
    int nbytes = 100;
    char my_string[5];
    char testString;
    int noutmax = 10;
    char outString[noutmax+1];
    int nout;
 
    my_string[0] = 'a'; my_string[1] = 'b'; my_string[2] = '\164'; my_string[3] = 'c'; my_string[4] = '\0';
    
	for ( j=0; j<31; j = j + 1)
	{
		printf("\n\n ====================================================\n");
		for ( i=0; i<strlen(my_string); i++) {
			printf("%o\n",my_string[i]);
		}
    
		nout = smixx_escape(NULL, my_string,0);
    
		printf(" There would be %d characters in the output string\n", nout);
    
		if (nout <= 0) return 0;
    
		nout = smixx_escape(outString, my_string,noutmax);
		printf ("Output string (%d):\n \n",nout);
   
		
		for (i=0; outString[i] != 0; i = i + 1) {
			printf("%c",outString[i]);
		}
		printf("\n");
		
		for (i=0; outString[i] != 0; i = i + 1) {
			printf("%o ",outString[i]);
		}
		printf("\n");
		
		my_string[2] = my_string[2] + 1;
	}
    
    return 0;
}

*/

/*
int main()
{
    int code, i;
    int bytes_read;
    int nbytes = 100;
    char *my_string;
    char testString;
    int noutmax = 10;
    char outString[noutmax+1];
    int nout;
  
    puts ("Enter string:");
      
    my_string = (char *) malloc (nbytes + 1);
    bytes_read = getline (&my_string, &nbytes, stdin);
    my_string[strlen(my_string)-1] = '\0';
    
    puts ("You typed:");
    puts (my_string);
    
    for ( i=0; i<strlen(my_string); i++) {
        printf("%o\n",my_string[i]);
    }
    
    nout = smixx_unescape(NULL, my_string,0);
    
    printf(" There would be %d characters in the output string\n", nout);
    
    if (nout <= 0) return 0;
    
    nout = smixx_unescape(outString, my_string,noutmax);
    printf ("Output string (%d):\n",nout);
   
    for (i=0; outString[i] != 0; i = i + 1) {
        printf("%o     %c\n",outString[i],outString[i]);
    }
    
    return 0;
}
*/

/*
int main()   
{ 
    int code, i;
    int bytes_read;
    int nbytes = 100;
    char *my_string;
    char testString;
    int nout;
    char outstring[11];
  
    puts ("Enter string:");
      
    my_string = (char *) malloc (nbytes + 1);
    bytes_read = getline (&my_string, &nbytes, stdin);
    my_string[strlen(my_string)-1] = '\0';
    
    puts ("You typed:");
    puts (my_string);
    
    for ( i=0; i<strlen(my_string); i++) {
        printf("%o\n",my_string[i]);
    }
    
    nout = smixx_escape_slash(NULL,my_string,0);
    printf ("Output string string length (%d):\n",nout);
    
    nout = smixx_escape_slash(outstring, my_string,8);
    
    
    printf ("Output string string (%d):\n",nout);
   
    for (i=0; i < nout; i = i + 1) {
        printf("%o     %c\n",outstring[i],outstring[i]);
    }
    
    return 0;
}

*/
