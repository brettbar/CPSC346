/*
Class: CPSC 346-02
Team Member 1: Brett Barinaga
Team Member 2: Allison Fellger 
GU Username of project lead: bbarinaga
Pgm Name: proj4.c
Pgm Desc: Simulates a termninal shell
Usage: ./a.out to run
Example commands:
ls
pwd
history
quit
cd dir
idle-python2.7
!!
!N where N is an int < 10
*/

#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define TRUE 80

char** getInput();
char** parseInput(char*);
void dispOutput(char**);
void child(char*,char**);

void dispHist(char**, int);
void save(char**, char*, int);
char* dcopy(char*);

char** parseHistory(char**, int);

int main(int argc, char* argv[])
{
  //A pointer to an array of pointers to char.  In essence, an array of 
  //arrays.  Each of the second level arrays is a c-string. These hold
  //the command line arguments entered by the user.
  //as exactly the same structure as char* argv[]
  char **args; 
  int save_size = 0;
  char** hist = (char**)malloc(MAX_LINE); 

  char* first;


  while (TRUE)
  {
  	
	printf("myShell> ");
	fflush(stdout);
 	args = getInput();
 	first = *args++;
  
     save(hist, first, save_size);
     if (save_size <= 9) 
     	save_size++;
     dispOutput(args);
    
     child(args[0], args);

    char** iterator = args;
	char* itchar = *iterator++;

	if(args != NULL) {
        dispOutput(args);
        if(!strcmp(args[0], "quit"))
        {
        	printf("\n---> This is OVER\n");
        	break;
        } 
        else if(!(strcmp(args[0], "history")))
    	{
     		dispHist(hist, save_size);
     	}
        else if(!strcmp(args[0], "cd")) 
        {
            chdir(args[1]);
        } 
        else if(!strcmp(args[0], "!!")) 
        {
            child(hist[1], args);
        } 
        else if(args[0][0] == '!') 
        {
            char* str = *args;
            *str++;
            int a = atoi(str);
            if(a < save_size + 1) 
            {
            	char* temp = dcopy(hist[a]);
            	char** newargs = parseInput(temp);
            	*newargs++;
                child(newargs[0], newargs);
            } 
            else 
            {
                printf("\n---> invalid history record\n");
            }  
        } 

     }
	
     //break;
     //See figure 3.36 on p. 158.  Do items 1, 2, 3 
  }
   return 0;
}

/*
Reads input string from the key board.   
invokes parseInput and returns the parsed input to main
*/
char** getInput()
{	 
	char* inp = (char*) malloc(MAX_LINE);
 	char* start = inp;
 	char c;
 	char** out;

 	while ((c = getchar()) != '\n')
  		*inp++ = c; //weird, yes? First add the character to the dereffed pointer
   	           //then go to the next position in dynamic memory 
 	*inp = '\0'; 

 	out = parseInput(start);
 	return out;
}  

/*
inp is a cstring holding the keyboard input
returns an array of cstrings, each holding one of the arguments entered at
the keyboard. The structure is the same as that of argv
Here the user has entered three arguements:
myShell>cp x y
*/ 
char** parseInput(char* inp)
{
	char** args = (char**) malloc(MAX_LINE);
	char** start  = args;
	*args++ = dcopy(inp);
	char* token = strtok(inp, " ");
	*args++ = token;

	while (token != NULL)
	{
		token = strtok(NULL, " ");
		*args++ = token;
	}	
	*args++ = "\0";
	args = start;
	
	return args;
} 
 
/*
Displays the arguments entered by the user and parsed by getInput
*/
void dispOutput(char** args)
{
	char** print = args;

	while (*print != "\0")
	{
		print++;
	}
}  

// Forks a new child process from the parent shell process
// Default waits since many commands do not work if the shell does not wait for 
// it to finish (such as ls and pwd)
void child(char* command, char** parameters)
{
  id_t pid; 

  char** iterator = parameters;
  char* first = *iterator++;

  pid = fork();

  if (pid < 0)  //fork failed
  {
   fprintf(stderr, "Fork Error");
  } 
  else
   if (pid == 0) // child process
   {
   	execvp(command,parameters);  //load args[0] into the child's address space 
   	exit(0);
   } 
   else           //parent process
   {
   	
    wait(NULL); // comment this out later
   }
}

// Copys a cstring and returns the copy
char* dcopy (char* source) 
{
    char* destination = (char*)malloc(MAX_LINE);
    char* hold = destination;
    int i = 0;
    char* print = source;
    char c;
    c = *print++;
    while(c != '\0') 
    {
       *destination = c;
        c = *print++;
        destination++;
    }
    return hold;
}

//saves a new cstring into the history cstring array
void save(char** history, char* new_item, int i) 
{
    while(i > 0) 
    {
        history[i] = history[i - 1];
        i--;
    }
    history[0] = new_item;
}

// displays history when the history command is called
void dispHist(char** history, int i) 
{
    char** print = history;
    int k = i;

    while(k > 1) 
    {
         print++;
        
        k--;
    }
    while(i > 0) 
    {
        printf("%d: %s\n", i, *print);
        print--;
        i--;
    }
}

// selects a and returns a command from history, specified by an integer
char** parseHistory(char** history, int i)
{

	printf("a");
	char* selected = dcopy(history[i]);
	printf("%s", selected);
	return parseInput(selected);
}