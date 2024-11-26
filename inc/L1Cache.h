#ifndef L1CACHE_H
#define L1CACHE_H

#include"Block.h"
#include<cmath>
#include<vector>

class L1Cache{
    //cache class that keeps track of sizes, sets, block matrix and LRU matrix
	public:
		int size;
		int blocksize;
		int assoc;
		int numsets;
		int tagsize;
		int indexsize;
		int offsetsize;
		Block **block;
		// int **LRU;

		//ye Srikar ka h
		int numnodes;
		// int **PLRU;
		std::vector<int> lru_tree;
		//Yaha tak

	public://all methods 
		L1Cache(int SIZE, int BLOCKSIZE, int ASSOC);
		void readcache(unsigned int tag, unsigned int index, unsigned int hexaddress);
		void writecache(unsigned int tag, unsigned int index, unsigned int hexaddress);

		//LRU
		// void setLRU(unsigned int index, int column);				
		// int evictLRU(unsigned int index);
		// void initLRU();

		//PLRU
		void setPLRU(unsigned int index, int column);				
		int evictPLRU(unsigned int index);
		void initPLRU();
};
#endif