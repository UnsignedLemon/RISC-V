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
	
	riscv::_PREDICTOR PREDICTOR;
	riscv::_IF        IF;
	riscv::_ID        ID;
	riscv::_EX     	  EX;
	riscv::_ME        ME;
	riscv::_WB        WB;
	
	unsigned int PC=0;
	bool shouldTerminate=0;
	const unsigned int terminateCmd=0x0ff00513;	
}

void updateProcess(){
	riscv::PC=riscv::PREDICTOR.newPC;		riscv::IF.proceed();
	riscv::B1_proceed();					riscv::ID.proceed();
	riscv::B2_proceed();					riscv::EX.proceed();
	riscv::B3_proceed();					riscv::ME.proceed();
	riscv::B4_proceed();					riscv::WB.proceed();
	for (int i=0;i<32;++i) riscv::R[i].write();
	riscv::PREDICTOR.proceed();
}

std::string map[]={
	"TERMINATE","NOP","LUI","AUIPC","JAL","JALR","BEQ","BNE","BLT","BGE","BLTU","BGEU","LB","LH","LW","LBU","LHU","SB","SH","SW","ADDI","SLTI","SLTIU","XORI","ORI","ANDI","SLLI","SRLI","SRAI","ADD","SUB","SLL","SLT","SLTU","XOR","SRL","SRA","OR","AND"
};

std::string rr[]={
	"0","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2",
	"s3","s4","s5","s6","s7","s8","s9","s10","s11"
};

using namespace riscv;

void printProcess(){
	cout<<"PC: "<<std::hex<<riscv::PC<<"\n";
	cout<<"IF: ";
	cout<<std::hex<<IF.INS<<"\n";
	cout<<"B1: ";
	cout<<std::hex<<B1.num_rs1<<" "<<B1.num_rs2<<std::dec<<"\n";
	
	cout<<"ID: ";
	cout<<map[(int)riscv::ID.outputBuffer.cmd]<<"\t";
	cout<<rr[ID.outputBuffer.rd]<<"\t"<<rr[ID.outputBuffer.rs1]<<"\t"<<rr[ID.outputBuffer.rs2]<<"\t";
	cout<<ID.outputBuffer.num_rs1<<" "<<ID.outputBuffer.num_rs2<<"\n";
	cout<<ID.outputBuffer.imm<<"\n";
	cout<<"EX: ";
	cout<<map[(int)riscv::EX.outputBuffer.cmd]<<"\t";
	cout<<rr[EX.outputBuffer.rd]<<"\t"<<rr[EX.outputBuffer.rs1]<<"\t"<<rr[EX.outputBuffer.rs2]<<"\t";
	cout<<EX.outputBuffer.num_rs1<<" "<<EX.outputBuffer.num_rs2<<"\n";
	cout<<ID.outputBuffer.imm<<"\n";
	
	cout<<"REG: \n";
	for (int i=0;i<32;++i) if (R[i].read()) cout<<rr[i]<<":\t"<<R[i].read()<<"\n";
	cout<<"MEM: \n";
	for (int i=0x12c0;i<524288;i+=4) if (memoryData.fetchInt(i)) cout<<i<<":\t"<<memoryData.fetchInt(i)<<"\n";
	system("pause");
}

int main(){
//	std::string str;
//	str="pi";
//	cin>>str;
//	std::string memorySource=pPath+str+sPath;
//	memoryData=DATA(memorySource.c_str());
	memoryData.readFromStandardInput();
	int clk=0;
	while (!riscv::shouldTerminate){
		++clk;
		updateProcess();			// 1-stage process.	
	}
	cout<<(((unsigned int)R[10].read())&255u)<<"\n";
	return 0;
}
