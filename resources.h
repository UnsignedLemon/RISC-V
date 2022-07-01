#ifndef RESOURCES_H
#define RESOURCES_H

#include <string>
#include "data/data.h"
#include "reg/reg.h"
#include "units/units.h"
#include "predictor/predictor.h"
#include "enum.h"

namespace riscv{
	
//--------------------------    DATA    ----------------------------------------
	extern riscv::DATA memoryData;
	
//-------------------------    REGISTERS    ------------------------------------
	extern riscv::REGISTER R[32];
	extern riscv::BUFFER B1,B2,B3,B4;
	extern riscv::_PC        PC;
	
//---------------------------    UNITS    --------------------------------------
	extern riscv::_IF        IF;
	extern riscv::_ID        ID;
	extern riscv::_EX      	 EX;
	extern riscv::_ME		 ME;
	extern riscv::_WB		 WB;
	
//-------------------------    PREDICTOR    ------------------------------------
	extern riscv::PREDICTOR  PRED;
	
//---------------    Const Instruction Referred to as END    -------------------
	extern const unsigned int terminateCmd;

//-------------------------    ENVIRONMENT    ----------------------------------
	extern bool shouldTerminate;
	extern bool shouldPause;
	extern bool shouldBubble;
	extern bool shouldDiscard;
	extern bool shouldPopPCQ;
	extern bool isTaken;		// Used to update PREDICTOR.
}

#endif
