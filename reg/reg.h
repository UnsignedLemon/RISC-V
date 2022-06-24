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
	
class REGISTER{
private:
	unsigned int input;
	unsigned int output;
public:
	REGISTER();
	unsigned int read();
	void setInput(unsigned int val);
	void write();
};

class BUFFER{
public:
	cmdType cmd;
	unsigned int currentPC;
	
	int rs1,rs2,rd;					// Register ID.
	unsigned int num_rs1,num_rs2;	// Val of registers.
	int imm;

public:
	BUFFER();
};

void B1_proceed();
void B2_proceed();
void B3_proceed();
void B4_proceed();

}

#endif
