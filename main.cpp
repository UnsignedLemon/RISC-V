// RISC-V simulator by UnsignedLemon.
// 5-stage pipeline.
// Support forwarding, process parallelization, and a hybrid predictor.

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

void updateRegister(){		// In random order.
	
	riscv::B1_proceed();
	riscv::B4_proceed();
	riscv::B3_proceed();
	for (int i=0;i<32;++i) riscv::R[i].write();
	riscv::PC.proceed();
	riscv::B2_proceed();

}

void updateUnit(){			// In random order.
	riscv::ME.proceed();
	riscv::ID.proceed();
	riscv::WB.proceed();
	riscv::IF.proceed();
	riscv::EX.proceed();
}

const char* pPath="testcases/";
const char* sPath=".data";

int main(){
//------------------------------------------------------------------------------
#ifdef OFFLINE
	std::string str;
	cin>>str;
	std::string memorySource=pPath+str+sPath;
	riscv::memoryData=riscv::DATA(memorySource.c_str());
#endif

#ifndef OFFLINE
	riscv::memoryData.readFromStandardInput();
#endif

//------------------------------------------------------------------------------
	int clk=1;	
	updateUnit();
	while (!riscv::shouldTerminate){
		++clk;
		updateRegister();
		updateUnit();
	}
	cout<<(((unsigned int)riscv::R[10].read())&255u)<<"\n";

//------------------------------------------------------------------------------
#ifdef OFFLINE
	cout<<"cycles: "<<clk<<"\n";
	riscv::PRED.showAccuracy();
#endif
	
	return 0;
}
