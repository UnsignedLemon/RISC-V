#ifndef REG_H
#define REG_H

#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <fstream>
#include "../exceptions.h"
#include "../enum.h"

namespace riscv{

//-------------------------    Class QUEUE    ----------------------------------
class QUEUE{		// Used for PCQ to store predicted PC.
private:
	int head,tail;
	unsigned int data[8];
public:
	QUEUE();
	void pushBack(unsigned int val);
	unsigned int getFront();
	void pop();
	void clear();
};

//-------------------------    Class REGISTER     ------------------------------
class REGISTER{
private:
	unsigned int input;
	unsigned int output;
public:
	REGISTER();
	unsigned int read();
	void setInput(unsigned int val);
	void write();
	void clear();
};

//--------------------------    Class PC    ------------------------------------

class _PC{
private:
	unsigned int currentPC;
public:
	QUEUE PCQ;
public:
	_PC();
	unsigned int read();
	void proceed();
	void clear();
};

//--------------------------    Class Buffer     -------------------------------

class BUFFER{
public:
	cmdType cmd;
	unsigned int currentPC;
	
	int rs1,rs2,rd;					// Register ID.
	unsigned int num_rs1,num_rs2;	// Val of registers.
	int imm;

public:
	BUFFER();
	void clear();
};

//---------------------------    Buffer Process    -----------------------------

void B1_proceed();
void B2_proceed();
void B3_proceed();
void B4_proceed();

}

#endif
