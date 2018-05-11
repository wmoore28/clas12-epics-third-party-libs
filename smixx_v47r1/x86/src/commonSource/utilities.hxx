#include "typedefs.hxx"
void process_line_forTrans(char str[]);
void strNtoONEBlank(char str[]);
   void strSqueeze(char str[]);
   void strTrim(char str[]);
   void strUpcase(char str[]);
   void process_line(char str[]);
   #include "smixx_common.hxx"
   class Name;
   class SMLlineVector;

   int specialCode(int nl, Name& line);
   int readLine(ifstream& file, Name& line);

   char* getToken(char str[], char delim, Name& token );


   bool check_name( const Name&);
   bool check_int(const Name&);
   bool check_float(const Name&);
   bool check_string(const Name&);
   bool check_compname( const Name& );

// This is a bodge for NT
   bool my_strcasecmp(const char str1[], const char str2[]);

/**
  The function searches for the first non-blank (or non-TAB) character in
  a given string. It returns the pointer to such character or if not found,
  the pointer to the terminating zero.
*/
   char* firstNonBlankPtn(char str[]);

/**
  The function searches for the first non-blank (or non-TAB) character in a segment
  of SML code.
  It takes as input:
       - pointer 'pSMLcode' which points to a number of SML lines. 
            Individual characters within this code are addressed
            by pair of integers (i,j), i being the line number (starting with 0)
	    and j being the character number (starting with 0) of
	    the referenced character on the ith line.
	
	- ist,jst ... address of the starting character within the code
	   
     output:
        - inbl,jnbl ... address of the first non-blank character if found. If not
	                found, it is undefined. (little messy)
			
	- inext,jnext ... address of the next character after the FNB chacter found.
	                  If FNB character not found, it is set to -1,0.
			  If the FNB character is the last character in the code 
			  segment, it also sets it to -1,0
			  
	- iprev,jprev ... address of the character before before the FNB when found.
	                  If not found, or FNB is the first character in the code
			  segment, it is set to -1,0
			  
     It returns the value of FNB found, or zero.
     
     In all the above, the end of a line is considered to be either
     the terminating zero or '!' or '#'
*/
   char firstNonBlank(SMLlineVector* pSMLcode,
		      const int ist,const int jst,
		      
		      int& inbl, int& jnbl, int& inext, int& jnext,
		      int& iprev, int& jprev);


   bool strncasecmp(char str1[], const char str2[]);

	char* getType(char str[],char* type);

//	char* getNextToken(char str[], const char del[], Name& name);

/**
  Input:
     - str[]  ... the function searches within the 'str' string for
                  a substring (token) of certain properties (see below).
		  
     - del[]  ... list of delimiters.
                 NB: end string character is automaticaly
                     considered as one of the delimiters
     
  Output:
     - token  ... output string
     
     In what follows:
      "space" = "space or tab"
     
     The behaviour of the function is somewhat different when "space"
     is one of the delimiters.
     
     In what follows, let us denote
        by "b" "space" character 
        by "d" one of the delimiters (but not space)    
	by "z" "space" character when space is one of the delimiters
	by "X" any character except "d", i.e. including spaces.
	by "Y" any character except "d" and space.
	by "NB" any character except "space", i.e. non blank character
	
     At the beginning all the initial spaces are skipped and the
     first non-blank character found. If this is one of the delimiters, the 
     token is set to "\0" and function returns pointer to the delimiter.
     Then there are two cases:
       1)  "space" is not one of the delimiters:
                        bbb..bXXXX..Xd....
          in this case the string is searched until a delimiter is found,
	  token is set to 'XXXX..X' and function returns pointer to
	  the delimiter
                 
       2)  "space" is one of the delimiters:
             a) first delimiter found is not "space"
                         zzz..zYYYY..Yd....
		in this case token is set to 'YYYY..Y' and functin returns
		pointer to the delimiter
		
	     b) first delimiter found is "space"
		 b1)     zzz..zYYYY..Yzzz..zzd..
		     i.e. the first non-blank character following is "d"
		     in this case the token is set to 'YYYY..Yzzz..zz'
		     and function returns pointer to "d". i.e. d takes
		     the precedence over z.
		     
		 b2)     zzz..zYYYY..Yzzz..zzY..
		     i.e. the first non-blank character is Y 
		     in this case the token is set to 'YYYY..Yzzz..z'
		      and function returns pointer to the last z
		      
	Summary:
	
	Input string                token       returnin pointer points to
	bbb..bXXXX..Xd..           XXXX..X            d
	
	zzz..zYYYY..Yd..           YYYY..Y            d
	
	zzz..zYYYY..Yzzz..zzd..    YYYY..Yzzz..zz     d
	
	zzz..zYYYY..Yzzz..zzY..    YYYY..Yzzz..z      the last z
	
*/
	char* getNextToken(char str[], const char del[],Name& token);

/**
  The function searches for a substring (token) of certain properties 
  (see below) in a segment of SML code. The token can span several lines.
  It takes as input:
       - pointer 'pSMLcode' which points to a number of SML lines. 
            Individual characters within this code are addressed
            by pair of integers (i,j), i being the line number (starting with 0)
	    and j being the character number (starting with 0) of
	    the referenced character on the ith line.
	
	- ist,jst ... address of the starting character within the code
	
	- char del[] ... list of delimiters
	   
     output:
     
        -  token      ... output string
	
        -  idel,jdel  ... address of the token's delimiter. If not   
			
	- inext,jnext ... address of the next character of interest after
	                  the token's delimiter.
			  
     In general the function returns the value of the delimiter which delimited
     the tokent.
     
     The function starts searching from (ist,jst) for the first non-blank
     character (FNB).
         If not found, then the function returns:
	     Return value   zero
	     token           "\0"
	    (idel,jdel)     undefined
	    (inext,jnext)   (-1,0)

	 If FNB is one of the non-blank delimiters, then the function returns:
	     Return value    the value of the delimiter
	     token           "\0" 
	    (idel,jdel)      address of the delimiter
	    (inext,jnext)    address of the character following the delimiter 
	                     ( (-1,0) if the delimiter is the last character
			              in the code)
	
         If FNB is found and it is not one of the non-blank delimiters, then
	 this is the beginning of the token.
	    If the token's delimiter is found on the same line as its begining,
	    then:
	      Return value    the value of the delimiter
	      token           extracted using the rules described under
	                      getNextToken(char str[],...)
	      (idel,jdel)     address of the delimiter
	     (inext,jnext)    address of the character following the delimiter 
	                     ( (-1,0) if the delimiter is the last character
			              in the code)
	 
	    If the token's delimiter is not found on the same line as its
	    beginining, then there are two possibilities:
	    
	        1) 'space' is one of the delimiters.
		In this case we consider this to be the token's end. The problem
	        is that we need to find proper delimiter... 
		****** the existing code needs reviewing ****
				
		2)  'space is not one of the delimiters.
		In this case we carry on and keep adding to the so far collected
		token, untill we either reach the end of the SML section or
		find proper delimiter.
	 
     In all the above, the end of a line is considered to be either
     the terminating zero or '!' or '#'
*/
	char getNextToken(SMLlineVector* pSMLcode,
				const int ist,const int jst,const char del[],
		
				Name& token,int& idel, int& jdel,
				int& inext, int& jnext);

	char* getName(char str[], const char del[], Name& name, int& ierr);

	char* getValue(char str[], const char del[], 
                  char* type, Name& value, int& ierr);

        char getValue(SMLlineVector* pSMLcode,
			const int ist,const int jst,const char del[],
			Name& value, char* type, int& ierr, int& idel, int& jdel,
			int& inext, int& jnext);

	char* getString(char str[],Name& value, int& ierr);



