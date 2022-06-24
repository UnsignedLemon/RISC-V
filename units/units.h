#ifndef UNITS_H
#define UNITS_H

#include "../reg/reg.h"

namespace riscv{

//----------------------    Class Predictor    ---------------------------------
class _PREDICTOR{
public:
	unsigned int newPC;
public:
	_PREDICTOR();
	void proceed();	
};

//-------------------------    Class IF    -------------------------------------
class _IF{
public:
	unsigned int INS,opcode,currentPC;
	bool isJumpCmd;
	
public:
	_IF();
	void proceed();
};

//--------------------------    Class ID    ------------------------------------
class _ID{
public:
	BUFFER outputBuffer;
public:
	_ID();
	void proceed();
};

//---------------------------    Class EX    -----------------------------------
class _EX{
public:
	BUFFER outputBuffer;
public:
	_EX();
	void proceed();
};

//---------------------------    Class ME    -----------------------------------
class _ME{
public:
	BUFFER outputBuffer;
public:
	_ME();
	void proceed();
};

//---------------------------    Class WB    -----------------------------------
class _WB{
public:
	BUFFER outputBuffer;
public:
	_WB();
	void proceed();
};
}

#endif
