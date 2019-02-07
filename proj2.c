/*
Class: CPSC 346-01 & CPSC 346-02
Team Member 1: Brett Barinaga
Team Member 2: Nick Walker
GU Username of project lead: bbarinaga
Pgm Name: proj2.c
Pgm Desc: Count the words in an entered string
as well as prints the entered string
Usage: ./a.out
*/


// Test


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80

char* get_inp();

void disp_inp(char*);

int wc(char*);

int main()
{
 char* inp = get_inp();
 printf("here is your input : %s\n",inp);
 printf("%d\n", wc(inp));
 disp_inp(inp); 

 free(inp);  //return dynamially allocated memory to the heap
 return 0;
}


char* get_inp()
{
 char* inp = (char*) malloc(MAX_LINE);
 char* start = inp;
 char c;

 printf("Enter text <= 80 characters in length\n");
 while ((c = getchar()) != '\n')
  *inp++ = c; //weird, yes? First add the character to the dereffed pointer
              //then go to the next position in dynamic memory 
 *inp = '\0'; 
 return start;
}

void disp_inp(char* out)
{
 while(*out)  //continue until the null character is encountered
  putchar(*out++);
 putchar('\n');
}

/*
pre:  inp is the address of an 80 byte block of memory 
post: returns the number of words stored in the block of memory.  A word
      is any sequence of characters that 
      1) are stored at the beginning of the block or 
      2) are stored at end of the block or
      3) are terminated by the end-of-line character or
      4) are preceded by and ended by one or more spaces
        
*/
int wc(char* inp)
{
	int words = 0;

	int inAWord = 0;

	while (*inp != '\0')
	{
		if (*inp == ' ' || *inp == '\n')
		{
			inAWord = 0;
		}
		else if (inAWord == 0 && *inp != ' ')
		{
			inAWord = 1;
			words++;
		}
		inp++;
	}
	return words;
}


