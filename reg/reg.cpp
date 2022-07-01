#include "reg.h"
#include "../resources.h"

using std::cin;
using std::cout;

namespace riscv{

//------------------------    Class QUEUE    -----------------------------------
QUEUE::QUEUE(){
	head=tail=0;
	memset(data,0,sizeof(data));
}

unsigned int QUEUE::getFront(){
	if (head==tail) throw queue_empty();
	return data[head];
}

void QUEUE::pushBack(unsigned int val){
	data[tail]=val;
	tail=(tail+1)%8;
}

void QUEUE::pop(){
	data[head]=0;
	head=(head+1)%8;
}
void QUEUE::clear(){
	head=tail=0;
	memset(data,0,sizeof(data));
}

//---------------------    Class PC    -----------------------------------------

_PC::_PC(){
	currentPC=0;
}

unsigned int _PC::read(){
	return currentPC;
}

void _PC::proceed(){
	if (shouldPause) return;
	if (shouldBubble) return;
	if (shouldDiscard){
		currentPC=EX.outputBuffer.num_rs2;
		PCQ.clear();					// Prediction MISSED. B1,B2,newPC are invalid.
		bool isJumpCmd=(EX.outputBuffer.cmd==JAL || EX.outputBuffer.cmd==JALR);
		PRED.update(EX.outputBuffer.currentPC,isTaken,false,currentPC,isJumpCmd);
		return;
	}
	if (shouldPopPCQ){					// Prediction HIT. Update PRED first.
		bool isJumpCmd=(EX.outputBuffer.cmd==JAL || EX.outputBuffer.cmd==JALR);
		PRED.update(EX.outputBuffer.currentPC,isTaken,true,PCQ.getFront(),isJumpCmd);
		PCQ.pop();
	}
	switch (IF.opcode){
		case 0x6f:
		case 0x67:
		case 0x63:{
			PRED.doPredict(currentPC);
			PCQ.pushBack(currentPC);
			break;
		}
		default:{
			currentPC+=4;
			break;
		}
	}
}

void _PC::clear(){
	currentPC=0;
	PCQ.clear();
}

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

void REGISTER::clear(){
	input=output=0;
}

//---------------------    Class BUFFER    -------------------------------------
BUFFER::BUFFER(){
	cmd=NOP;currentPC=0;
	rs1=rs2=rd=imm=0;
	num_rs1=num_rs2=0;
}

void BUFFER::clear(){
	cmd=NOP;currentPC=0;
	rs1=rs2=rd=imm=0;
	num_rs1=num_rs2=0;	
}

//---------------------    Process of B1    ------------------------------------
void B1_proceed(){
	if (shouldPause || shouldBubble) return;
	if (shouldDiscard){
		B1.num_rs1=B1.num_rs2=0;
		B1.currentPC=EX.outputBuffer.num_rs2;
		return;
	}
	// Read from IF.
	B1.num_rs1=IF.INS;
	B1.num_rs2=IF.opcode;
	B1.currentPC=IF.currentPC;
}

//----------------------    Process of B2    -----------------------------------

	//======================    Do forwarding    ===============================
static bool isWriteCmd(cmdType cmd){
	bool res=false;
	switch (cmd){
		case BEQ:
		case BNE:
		case BLT:
		case BGE:
		case BLTU:
		case BGEU:
		case SB:
		case SH:
		case SW:
		case NOP:
		case TERMINATE:{
			// Do nothing.
			res=false;
			break;
		}
		default:
			res=true;
	}
	return res;
}

static bool doForwarding(int regName,unsigned int &regVal){
	if (isWriteCmd(EX.outputBuffer.cmd) && EX.outputBuffer.rd==regName){
		regVal=EX.outputBuffer.num_rs1;
		// Impossible cases.
		switch(EX.outputBuffer.cmd){
			case LH:
			case LB:
			case LW:
			case LBU:
			case LHU:
				throw forwarding_not_match();
		}
		return true;
	}
	if (isWriteCmd(ME.outputBuffer.cmd) && ME.outputBuffer.rd==regName){
		regVal=ME.outputBuffer.num_rs1;
		return true;
	}
	if (isWriteCmd(WB.outputBuffer.cmd) && WB.outputBuffer.rd==regName){
		regVal=WB.outputBuffer.num_rs1;
		return true;
	}
	return false;
}

	//=======================    B2 Process    =================================
void B2_proceed(){
	if (shouldPause) return;
	if (shouldDiscard){
		B2.cmd=NOP;
		B2.currentPC=0;
		B2.rd=B2.rs1=B2.rs2=0;
		B2.num_rs1=B2.num_rs2=0;
		B2.imm=0;
		return;
	}
	//======================    Data copy    ===================================
	B2.currentPC=ID.outputBuffer.currentPC;
	B2.rd=ID.outputBuffer.rd;
	B2.rs1=ID.outputBuffer.rs1;
	B2.rs2=ID.outputBuffer.rs2;
	B2.num_rs1=B2.num_rs2=0;
	B2.imm=ID.outputBuffer.imm;
	
	//======================    Special cases    ===============================
	// Determine cmd.
	if (ID.isUnknown){		// A bubble. 
		B2.cmd=NOP;
		return;
	}
	B2.cmd=ID.outputBuffer.cmd;
	
	// Forwarding.
	if (B2.rs1){
		if (!doForwarding(B2.rs1,B2.num_rs1)){
			B2.num_rs1=ID.outputBuffer.num_rs1;
		}
	}
	if (B2.rs2){
		if (!doForwarding(B2.rs2,B2.num_rs2)){
			B2.num_rs2=ID.outputBuffer.num_rs2;
		}
	}
}

//-------------------------    Process of B3    --------------------------------

void B3_proceed(){
	if (shouldPause) return;
	B3=EX.outputBuffer;
}

//-------------------------    Process of B4    --------------------------------
void B4_proceed(){
	if (shouldPause) return;
	B4=ME.outputBuffer;
}

}
