#include "predictor.h"
#include "../exceptions.h"
#include <iostream>
#include <string>
#include <cstring>
using std::cout;

namespace riscv{

//-----------------------    Class PREDICTOR_BASE    ---------------------------
PREDICTOR_BASE::PREDICTOR_BASE(const char* str):name(str),tot(0),hit(0){
	memset(targetAddress,0,sizeof(targetAddress));
}

void PREDICTOR_BASE::showAccuracy(){
	cout<<name<<":\n";
	cout<<"tot: "<<tot<<"\n";
	cout<<"hit: "<<hit<<"\n";
	cout<<"acc: "<<(double)hit/(double)tot<<"\n";
}

void PREDICTOR_BASE::clear(){
	tot=hit=0;
	memset(targetAddress,0,sizeof(targetAddress));
}

//-----------------------    Class LHP    --------------------------------------
LOCAL_HISTORY_PREDICTOR::LOCAL_HISTORY_PREDICTOR():PREDICTOR_BASE("LOCAL HISTORY PREDICTOR"){
	memset(branchHistoryTable,0,sizeof(branchHistoryTable));
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i)
		for (int j=0;j<PREDICTOR_BASE::historySize;++j)
			PCTable[i][j]=1;
}

void LOCAL_HISTORY_PREDICTOR::doPredict(unsigned int &currentPC){
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	unsigned int pattern=branchHistoryTable[id];
	if (id>=PCTableSize || pattern>=historySize) throw index_out_of_bound();
	
	currentPC = ((PCTable[id][pattern]&2) ? targetAddress[id] : currentPC+4);
}

void LOCAL_HISTORY_PREDICTOR::update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int jumpAddress,bool isThisPred){
	hit+=isHit&isThisPred;
	tot+=isThisPred;
	
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	unsigned int pattern=branchHistoryTable[id];
	if (id>=PCTableSize || pattern>=historySize) throw index_out_of_bound();

	//===================    Update history pattern    =========================
	branchHistoryTable[id]<<=1;
	branchHistoryTable[id]|=isTaken;
	branchHistoryTable[id]&=(PREDICTOR_BASE::historySize-1);
	//=======================    Update PCTable    =============================
	if (isTaken){
		if (PCTable[id][pattern]<3) PCTable[id][pattern]++;
		targetAddress[id]=jumpAddress;		// Update new targetAddress in case of address change.
	}
	else{
		if (PCTable[id][pattern]) PCTable[id][pattern]--;
	}
}

void LOCAL_HISTORY_PREDICTOR::clear(){
	this->PREDICTOR_BASE::clear();
	memset(branchHistoryTable,0,sizeof(branchHistoryTable));
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i)
		for (int j=0;j<PREDICTOR_BASE::historySize;++j)
			PCTable[i][j]=1;
}

//-----------------------    Class GHP    --------------------------------------
GLOBAL_HISTORY_PREDICTOR::GLOBAL_HISTORY_PREDICTOR():PREDICTOR_BASE("GLOBAL HISTORY PREDICTOR"){
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i)
		for (int j=0;j<PREDICTOR_BASE::historySize;++j)
			PCTable[i][j]=1;
	globalPattern=0;
}

void GLOBAL_HISTORY_PREDICTOR::doPredict(unsigned int &currentPC){
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	unsigned int pattern=globalPattern;
	if (id>=PCTableSize || pattern>=historySize) throw index_out_of_bound();
	
	currentPC = ((PCTable[id][pattern]&2) ? targetAddress[id] : currentPC+4);
}

void GLOBAL_HISTORY_PREDICTOR::update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int jumpAddress,bool isThisPred){
	hit+=isHit&isThisPred;
	tot+=isThisPred;
	
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	unsigned int pattern=globalPattern;
	if (id>=PCTableSize || pattern>=historySize) throw index_out_of_bound();

	//===================    Update global pattern    ==========================
	globalPattern<<=1;
	globalPattern|=isTaken;
	globalPattern&=(PREDICTOR_BASE::historySize-1);
	//=======================    Update PCTable    =============================
	if (isTaken){
		if (PCTable[id][pattern]<3) PCTable[id][pattern]++;
		targetAddress[id]=jumpAddress;		// Update new targetAddress in case of address change.
	}
	else{
		if (PCTable[id][pattern]) PCTable[id][pattern]--;
	}
}

void GLOBAL_HISTORY_PREDICTOR::clear(){
	this->PREDICTOR_BASE::clear();
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i)
		for (int j=0;j<PREDICTOR_BASE::historySize;++j)
			PCTable[i][j]=1;
	globalPattern=0;
}

//-------------------------    Class TBC    ------------------------------------
TWO_BIT_COUNTER::TWO_BIT_COUNTER():PREDICTOR_BASE("TWO BIT COUNTER"){
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i) twoBitCounter[i]=1;	
}

void TWO_BIT_COUNTER::doPredict(unsigned int &currentPC){
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	if (id>=PCTableSize) throw index_out_of_bound();
	currentPC = (twoBitCounter[id]&2) ? targetAddress[id] : currentPC+4;
}

void TWO_BIT_COUNTER::update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int jumpAddress,bool isThisPred){
	hit+=isHit&isThisPred;
	tot+=isThisPred;
	
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	if (id>=PCTableSize) throw index_out_of_bound();

	if (isTaken){
		if (twoBitCounter[id]<3) twoBitCounter[id]++;
		targetAddress[id]=jumpAddress;
		// Update new targetAddress in case of address change.
	}
	else{
		if (twoBitCounter[id]) twoBitCounter[id]--;
	}		
}

void TWO_BIT_COUNTER::clear(){
	this->PREDICTOR_BASE::clear();
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i) twoBitCounter[i]=1;	
}

//------------------------    Class PREDICTOR    -------------------------------
PREDICTOR::PREDICTOR():lhp(),tbc(),ghp(){
	memset(trainCycle,0,sizeof(trainCycle));
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i) predictorChoice[i]=2;
}

void PREDICTOR::doPredict(unsigned int &currentPC){
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	if (trainCycle[id]<PREDICTOR_BASE::trainLimit) tbc.doPredict(currentPC);
	else{
		if (predictorChoice[id] > 1) lhp.doPredict(currentPC);
		else ghp.doPredict(currentPC);
	}
}

void PREDICTOR::update(unsigned int currentPC,bool isTaken,bool isHit,unsigned int address,bool isJumpCmd){
	unsigned int id=(currentPC & PREDICTOR_BASE::PCMask)>>2;
	
	if (trainCycle[id]<PREDICTOR_BASE::trainLimit){
		tbc.update(currentPC,isTaken,isHit,address,!isJumpCmd);
		lhp.update(currentPC,isTaken,false,address,false);
		ghp.update(currentPC,isTaken,false,address,false);
		++trainCycle[id];		
	}
	else{
		if (predictorChoice[id] > 1){		// LHP
			lhp.update(currentPC,isTaken,isHit,address,!isJumpCmd);
			ghp.update(currentPC,isTaken,false,address,false);
			if (isHit){
				if (predictorChoice[id]<3) ++predictorChoice[id];
			}
			else{
				if (predictorChoice[id]) --predictorChoice[id];
			}
		}
		else{								// GHP
			lhp.update(currentPC,isTaken,false,address,false);
			ghp.update(currentPC,isTaken,isHit,address,!isJumpCmd);
			if (!isHit){
				if (predictorChoice[id]<3) ++predictorChoice[id];
			}
			else{
				if (predictorChoice[id]) --predictorChoice[id];
			}			
		}
	}
}

void PREDICTOR::showAccuracy(){
	tbc.showAccuracy();
	lhp.showAccuracy();
	ghp.showAccuracy();
	cout<<"HYBRID BRANCH PREDICTOR: \n";
	cout<<"tot: "<<tbc.tot+lhp.tot+ghp.tot<<"\n";
	cout<<"hit: "<<tbc.hit+lhp.hit+ghp.hit<<"\n";
	double r=tbc.tot+lhp.tot+ghp.tot?((double)(tbc.hit+lhp.hit+ghp.hit)/double(tbc.tot+lhp.tot+ghp.tot)) : 0;
	cout<<"acc: "<<r<<"\n";
}

void PREDICTOR::clear(){
	memset(trainCycle,0,sizeof(trainCycle));
	for (int i=0;i<PREDICTOR_BASE::PCTableSize;++i) predictorChoice[i]=2;
	ghp.clear();
	tbc.clear();
	lhp.clear();
}

}
