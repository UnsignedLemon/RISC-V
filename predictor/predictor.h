#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "../enum.h"
#include <string>
using std::string;

namespace riscv{

class PREDICTOR;
//-----------------------    Class PREDICTOR_BASE    ---------------------------
class PREDICTOR_BASE{
	friend class PREDICTOR;
public:
	//========================    Parameters    ================================
	static const int historyDigit=9;						// Number of last branches to be recorded. 
	static const int PCDigit=8;								// Number of PC digits to be 'hashed' in table.
	static const unsigned int PCMask=((1<<PCDigit)-1)<<2;	// Mask of PC. Notice that the last 2 bits are not taken in.
	static const int historySize=1<<historyDigit;
	static const int PCTableSize=1<<PCDigit;
	
	static const int trainLimit=historyDigit*2;				// If > trainLimit, use LHP/GHP. else use 2bCounter.
	
	//=========================    Base Class    ===============================
protected:
	string name;
	int tot,hit;
	unsigned int targetAddress[PCTableSize];				// If jump, target address.
	
public:
	PREDICTOR_BASE(const char* str);
	virtual void doPredict(unsigned int &currentPC) = 0;
	virtual void update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int jumpAddress,bool isThisPred) = 0;
	void showAccuracy();
	virtual void clear();
}; 

//--------------------------    Class LHP    -----------------------------------

class LOCAL_HISTORY_PREDICTOR : public PREDICTOR_BASE{
private:
	unsigned int branchHistoryTable[PCTableSize];			// Store result history of this PC.
	unsigned int PCTable[PCTableSize][historySize];			// 2-bit counter of PC & historyPattern.

public:
	LOCAL_HISTORY_PREDICTOR();
	void doPredict(unsigned int &currentPC);
	void update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int jumpAddress,bool isThisPred);
	void clear();
};

//--------------------------    Class GHP    -----------------------------------

class GLOBAL_HISTORY_PREDICTOR : public PREDICTOR_BASE{
private:
	unsigned int PCTable[PCTableSize][historySize];			// 2-bit counter of PC & historyPattern.
	unsigned int globalPattern;

public:
	GLOBAL_HISTORY_PREDICTOR();
	void doPredict(unsigned int &currentPC);
	void update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int jumpAddress,bool isThisPred);
	void clear();
	
};

//-------------------------    Class TBC    ------------------------------------

class TWO_BIT_COUNTER : public PREDICTOR_BASE{
private:
	unsigned int twoBitCounter[PCTableSize];

public:
	TWO_BIT_COUNTER();
	void doPredict(unsigned int &currentPC);
	void update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int jumpAddress,bool isThisPred);
	void clear();
};

//-------------------------    Class PREDICTOR    ------------------------------

// Fuck Return Address Stack!

// Hybrid Predictor.
// Not trained: 2-bit counter.
// Trained: Local history predictor vs Global history predictor.
// Using trainCnt of every PC to track run times.
class PREDICTOR{
private:
	LOCAL_HISTORY_PREDICTOR lhp;
	GLOBAL_HISTORY_PREDICTOR ghp;
	TWO_BIT_COUNTER tbc;
	int trainCycle[PREDICTOR_BASE::PCTableSize];
	unsigned int predictorChoice[PREDICTOR_BASE::PCTableSize];
	
public:
	PREDICTOR();
	void doPredict(unsigned int &currentPC);
	void update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int address,bool isJumpCmd);
	void showAccuracy();
	void clear();
};

}

#endif
