#include "units.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "../exceptions.h"
#include "../resources.h"
using std::cout;

namespace riscv{
//----------------------------    Predictor    ---------------------------------
_PREDICTOR::_PREDICTOR(){
	newPC=0;
}

void _PREDICTOR::proceed(){
	if (IF.INS==terminateCmd){
		newPC=PC;
		return;
	}
	switch(EX.outputBuffer.cmd){
		case JAL:
		case JALR:{
			newPC=EX.outputBuffer.num_rs2;
			break;
		}
		case BEQ:
		case BNE:
		case BLT:
		case BGE:
		case BLTU:
		case BGEU:{
			if (EX.outputBuffer.num_rs1) newPC=EX.outputBuffer.num_rs2;
			else newPC=PC+4;
			break;
		}
		default:
			newPC=PC+4;
	}
}

//-----------------------------    I F    --------------------------------------
_IF::_IF(){
	INS=opcode=currentPC=0;
	isJumpCmd=false;
}

void _IF::proceed(){
	INS=memoryData.fetchInt(PC);
	currentPC=PC;
	opcode=INS&(0x7f);
	isJumpCmd=opcode&(0x40);
}

//-----------------------------    I D    --------------------------------------
_ID::_ID(){}

void _ID::proceed(){
	unsigned int INS=B1.num_rs1;		// Instruction.
	unsigned int opcode=B1.num_rs2;		// Opcode.
		
	unsigned int func3=(INS>>12)&7;
	unsigned int func5=(INS>>30)&1;
	
	unsigned int frag1=0,frag2=0,frag3=0,frag4=0;
	
	int INS_int=(int)(INS);
	
	outputBuffer.currentPC=B1.currentPC;
	outputBuffer.rs1=outputBuffer.rs2=outputBuffer.rd=0;
	outputBuffer.num_rs1=outputBuffer.num_rs2=0;
	outputBuffer.imm=0;
	
	if (INS==terminateCmd){
		outputBuffer.cmd=TERMINATE;
		return;
	}
	switch (opcode){
		case 0:						//NOP
			outputBuffer.cmd=NOP;
			break;
		case 0x37:					//LUI
			outputBuffer.cmd=LUI;
			outputBuffer.rd=(INS>>7)&31;
			outputBuffer.imm=(INS_int>>12)<<12;
			break;
		case 0x17:					//AUIPC
			outputBuffer.cmd=AUIPC;
			outputBuffer.rd=(INS>>7)&31;
			outputBuffer.imm=(INS_int>>12)<<12;
			break;
		case 0x6f:					//JAL
			outputBuffer.cmd=JAL;
			outputBuffer.rd=(INS>>7)&31;
			frag1=(INS>>31)&1;
			frag2=(INS>>21)&1023;
			frag3=(INS>>20)&1;
			frag4=(INS>>12)&255;
			outputBuffer.imm=(((int)((frag2<<1)|(frag3<<11)|(frag4<<12)|(frag1<<20)))<<11)>>11;
			break;
		case 0x67:					//JALR
			outputBuffer.cmd=JALR;
			outputBuffer.rd=(INS>>7)&31;
			outputBuffer.rs1=(INS>>15)&31;
			outputBuffer.imm=INS_int>>20;
			break;
		default:{					//Others with func3.
			switch (opcode){
				case 0x63:{					// 1100011
					outputBuffer.rs1=(INS>>15)&31;
					outputBuffer.rs2=(INS>>20)&31;
					frag1=(INS>>31)&1;
					frag2=(INS>>25)&63;
					frag3=(INS>>8)&15;
					frag4=(INS>>7)&1;
					outputBuffer.imm=(((int)((frag3<<1)|(frag2<<5)|(frag4<<11)|(frag1<<12)))<<19)>>19;
					switch (func3){
						case 0:				//BEQ
							outputBuffer.cmd=BEQ;
							break;
						case 1:				//BNE
							outputBuffer.cmd=BNE;
							break;
						case 4:				//BLT
							outputBuffer.cmd=BLT;
							break;
						case 5:				//BGE
							outputBuffer.cmd=BGE;
							break;
						case 6:				//BLTU
							outputBuffer.cmd=BLTU;
							break;
						case 7:				//BGEU
							outputBuffer.cmd=BGEU;
							break;
					}
					break;
				}
				case 0x3:{					//0000011
					outputBuffer.rd=(INS>>7)&31;
					outputBuffer.rs1=(INS>>15)&31;
					outputBuffer.imm=INS_int>>20;
					switch (func3){
						case 0:{			//LB
							outputBuffer.cmd=LB;
							break;
						}
						case 1:{			//LH
							outputBuffer.cmd=LH;
							break;
						}
						case 2:{			//LW
							outputBuffer.cmd=LW;
							break;
						}
						case 4:{			//LBU
							outputBuffer.cmd=LBU;
							break;
						}
						case 5:{			//LHU
							outputBuffer.cmd=LHU;
							break;
						}
					}
					break;
				}
				case 0x23:{					//0100011
					outputBuffer.rs1=(INS>>15)&31;
					outputBuffer.rs2=(INS>>20)&31;
					frag1=(INS>>25);
					frag2=(INS>>7)&31;
					outputBuffer.imm=(((int)(frag2|(frag1<<5)))<<20)>>20;
					switch (func3){
						case 0:{			//SB
							outputBuffer.cmd=SB;
							break;
						}
						case 1:{			//SH
							outputBuffer.cmd=SH;
							break;
						}
						case 2:{			//SW
							outputBuffer.cmd=SW;
							break;
						}
					}
					break;
				}
				case 0x13:{					//0010011
					outputBuffer.rd=(INS>>7)&31;
					outputBuffer.rs1=(INS>>15)&31;
					outputBuffer.imm=INS_int>>20;		//SRAI special judge.
					switch (func3){
						case 0:{			//ADDI
							outputBuffer.cmd=ADDI;
							break;
						}
						case 2:{			//SLTI
							outputBuffer.cmd=SLTI;
							break;
						}
						case 3:{			//SLTIU
							outputBuffer.cmd=SLTIU;
							break;
						}
						case 4:{			//XORI
							outputBuffer.cmd=XORI;
							break;
						}
						case 6:{			//ORI
							outputBuffer.cmd=ORI;
							break;
						}
						case 7:{			//ANDI
							outputBuffer.cmd=ANDI;
							break;
						}
						case 1:{			//SLLI
							outputBuffer.cmd=SLLI;
							break;
						}
						case 5:{
							if (!func5){	//SRLI
								outputBuffer.cmd=SRLI;
							}
							else{			//SRAI
								outputBuffer.imm=(INS_int>>20)&31;
								outputBuffer.cmd=SRAI;
							}
							break;
						}
					}
					break;
				}
				default:{					//0110011
					outputBuffer.rd=(INS>>7)&31;
					outputBuffer.rs1=(INS>>15)&31;
					outputBuffer.rs2=(INS>>20)&31;				
					switch (func3){
						case 0:{
							if (!func5){	//ADD
								outputBuffer.cmd=ADD;	
							}
							else{			//SUB
								outputBuffer.cmd=SUB;
							}
							break;
						}
						case 1:{			//SLL
							outputBuffer.cmd=SLL;
							break;
						}
						case 2:{			//SLT
							outputBuffer.cmd=SLT;
							break;
						}
						case 3:{			//SLTU
							outputBuffer.cmd=SLTU;
							break;
						}
						case 4:{			//XOR
							outputBuffer.cmd=XOR;
							break;
						}
						case 5:{
							if (!func5){	//SRL
								outputBuffer.cmd=SRL;
							}
							else{			//SRA
								outputBuffer.cmd=SRA;
							}
							break;
						}
						case 6:{			//OR
							outputBuffer.cmd=OR;
							break;
						}
						case 7:{			//AND
							outputBuffer.cmd=AND;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}
	
	outputBuffer.num_rs1=R[outputBuffer.rs1].read();
	outputBuffer.num_rs2=R[outputBuffer.rs2].read(); 
}

//------------------------------    E X    -------------------------------------
_EX::_EX(){}

void _EX::proceed(){
	// num_rs1 & num_rs2 are used for result storage.
	outputBuffer=B2;
	outputBuffer.num_rs1=outputBuffer.num_rs2=0;
	switch (outputBuffer.cmd){
		case TERMINATE:
		case NOP:{
			// DO NOTHING.
			break;
		}
		case LUI:{
			outputBuffer.num_rs1=B2.imm;
			break;
		}
		case AUIPC:{
			outputBuffer.num_rs1=B2.currentPC+B2.imm;
			break;
		}
		// For all of the jump cmd, candidate PC is stored in num_rs2.
		case JAL:{
			outputBuffer.num_rs2=B2.currentPC+B2.imm;
			outputBuffer.num_rs1=B2.currentPC+4;
			break;
		}
		case JALR:{			
			outputBuffer.num_rs2=B2.num_rs1+B2.imm;
			outputBuffer.num_rs1=B2.currentPC+4;
			break;
		}
		case BEQ:{
			if (B2.num_rs1==B2.num_rs2){
				outputBuffer.num_rs2=B2.currentPC+B2.imm;
				outputBuffer.num_rs1=1;						// Represent that jump successfully.
			}
			break;
		}
		case BNE:{
			if (B2.num_rs1!=B2.num_rs2){
				outputBuffer.num_rs2=B2.currentPC+B2.imm;
				outputBuffer.num_rs1=1;						// Represent that jump successfully.
			}
			break;
		}
		case BLT:{
			if ((int)B2.num_rs1<(int)B2.num_rs2){
				outputBuffer.num_rs2=B2.currentPC+B2.imm;
				outputBuffer.num_rs1=1;						// Represent that jump successfully.
			}
			break;
		}
		case BGE:{
			if ((int)B2.num_rs1>=(int)B2.num_rs2){
				outputBuffer.num_rs2=B2.currentPC+B2.imm;
				outputBuffer.num_rs1=1;						// Represent that jump successfully.
			}
			break;
		}
		case BLTU:{
			if (B2.num_rs1<B2.num_rs2){
				outputBuffer.num_rs2=B2.currentPC+B2.imm;
				outputBuffer.num_rs1=1;						// Represent that jump successfully.
			}
			break;
		}
		case BGEU:{
			if (B2.num_rs1>=B2.num_rs2){
				outputBuffer.num_rs2=B2.currentPC+B2.imm;
				outputBuffer.num_rs1=1;						// Represent that jump successfully.
			}
			break;
		}
		case LB:											// Compute memory address.
		case LH:
		case LW:
		case LBU:
		case LHU:
		case SB:
		case SH:
		case SW:{
			outputBuffer.num_rs1=B2.num_rs1+B2.imm;
			outputBuffer.num_rs2=B2.num_rs2;				// In terms of Lcmd, it doesn't matter.
															// In terms of Scmd, this is the target val.
			break;
		}
		case ADDI:{
			outputBuffer.num_rs1=B2.num_rs1+B2.imm;
			break;
		}
		case SLTI:{
			outputBuffer.num_rs1=(int)B2.num_rs1<B2.imm;
			break;
		}
		case SLTIU:{
			outputBuffer.num_rs1=B2.num_rs1<(unsigned int)B2.imm;
			break;
		}
		case XORI:{
			outputBuffer.num_rs1=B2.num_rs1 ^ B2.imm;
			break;
		}
		case ORI:{
			outputBuffer.num_rs1=B2.num_rs1 | B2.imm;
			break;
		}
		case ANDI:{
			outputBuffer.num_rs1=B2.num_rs1 & B2.imm;
			break;
		}
		case SLLI:{
			outputBuffer.num_rs1=B2.num_rs1 << B2.imm;
			break;
		}
		case SRLI:{
			outputBuffer.num_rs1=(unsigned int)B2.num_rs1 >> B2.imm;
			break;
		}
		case SRAI:{
			outputBuffer.num_rs1=(unsigned int)((int)B2.num_rs1 >> B2.imm);
			break;
		}
		case ADD:{
			outputBuffer.num_rs1=B2.num_rs1+B2.num_rs2;
			break;
		}
		case SUB:{
			outputBuffer.num_rs1=B2.num_rs1-B2.num_rs2;
			break;
		}
		case SLL:{
			unsigned int digits=B2.num_rs2&31;
			outputBuffer.num_rs1=B2.num_rs1 << digits;
			break;
		}
		case SLT:{
			outputBuffer.num_rs1=(int)B2.num_rs1<(int)B2.num_rs2;
			break;
		}
		case SLTU:{
			outputBuffer.num_rs1=B2.num_rs1<B2.num_rs2;
			break;
		}
		case XOR:{
			outputBuffer.num_rs1=B2.num_rs1^B2.num_rs2;
			break;
		}
		case SRL:{
			unsigned int digits=B2.num_rs2&31;
			outputBuffer.num_rs1=B2.num_rs1 >> digits;
			break;
		}
		case SRA:{
			unsigned int digits=B2.num_rs2&31;
			outputBuffer.num_rs1=(unsigned int)(((int)B2.num_rs1) >> digits);
			break;
		}
		case OR:{
			outputBuffer.num_rs1=B2.num_rs1 | B2.num_rs2;
			break;
		}
		case AND:{
			outputBuffer.num_rs1=B2.num_rs1 & B2.num_rs2;
			break;
		}
	}
}

//------------------------------    M E    -------------------------------------
_ME::_ME(){}

void _ME::proceed(){
	outputBuffer=B3;
	outputBuffer.num_rs1=outputBuffer.num_rs2=0;
	unsigned int pos=B3.num_rs1;
	switch (B3.cmd){
		case TERMINATE:{
			//DO NOTHING
			break;
		}
		// The address of ALL MEMORY ACCESS CMD are stored in num_rs1 of B3.
		// The result of MEMORY READ CMD are stored in num_rs1 of ME. 
		case LB:{
			outputBuffer.num_rs1=(unsigned int)(((int)(memoryData.fetchByte(pos))<<24)>>24);
			break;
		}
		case LH:{
			outputBuffer.num_rs1=(unsigned int)(((int)(memoryData.fetchHalfWord(pos))<<16)>>16);
			break;
		}
		case LW:{
			outputBuffer.num_rs1=memoryData.fetchInt(pos);
			break;
		}
		case LBU:{
			outputBuffer.num_rs1=memoryData.fetchByte(pos);
			break;
		}
		case LHU:{
			outputBuffer.num_rs1=memoryData.fetchHalfWord(pos);
			break;
		}
		case SB:{
			memoryData.writeByte(pos,B3.num_rs2);
			break;
		}
		case SH:{
			memoryData.writeHalfWord(pos,B3.num_rs2);
			break;
		}
		case SW:{
			memoryData.writeInt(pos,B3.num_rs2);
			break;
		}
		default:{
			// Restoration.
			outputBuffer.num_rs1=B3.num_rs1;
			outputBuffer.num_rs2=B3.num_rs2;
			// DO NOTHING.
			break;
		}
	}
}

//---------------------------    W B    ----------------------------------------
_WB::_WB(){}

void _WB::proceed(){
	outputBuffer=B4;
	if (B4.cmd==TERMINATE){
		shouldTerminate=true;
		return;
	}
	switch (B4.cmd){
		case NOP:
		case BEQ:
		case BNE:
		case BLT:
		case BGE:
		case BLTU:
		case BGEU:
		case SB:
		case SH:
		case SW:{
			//DO NOTHING.
			break;
		}
		default:{		// Target val in num_rs1 of B4.
			if (B4.rd) R[B4.rd].setInput(B4.num_rs1);
			break;
		}
	}
}

}
