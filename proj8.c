/*
Class:CPSC 346-02
Team Member 1: Sebastian Berven
Team Member 2: Brett Barinaga 
GU Username of project lead: sberven
Pgm Name: Homework 8
Pgm Desc: Simulates virtual memory and the use of pages and a page table.
Usage: ./a.out BACKING_STORE.bin addresses.txt
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  //allows use of bool type

#define TLB_SIZE 16
#define PAGES 256
#define PAGE_MASK 255
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE PAGES * PAGE_SIZE 




//single entry in the page table.  present_absent = 'A' page not in RAM.  
//present_absent = 'P' page in RAM
struct pagetable_entry 
   {
    char present_absent; 
    char frame_number;
   };
typedef struct pagetable_entry pagetable_entry; 


//single entry in the translation lookaside buffer.  
//tlb_ref holds the number of page the fault.  Used to implement LRU
struct tlb_entry 
   {
    char page_number;
    char frame_number;
    int tlb_ref;
   };
typedef struct tlb_entry tlb_entry;  

//decomposed entry from addresses.txt
struct virt_addr 
   {
    unsigned char page_number;
    unsigned char offset; 
   };
typedef struct virt_addr virt_addr;  

//file references
struct files
{
  char* backing;         //reference to secondary storage (BACKING_STORE.bin)
  FILE* fin;             //reference to address bus (addresses.txt)
};
typedef struct files files;
  
//function prototypes
void init_pagetable(pagetable_entry*);
void init_tlb(tlb_entry*);
files open_files(char*, char*);
void  translate(files, virt_addr*, int*);
char write_read_memory(char*, virt_addr*, files, int*, int);
bool check_pagetable(pagetable_entry*,virt_addr,char*);
void update_pagetable(pagetable_entry*,virt_addr*,int);
char consult_pagetable(pagetable_entry*, char*, virt_addr*, int*);
bool check_tlb(tlb_entry*,virt_addr*,char*);
void update_tlb(tlb_entry*,virt_addr*,char*, int);
char consult_tlb(tlb_entry*, char*, virt_addr*, int*, int);
void display_values(int, int, char);

int main(int argc, char *argv[])
{ 
	int logical_address;  	       //address from addresses.txt
	virt_addr virtual_address;           //holds page number and offset from logical_address
	unsigned int physical_address;                //offset into page frame 
	pagetable_entry pagetable[PAGES];    //page table
	tlb_entry tlb[TLB_SIZE];             //translation lookaside buffer
	char main_memory[MEMORY_SIZE];       //RAM
	files file_ids;                      //identifiers for the two files used in the program
	int multi;
	char value;
	int page_fault;

	init_pagetable(pagetable);

	init_tlb(tlb);

	file_ids = open_files(argv[1],argv[2]);
	multi = 0;
	page_fault = 0;

	for(int i = 0; i<1000; i++){

		translate(file_ids,&virtual_address,&logical_address);    

		char frame_number;

/*		if (check_tlb(tlb,&virtual_address,&frame_number)){
			printf("page reference in translation lookaside buffer\n");
			value = consult_tlb(tlb, main_memory, &virtual_address, &physical_address, multi);
		}
		else */
		if(check_pagetable(pagetable,virtual_address,&frame_number)){
			printf("page reference in page table\n");
			//update_tlb(tlb, &virtual_address, &frame_number, multi);
			value = consult_pagetable(pagetable, main_memory, &virtual_address, &physical_address);
		}
		else{
			//printf("page reference not in page table\n");
			value = write_read_memory(main_memory, &virtual_address,file_ids,&physical_address, multi);
			update_pagetable(pagetable, &virtual_address, multi);
			//update_tlb(tlb, &virtual_address, &frame_number, multi);
			multi++;
		}
		display_values(logical_address, physical_address, value);
  
	}
	printf("Page Faults = %d\n", multi);
	float pfrate = (multi/1000.0);
	printf("Page Fault Rate = %f\n", pfrate);
	return 0;
}

//Init page table.  Set absent field to 'A'
void init_pagetable(pagetable_entry* pagetable)
{
	for (int i = 0; i < PAGES; i++)
		pagetable[i].present_absent = 'A';
} 

//Init tlb. Set page_number field to -1 (i.e., empty) 
void init_tlb(tlb_entry* tlb)
{
	for (int i = 0; i < TLB_SIZE; i++)
		tlb[i].page_number = -1;
}

//open and prepare files
files open_files(char* argv_1, char* argv_2)
{ 
	files ids;

	//allow BACKING_STORE.bin to be treated as an array
	int backing_fd = open(argv_1, O_RDONLY);  //open BACKING_STORE.bin
	ids.backing = mmap(0,MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0);

	ids.fin = fopen(argv_2,"r"); //open addresses.txt
	return ids;

}

//read and translate address 
void translate(files ids, virt_addr* virt_address, int* logical_address_out)
{
	unsigned int logical_address;    //the value stored in address.txt 
	fscanf(ids.fin,"%u", &logical_address);

	virt_address->page_number = (logical_address >> OFFSET_BITS) & PAGE_MASK;

	virt_address->offset = logical_address & OFFSET_MASK;
	*logical_address_out = logical_address;
}

//copy backing store to main memory
//read sought after value from offset into memory
char write_read_memory(char* main_memory,  virt_addr* virtual_address, files file_ids, 
		int* physical_address, int multi)
{
	unsigned int frame_number  = (0+multi)%256;


	memcpy(main_memory + frame_number * PAGE_SIZE,
		 file_ids.backing + virtual_address->page_number * PAGE_SIZE, PAGE_SIZE);
	//offset into page frame 
	*physical_address = (frame_number << OFFSET_BITS) | virtual_address->offset; 

	char value = main_memory[(*physical_address & 255) * PAGE_SIZE + virtual_address->offset];

	return value;
}

char consult_pagetable(pagetable_entry* pagetable, char* main_memory,  virt_addr* virtual_address, int* physical_address)
{
	unsigned int frame_number = pagetable[virtual_address->page_number].frame_number;
	*physical_address = ((frame_number << OFFSET_BITS) | virtual_address->offset) & 65535 ; 

	char value = main_memory[(*physical_address & 255) * PAGE_SIZE + virtual_address->offset];
	return value;
}

//interrogate page table
bool check_pagetable(pagetable_entry* pagetable, virt_addr virtual_address, char* frame_number)
{
	if (pagetable[virtual_address.page_number].present_absent == 'A')
		return false;
	*frame_number = pagetable[virtual_address.page_number].frame_number;
	return true; 
}

void update_pagetable(pagetable_entry* pagetable, virt_addr* virtual_address, int frame_number)
{
	pagetable[virtual_address->page_number].present_absent = 'P';
	pagetable[virtual_address->page_number].frame_number = frame_number; 
}
/*
//interrogate tlb
bool check_tlb(tlb_entry* tlb, virt_addr* virtual_address,char* frame_number)
{
printf("Made it to here\n");
	for (int i = 0; i < TLB_SIZE; i++)
		if (tlb[i].page_number == virtual_address->page_number){
			return true;
 
		}
	return false;
}

char consult_tlb(tlb_entry* tlb, char* main_memory,  virt_addr* virtual_address, int* physical_address, int multi)
{
	unsigned int frame_number;
	for (int i = 0; i < TLB_SIZE; i++)
		if (tlb[i].page_number == virtual_address->page_number){
			frame_number = tlb[i].frame_number;
			tlb[i].tlb_ref = multi;
		}

	*physical_address = ((frame_number << OFFSET_BITS) | virtual_address->offset) & 65535 ; 

	char value = main_memory[(*physical_address & 255) * PAGE_SIZE + virtual_address->offset];
	return value;
}

void update_tlb(tlb_entry* tlb, virt_addr* virtual_address, char* frame_number, int multi)
{
	int lowest = tlb[0].tlb_ref;
	int low_place;
	
	for(int i = 0; i< TLB_SIZE; i++)
		if(tlb[i].tlb_ref < lowest)
			low_place = i;
	
	tlb[low_place].page_number = virtual_address->page_number;
	tlb[low_place].frame_number = *frame_number;
	tlb[low_place].tlb_ref = multi;
}
*/
void display_values(int logical_address, int physical_address, char value)
{
	printf("Virtual address: %d Physical address: %d Value: %d\n",
		 logical_address, physical_address, value);
}
