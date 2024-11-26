#include"L1Cache.h"

extern int L1reads;
extern int L1readmisses;
extern int L1writes;
extern int L1writemisses;
extern int L1writebacks;
extern int TOTALBITS;

// void L1Cache::initLRU(){
// 	//initializes LRU can be set to anything
// 	for(int i = 0; i < numsets; i++){
// 		for(int k = 0; k < assoc; k++){
// 			LRU[i][k] = k;
// 		}
// 	}
// }

// int L1Cache::evictLRU(unsigned int index){
// 	//finds LRU block and sends it back
// 	return LRU[index][assoc-1];
// }

// void L1Cache::setLRU(unsigned int index, int column){
// 	//updates LRU
// 	int temp = -1;
// 	//finds column that was last read/wrote to
// 	for(int i = 0; i < assoc; i++){
// 		if(LRU[index][i] == column){
// 			temp = i;	
// 			break;	
// 		}
// 	}
// 	//shifts used row to front of array. shifts other vals back
// 	for(int j = temp; j > 0; j--){
// 		LRU[index][j] = LRU[index][j-1];
// 	}
// 	LRU[index][0] = column;

// 	return;	
// }

void L1Cache::initPLRU(){
    //initialize the below variables in .hh
    
    numnodes=assoc-1;
    //initialises tree to 0
    lru_tree.resize(numnodes*numsets,0);
	
}

void L1Cache::setPLRU(unsigned int index, int column){
    int base=index*numnodes;
    int node=0;
    int levels=log2(numnodes);
    for(int i=levels;i>0;i--)
    {
        int bit=(column>>(i-1))&1;
        lru_tree[node+base]=!bit;
        node= (2*node)+1+bit;
    }
    return;

}		
int L1Cache::evictPLRU(unsigned int index){
    int base=index*numnodes;
    int levels=log2(numnodes);
    int node=0;
    int column=0;
    for(int i=levels+1;i>0;i--)
    {
        int bit=lru_tree[node+base];
        column=(column<<1)||bit;
        node=(2*node)+1+bit;   
    }
    return column;
}

		
L1Cache::L1Cache(int SIZE, int BLOCKSIZE, int ASSOC){
	//initializes cache vals
	size = SIZE;
	blocksize = BLOCKSIZE;
	assoc = ASSOC;
	numsets = size/(blocksize*assoc);
	indexsize = log2(numsets);
	
	offsetsize = log2(blocksize);
	tagsize = TOTALBITS - indexsize - offsetsize;
	block = new Block*[numsets];
	for(int i = 0; i < numsets; i++){
		block[i] = new Block[assoc];
	}

	// LRU = new int*[numsets];
	// for(int i = 0; i < numsets; i++){
	// 	LRU[i] = new int[assoc];
	// }	

	// this->initLRU();	
	this->initPLRU();	
}

void L1Cache::readcache(unsigned int tag, unsigned int index, unsigned int hexaddress){
	L1reads++;	//in stats
		
	// reads for direct hit
	int column = -1;
    for(int i = 0; i < assoc; i++){
        if(block[index][i].isValid == true && block[index][i].tag == tag){
			column = i;
			// this->setLRU(index, column);
			this->setPLRU(index, column);
			return; //found it all good i.e HIT
			//block[index][i].isDirty = false;
        }
    }

	// read is not a hit, read miss up, check for empty blocks
	L1readmisses++;
	for(int k = 0; k<assoc; k++){	
		if(block[index][k].isValid == false){
			block[index][k].isDirty = false;
			block[index][k].isValid = true;
			block[index][k].tag = tag; 
			block[index][k].fulladdress=hexaddress;
			// this->setLRU(index, k);
			this->setPLRU(index, k);

			//Read Main Memory

			return;
			//all good
		}

	}

	// No empty blocks, look for LRU block and evict
	// int evictColumn = evictLRU(index);
	int evictColumn = evictPLRU(index);

	if(block[index][evictColumn].isDirty){
		L1writebacks++; //Write Back if dirty block	
	}

	block[index][evictColumn].tag=tag;
	block[index][evictColumn].isValid=true;
	block[index][evictColumn].isDirty=false;
	block[index][evictColumn].fulladdress = hexaddress;
	// this->setLRU(index, evictColumn);
	this->setPLRU(index, evictColumn);

	return; 
}


void L1Cache::writecache(unsigned int tag, unsigned int index, unsigned int hexaddress){

	L1writes++;
	// check for write hit
	for(int i = 0 ; i < assoc; i++){

		if(block[index][i].tag==tag){
			// this->setLRU(index, i);
			this->setPLRU(index, i);
			block[index][i].isDirty = true;
			block[index][i].isValid = true;
			//hit found all good
			return;
		}

	}
	//no hit found. write miss
	L1writemisses++;
	
	//check for unused block and enter tag data
	for(int j = 0; j < assoc; j++){

		if(block[index][j].isValid == false){

			block[index][j].tag = tag;
			block[index][j].isDirty=true;
			block[index][j].isValid = true;
			block[index][j].fulladdress = hexaddress;
				
			// this->setLRU(index, j);
			this->setPLRU(index, j);
			return;	
		}
	
	}	
	
	//not empty block find LRU block and evict it	
	// int evictColumn = evictLRU(index);
	int evictColumn = evictPLRU(index);
	if(block[index][evictColumn].isDirty == true){
		L1writebacks++;

	}

	block[index][evictColumn].fulladdress = hexaddress;
	block[index][evictColumn].tag = tag;	
	block[index][evictColumn].isDirty = true;
	// this->setLRU(index, evictColumn);	
	this->setPLRU(index, evictColumn);
	
	return;	
}