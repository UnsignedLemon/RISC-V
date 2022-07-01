#include "resources.h"

namespace riscv{
//----------------------------    Memory    ------------------------------------
riscv::DATA memoryData;

//----------------------------    Register    ----------------------------------
riscv::REGISTER R[32];
riscv::BUFFER B1,B2,B3,B4;
riscv::_PC		  PC;

//------------------------    Function Units    --------------------------------
riscv::_IF        IF;
riscv::_ID        ID;
riscv::_EX     	  EX;
riscv::_ME        ME;
riscv::_WB        WB;
riscv::PREDICTOR  PRED;

//------------------------    End Control    -----------------------------------
bool shouldTerminate=0;
const unsigned int terminateCmd=0x0ff00513;	

//------------------------    Environment    -----------------------------------
bool shouldPause=0;
bool shouldBubble=0;
bool shouldDiscard=0;
bool shouldPopPCQ=0;
bool isTaken=0;

}

