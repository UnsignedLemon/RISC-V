#ifndef RESOURCES_H
#define RESOURCES_H

#include <string>
#include "data/data.h"
#include "reg/reg.h"
#include "units/units.h"
#include "enum.h"

namespace riscv{
	
	extern riscv::DATA memoryData;
	
	extern riscv::REGISTER R[32];
	extern riscv::BUFFER B1,B2,B3,B4;
	
	extern riscv::_PREDICTOR PREDICTOR;
	extern riscv::_IF        IF;
	extern riscv::_ID        ID;
	extern riscv::_EX      	 EX;
	extern riscv::_ME		 ME;
	extern riscv::_WB		 WB;
	
	extern unsigned int PC;
	extern bool shouldTerminate;
	extern const unsigned int terminateCmd;

}

extern std::string map[];

#endif
