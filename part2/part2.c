/******************************************************
 * Declarations
 ******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>


#define BIT_SHIFTS 8
#define NOT_FOUND -1
#define MAX_LINE_LENGTH 100

// #Define'd sizes
#define PHYSICAL_MEMORY_SIZE 128
#define FRAME_SIZE 256
#define PAGE_TABLE_SIZE 128
#define TLB_SIZE 16

#define FIFO 0
#define LRU 1
int page_replacement_strategy = FIFO;

// Make the TLB array
// Need pages associated with frames (could be 2D array, or C++ list, etc.)
int TLB[TLB_SIZE][2] = {{-1}};

// Make the Page Table
// Again, need pages associated with frames (could be 2D array, or C++ list, etc.)
int PAGE_TABLE[PAGE_TABLE_SIZE][3] = {{-1}};



// Make the memory
// Memory array (easiest to have a 2D array of size x frame_size)
char PHYSICAL_MEMORY[PHYSICAL_MEMORY_SIZE][FRAME_SIZE];

// where I temporarily store a frame from backing
//store until it goes into memory
char BACKING_BUFFER[FRAME_SIZE];
// bookkeeping
int PAGE_TABLE_INDEX = 0;
int TLB_INDEX = 0;
// Stats
int TLB_HIT = 0;
int PAGE_TABLE_HIT = 0;
int NUM_PAGE_FAULT = 0;
int NUM_TRANSLATED_ADDRESSES = 0;
int COUNTER = 0;



/******************************************************
 * Function Declarations
 ******************************************************/

/***********************************************************
 * Function: get_page_and_offset - get the page and offset from the logical address
 * Parameters: logical_address
 *   page_num - where to store the page number
 *   offset - where to store the offset
 * Return Value: none
 ***********************************************************/

void get_page_and_offset(int logical_address, int *page_num, int *offset){
	*offset = logical_address & ((1 << BIT_SHIFTS) - 1);
    *page_num = logical_address >> BIT_SHIFTS;
	
	
};

/***********************************************************
 * Function: get_frame_TLB - tries to find the frame number in the TLB
 * Parameters: page_num
 * Return Value: the frame number, else NOT_FOUND if not found
 ***********************************************************/
int get_frame_TLB(int page_num){
	for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i][0] == page_num) {
            TLB_HIT++;
            return TLB[i][1];
        }
    }
	
    return NOT_FOUND;
};

/***********************************************************
 * Function: get_available_frame - get a valid frame
 * Parameters: none
 * Return Value: frame number
 ***********************************************************/


/***********************************************************
 * Function: get_frame_pagetable - tries to find the frame in the page table
 * Parameters: page_num
 * Return Value: page number, else NOT_FOUND if not found (page fault)
 ***********************************************************/
int get_frame_pagetable(int page_num){
	for (int i = 0; i < PAGE_TABLE_INDEX; i++) {
        if (PAGE_TABLE[i][0] == page_num) {
            PAGE_TABLE_HIT++;
            return PAGE_TABLE[i][1];
        }
		
    }
	NUM_PAGE_FAULT++;
    return NOT_FOUND;
};

/***********************************************************
 * Function: update_page_table - update the page table with frame info
 * Parameters: page_num, frame_num
 * Return Value: none
 ***********************************************************/
void update_page_table(int page_num, int frame_num){
	// Find the old frame number for this page in the page table
    int old_page_num = NOT_FOUND;
    for (int i = 0; i < PAGE_TABLE_INDEX; i++) {
        if (PAGE_TABLE[i][1] == frame_num) {
            old_page_num = PAGE_TABLE[i][1];
            PAGE_TABLE[i][0] = page_num; // Update the page number
			PAGE_TABLE[i][2] = COUNTER;
			break;
        }
    }

	// If the page is not found, add a new entry
    if (old_page_num == NOT_FOUND) {
        PAGE_TABLE[PAGE_TABLE_INDEX][0] = page_num;
        PAGE_TABLE[PAGE_TABLE_INDEX][1] = frame_num;
		PAGE_TABLE[PAGE_TABLE_INDEX][2] = COUNTER;
        PAGE_TABLE_INDEX++;
    }
};



int get_available_frame(){

	static int frame_index = -1;
	int frame = -1;
	if (page_replacement_strategy == FIFO) {
        // FIFO implementation remains the same
        //int victim_frame = frame_queue[front];
        frame_index = (frame_index + 1) % PHYSICAL_MEMORY_SIZE;
		//enqueue(victim_frame); // Add the new frame to the queue
        //return victim_frame;
		frame = frame_index;
		return frame;
    } else if (page_replacement_strategy == LRU) {
        if(PAGE_TABLE_INDEX < PAGE_TABLE_SIZE){
			return PAGE_TABLE_INDEX;
		}
		else{
			int minimum_so_far_found = COUNTER;

			for(int i = 0; i < PAGE_TABLE_SIZE; i++){
				if(PAGE_TABLE[i][2]< minimum_so_far_found ){
					minimum_so_far_found = PAGE_TABLE[i][2];
					frame = i;
				}
			}
		}
    }

    return frame; // Invalid replacement strategy
};



/***********************************************************
 * Function: backing_store_to_memory - finds the page in the backing store and
 *   puts it in memory
 * Parameters: page_num - the page number (used to find the page)
 *   frame_num - the frame number for storing in physical memory
 * Return Value: none
 ***********************************************************/
void backing_store_to_memory(int page_num, int frame_num, const char *fname){
	
	FILE *backing_file = fopen(fname, "rb");
    // Load the page from the backing store into the victim frame
    size_t result;
    if (fseek(backing_file, page_num * FRAME_SIZE, SEEK_SET) != 0) {
        perror("Seeking error");
        return -1;
    }

    result = fread(BACKING_BUFFER, sizeof(char), FRAME_SIZE, backing_file);
    if (result != FRAME_SIZE) {
        perror("Reading error");
        return -1;
    }
	fclose(backing_file);
    //memcpy(PHYSICAL_MEMORY[victim_frame], BACKING_BUFFER, FRAME_SIZE);
	for(int i=0;i<FRAME_SIZE; i++){
		PHYSICAL_MEMORY[frame_num][i] = BACKING_BUFFER[i];
	}


}



/***********************************************************
 * Function: update_TLB - update TLB (FIFO)
 * Parameters: page_num, frame_num
 * Return Value: none
 ***********************************************************/
void update_TLB(int page_num, int frame_num){
	// Find the old frame number for this page in the TLB
    int old_page_num = NOT_FOUND;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i][1] == frame_num) {
            old_page_num = TLB[i][0];
            TLB[i][0] = page_num; // Update the page number
			TLB[i][1] = frame_num;
            //break;
        }
    }

    // If the page is not found, add a new entry using FIFO
    if (old_page_num == NOT_FOUND) {
        TLB[TLB_INDEX][0] = page_num;
        TLB[TLB_INDEX][1] = frame_num;
        TLB_INDEX = (TLB_INDEX + 1) % TLB_SIZE;
    }

	
};


/******************************************************
 * Assumptions:
 *   If you want your solution to match follow these assumptions
 *   1. In Part 1 it is assumed memory is large enough to accommodate
 *      all frames -> no need for frame replacement
 *   2. Part 1 solution uses FIFO for TLB updates
 *   3. In the solution binaries it is assumed a starting point at frame 0,
 *      subsequently, assign frames sequentially
 *   4. In Part 2 you should use 128 frames in physical memory
 ******************************************************/

int main(int argc, char * argv[]) {
		// argument processing
		// For Part2: read in whether this is FIFO or LRU strategy

		// initialization
		int logical_address;

		
        // read addresses.txt
		// Open the addresses.txt file
    	FILE *addresses_file = fopen(argv[2], "r");
    	if (addresses_file == NULL) {
        	fprintf(stderr, "Error: Unable to open addresses file.\n");
        	return 1;
    	}

		freopen("correct.txt", "w", stdout);

		char *backing_store_file = argv[1];

    	// Parse the replacement strategy
    	if (strcmp(argv[3], "fifo") == 0) {
        	page_replacement_strategy = FIFO;
    	} else if (strcmp(argv[3], "lru") == 0) {
        	page_replacement_strategy = LRU;
    	} else {
        	fprintf(stderr, "Invalid replacement strategy: %s\n", argv[3]);
        	return 1;
    	}
		//printf("Page Replacement: %d \n", page_replacement_strategy);
		char line[MAX_LINE_LENGTH];
		while(fgets(line, MAX_LINE_LENGTH, addresses_file)!=NULL) {
				// pull addresses out of the file
				logical_address = atoi(line);

				// Check if the logical address is valid
        		if (logical_address < 0 || logical_address >= (1 << (BIT_SHIFTS * 2))) {
            		fprintf(stderr, "Warning: Invalid logical address %d\n", logical_address);
            		continue;
        		}

				// Step 0:
				// get page number and offset
				//   bit twiddling
				int page_num, offset;
				get_page_and_offset(logical_address, &page_num, &offset);

				int frame_num = get_frame_TLB(page_num);
				int flag = 0;
				if(frame_num == NOT_FOUND){
					frame_num = get_frame_pagetable(page_num);
					if(frame_num == NOT_FOUND){
						frame_num = get_available_frame();
						backing_store_to_memory(page_num, frame_num, backing_store_file);
						//update_page_table(page_num, frame_num);
						//flag = 1;
						
					}
					flag = 1;
					update_TLB(page_num, frame_num);
				}
				
				update_page_table(page_num, frame_num);
				int value = PHYSICAL_MEMORY[frame_num][offset];
				NUM_TRANSLATED_ADDRESSES++;
				COUNTER++;
				printf("Virtual address: %d Physical address: %d Value: %d\n",logical_address, (frame_num << 8)|offset, value);
		}

		
		fclose(addresses_file);

		float rate_page_fault = (float)NUM_PAGE_FAULT / NUM_TRANSLATED_ADDRESSES;
    	float rate_TLB = (float)TLB_HIT / NUM_TRANSLATED_ADDRESSES;
		// output useful information for grading purposes
		printf("Number of Translated Addresses = %d\n", NUM_TRANSLATED_ADDRESSES);
		printf("Page Faults = %d\n", NUM_PAGE_FAULT);
		printf("Page Fault Rate = %.3f\n", rate_page_fault);
		printf("TLB Hits = %d\n", TLB_HIT);
		printf("TLB Hit Rate = %.3f\n", rate_TLB);
		
}
