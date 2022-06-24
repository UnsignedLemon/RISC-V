#include "reg.h"
#include "../resources.h"

using std::cin;
using std::cout;

namespace riscv{

//--------------------    Class REGISTER    ------------------------------------
REGISTER::REGISTER(){
	input=output=0;
}

unsigned int REGISTER::read(){
	return output;
}

void REGISTER::setInput(unsigned int val){
	input=val;
}

void REGISTER::write(){
	output=input;
}

//---------------------    Class BUFFER    -------------------------------------
BUFFER::BUFFER(){
	cmd=NOP;currentPC=0;
	rs1=rs2=rd=imm=0;
	num_rs1=num_rs2=0;
}

//---------------------    Process of Buffers    -------------------------------
void B1_proceed(){		// Read from IF.
	B1.num_rs1=IF.INS;
	B1.num_rs2=IF.opcode;
	B1.currentPC=IF.currentPC;
}

void B2_proceed(){		// Read from ID.
	B2=ID.outputBuffer;
}

void B3_proceed(){		// Read from EX.
	B3=EX.outputBuffer;
}

void B4_proceed(){		// Read from ME.
	B4=ME.outputBuffer;
}

}
