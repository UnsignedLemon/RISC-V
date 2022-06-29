// RISC-V by UnsignedLemon
// It's getting dark to dark to see...
// Feel like I'm knocking on knocking on the heaven's door...
#include <iostream>
#include <cstdio>
#include <string>
#include <iomanip>
#include "resources.h"

using std::cin;
using std::cout;
using std::string;

namespace riscv{
	const char* pPath="testcases/";
	const char* sPath=".data";
	
	riscv::DATA memoryData;
	
	riscv::REGISTER R[32];
	riscv::BUFFER B1,B2,B3,B4;
	riscv::_PC		  PC;
	
	riscv::_IF        IF;
	riscv::_ID        ID;
	riscv::_EX     	  EX;
	riscv::_ME        ME;
	riscv::_WB        WB;
	
	bool shouldTerminate=0;
	const unsigned int terminateCmd=0x0ff00513;	
	
	bool shouldPause=0;
	bool shouldBubble=0;
	bool shouldDiscard=0;
	bool shouldPopPCQ=0;
}

std::string map[]={
	"TERMINATE","NOP","LUI","AUIPC","JAL","JALR","BEQ","BNE","BLT","BGE","BLTU","BGEU","LB","LH","LW","LBU","LHU","SB","SH","SW","ADDI","SLTI","SLTIU","XORI","ORI","ANDI","SLLI","SRLI","SRAI","ADD","SUB","SLL","SLT","SLTU","XOR","SRL","SRA","OR","AND"
};

std::string rr[]={
	"0","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2",
	"s3","s4","s5","s6","s7","s8","s9","s10","s11"
};

int cnt=0;
int lastPC=-1;
using namespace riscv;

void updateRegister(){
	for (int i=0;i<32;++i) riscv::R[i].write();
//	if (WB.outputBuffer.cmd!=NOP && (WB.outputBuffer.currentPC!=lastPC)){
//		cout<<std::hex<<WB.outputBuffer.currentPC<<std::dec<<"\n";
//		lastPC=WB.outputBuffer.currentPC;
//		++cnt;
//		cout<<cnt<<"\n";
//		system("pause");
//	}
	riscv::B1_proceed();
	riscv::PC.proceed();
	riscv::B4_proceed();
	riscv::B2_proceed();
	riscv::B3_proceed();
}

void updateUnit(){
	riscv::ID.proceed();
	riscv::IF.proceed();
	riscv::WB.proceed();
	riscv::EX.proceed();
	riscv::ME.proceed();
}

#ifdef DEBUG
using namespace riscv;

void printRegister(){
	cout<<"control state: \n";
	cout<<"PAUSE "<<shouldPause<<"\n";
	cout<<"BUBBLE "<<shouldBubble<<"\n";
	cout<<"DISCARD "<<shouldDiscard<<"\n";
	cout<<"POP_PCQ "<<shouldPopPCQ<<"\n\n";
	
	cout<<"PC= "<<std::hex<<PC.read()<<std::dec<<":\n\n";

	cout<<"PCQ top: "<<std::dec<<PC.PCQ.getFront()<<"\n";
	cout<<"B1:\n";
	cout<<std::hex<<B1.num_rs1<<std::dec<<"\n";
	cout<<"B2:\n";
	cout<<map[B2.cmd]<<" "<<rr[B2.rd]<<" "<<rr[B2.rs1]<<" "<<rr[B2.rs2]<<" "<<B2.imm<<"\n";
	cout<<B2.num_rs1<<" "<<B2.num_rs2<<"\n";
	cout<<"B3:\n";
	cout<<map[B3.cmd]<<" "<<rr[B3.rd]<<" "<<rr[B3.rs1]<<" "<<rr[B3.rs2]<<" "<<B3.imm<<"\n";
	cout<<B3.num_rs1<<" "<<B3.num_rs2<<"\n";
	cout<<"B4:\n";
	cout<<map[B4.cmd]<<" "<<rr[B4.rd]<<" "<<rr[B4.rs1]<<" "<<rr[B4.rs2]<<" "<<B4.imm<<"\n";
	cout<<B4.num_rs1<<" "<<B4.num_rs2<<"\n";
	
	cout<<"REG:\n";
	for (int i=0;i<32;++i) if (R[i].read()) cout<<rr[i]<<": "<<R[i].read()<<"\n";
	
}

void printUnit(){
	cout<<"\n";
	cout<<"IF:\n"<<std::hex<<IF.INS<<"  isJumpCmd "<<IF.isJumpCmd<<"\n"<<std::dec;
	cout<<"ID:\n"<<map[ID.outputBuffer.cmd]<<" "<<rr[ID.outputBuffer.rd]<<" "<<rr[ID.outputBuffer.rs1];
	cout<<" "<<rr[ID.outputBuffer.rs2]<<" "<<ID.outputBuffer.imm<<"\n";
	cout<<ID.outputBuffer.num_rs1<<" "<<ID.outputBuffer.num_rs2<<"\n";

	cout<<"EX:\n"<<map[EX.outputBuffer.cmd]<<" "<<rr[EX.outputBuffer.rd]<<" "<<rr[EX.outputBuffer.rs1];
	cout<<" "<<rr[EX.outputBuffer.rs2]<<" "<<EX.outputBuffer.imm<<"\n";
	cout<<EX.outputBuffer.num_rs1<<" "<<EX.outputBuffer.num_rs2<<"\n";

	cout<<"ME:\n"<<map[ME.outputBuffer.cmd]<<" "<<rr[ME.outputBuffer.rd]<<" "<<rr[ME.outputBuffer.rs1];
	cout<<" "<<rr[ME.outputBuffer.rs2]<<" "<<ME.outputBuffer.imm<<"\n";
	cout<<ME.outputBuffer.num_rs1<<" "<<ME.outputBuffer.num_rs2<<"\n";
	
	cout<<"WB:\n"<<map[WB.outputBuffer.cmd]<<" "<<rr[WB.outputBuffer.rd]<<" "<<rr[WB.outputBuffer.rs1];
	cout<<" "<<rr[WB.outputBuffer.rs2]<<" "<<WB.outputBuffer.imm<<"\n";
	cout<<WB.outputBuffer.num_rs1<<" "<<WB.outputBuffer.num_rs2<<"\n";
	
	cout<<"MEM:\n";
	for (int i=0x119c;i<524288;i+=4){
		if (memoryData.fetchInt(i)) cout<<i<<": "<<memoryData.fetchInt(i)<<"\n";
	}
	
	system("pause");
}
#endif

//---------------------------    D E F    --------------------------------------
#define NOT_SHOW_CLK
#define ONLINE_JUDGE
//------------------------------------------------------------------------------

int main(){
#ifndef ONLINE_JUDGE	
	std::string str;
	cin>>str;
	std::string memorySource=riscv::pPath+str+riscv::sPath;
	riscv::memoryData=riscv::DATA(memorySource.c_str());
#endif
#ifdef ONLINE_JUDGE
	riscv::memoryData.readFromStandardInput();
#endif

	int clk=0;	
	// To start with CMD0.	
	updateUnit();
	//printUnit();
	
	while (!riscv::shouldTerminate){
		++clk;
	//	cout<<"----------------------------------------------\n"<<clk<<"\n";
		updateRegister();
	//	printRegister();
		updateUnit();			// 5-stage process.	
	//	printUnit();
	}
	cout<<(((unsigned int)riscv::R[10].read())&255u)<<"\n";
#ifdef SHOW_CLK
	cout<<clk<<"\n";
#endif

	return 0;
}
