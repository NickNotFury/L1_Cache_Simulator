#ifndef BLOCK_H
#define BLOCK_H

class Block{
	// block class that holds dirty and valid bits and manages tags	
	public:
		bool isDirty;
		bool isValid;
		unsigned int tag;
		unsigned int fulladdress;

	public:
		Block();
};
#endif