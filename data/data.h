#ifndef STATIC_DATA_H
#define STATIC_DATA_H

#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <fstream>
#include "../exceptions.h"

#define maxSize 524288

namespace riscv{

//---------------------------    STATIC DATA    --------------------------------
class DATA{									// Used for program storage.
private:
	unsigned char dataMemory[maxSize];
	
public:
	DATA();
	DATA(const char* path);									// Using files to construct.
	void readFromStandardInput();
	
	unsigned char fetchData(unsigned int pos) const;		// Fetch UnsignedChar;	
	unsigned int fetchInt(unsigned int pos) const;			// Fetch 4 Bytes.
	unsigned int fetchHalfWord(unsigned int pos) const;		// Fetch 2 Bytes.
	unsigned int fetchByte(unsigned int pos) const;			// Fetch Byte.
	
	void writeInt(unsigned int pos,unsigned int val);
	void writeHalfWord(unsigned int pos,unsigned int val);
	void writeByte(unsigned int pos, unsigned int val);
	
	void print() const;
};

}

#endif
