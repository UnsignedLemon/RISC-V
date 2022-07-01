#ifndef UNITS_H
#define UNITS_H

#include "../reg/reg.h"

namespace riscv{

//-------------------------    Class IF    -------------------------------------
class _IF{
public:
	unsigned int INS,opcode,currentPC;
	
public:
	_IF();
	void proceed();
	void clear();
};

//--------------------------    Class ID    ------------------------------------
class _ID{
public:
	// Same as shouldBubble.
	bool isUnknown;			// If target source REG is required from a LOAD cmd right before the
							// current cmd, then a BUBBLE is a must, and the val of REG is UNKNOWN.
							// In other cases, although the val of REG may be unknown in ID process,
							// in B2 the val can be acquired by FORWARDING.
	BUFFER outputBuffer;
public:
	_ID();
	void proceed();
	void clear();
};

//---------------------------    Class EX    -----------------------------------
class _EX{
public:
	BUFFER outputBuffer;
public:
	_EX();
	void proceed();
	void clear();
};

//---------------------------    Class ME    -----------------------------------
class _ME{
private:
	int stage;			// To simulate 3-cycles MEM access.
public:
	BUFFER outputBuffer;
public:
	_ME();
	void proceed();
	void clear();
};

//---------------------------    Class WB    -----------------------------------
class _WB{
public:
	BUFFER outputBuffer;
public:
	_WB();
	void proceed();
	void clear();
};
}

#endif
