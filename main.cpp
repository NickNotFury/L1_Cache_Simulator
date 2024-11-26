#include"Block.h"
#include"L1Cache.h"

#include<stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

//cmd arguments
int Blocksize = 0;
int L1Size = 0;
int L1Assoc = 0;
char* tracefile;

//stats
int L1reads = 0;
int L1readmisses = 0;
int L1writes = 0;
int L1writemisses;
float L1MissRate = 0.0;
int L1writebacks = 0;
int totalmemtraffic = 0;
int TOTALBITS = 32;

//take command line args and save them into global variables
void initVals(char** args){
		Blocksize = atoi(args[1]);
        L1Size = atoi(args[2]);
        L1Assoc = atoi(args[3]);
        tracefile = args[4];
}

// read the trace files for commands
void readfile(char *file, L1Cache L1){

	char readstore;
	string address;
	string line;
	unsigned int hexaddress;
	unsigned int tag;
	unsigned int index;
//	unsigned int offset; 

	ifstream infile(file);
	
	while(infile >> readstore >> address){
		// read file for hex address
		stringstream ss;
		ss << address;
		ss >> hex >> hexaddress;
                tag = hexaddress>>(L1.offsetsize + L1.indexsize);
		if(L1.numsets > 1){	       
        	       	index = (hexaddress<<L1.tagsize)>>(L1.tagsize+L1.offsetsize);
		}
		else{// there is only 1 est, we know the index is 0
			index = 0;
		}
	//	offset = (hexaddress<<(TOTALBITS-L1.offsetsize))>>(TOTALBITS-L1.offsetsize);
 		// totalcount++;
		
		//perform read	              
		if(readstore == 'r'){
			L1.readcache(tag, index, hexaddress);
		
		}//perform write
		else if(readstore == 'w'){

			L1.writecache(tag, index, hexaddress);
		}
		else{//debugger
			printf("strange error\n");
		}

	}
}

void printStats(L1Cache L1){
	//print all according to validation run txt
	printf("  ===== Simulator configuration =====\n");
	printf("  BLOCKSIZE:                        %d\n", L1.blocksize);
	printf("  L1_SIZE:                          %d\n", L1.size);
	printf("  L1_ASSOC:                         %d\n", L1.assoc);
	printf("  trace_file:                       %s\n\n", tracefile);
	printf("===== L1 contents =====\n");

	// for(int i = 0; i < L1.numsets; i++){

	// 	printf("set	%d:", i);
	// 	for(int k = 0; k < L1.assoc; k++){
		
	// 		int column = L1.LRU[i][k];
	// 		char dirtychar;
	// 		if(L1.block[i][column].isDirty){
	// 			dirtychar = 'D';
	// 		}
	// 		else{
	// 			dirtychar = 'N';
	// 		}
	// 		printf("	%x %c	||", L1.block[i][column].tag, dirtychar);  

	// 	}
	// 		printf("\n");
		
	// }
    	
	printf("\n");
	printf("===== Simulation Results =====\n");
	printf("a. number of L1 reads:			%d\n", L1reads);
	printf("b. number of L1 read misses:		%d\n", L1readmisses);
	printf("c. number of L1 writes:			%d\n", L1writes);
	printf("d. number of L1 write misses:		%d\n", L1writemisses);
	L1MissRate = ((float)L1readmisses+(float)L1writemisses)/((float)L1writes+(float)L1reads);
//	printf("e. L1 miss rate:			%f\n", L1MissRate);
	// cout << fixed;
	// cout << "e. L1 miss rate:			" <<  setprecision(4) << L1MissRate << "\n";
	printf("f. number of writebacks from L1 memory:	%d\n", L1writebacks);
	
	totalmemtraffic = L1readmisses+L1writemisses;
	printf("g. total memory traffic:                %d\n", totalmemtraffic);
	
	return;		

}

int main(int argc, char** argv){

    //set command line arguments for our variable
	if(argc != 5){
		//input is not correct, stop program
		std::cout<<"Input args are not correct\n";
		return 0;
	}
	else{
		//get vals from input line
		initVals(argv);
	}


	//initialize cache l1
	L1Cache L1 = L1Cache(L1Size, Blocksize, L1Assoc);

    readfile(tracefile, L1);

    printStats(L1);

    return 0;
}