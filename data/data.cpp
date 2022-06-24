#include "data.h"
using std::cin;
using std::cout;
using std::string;
using std::fstream;

namespace riscv{

static unsigned int getDecimalNumber(const char* c,unsigned int digit){
	unsigned int res=0;
	for (int i=0;i<digit;++i){
		unsigned char bit=(c[i]>='0' && c[i]<='9' ? c[i]-'0' : c[i]-'A'+10);
		res=(res<<4)|bit;
	}
	return res;
}

//-----------------------------    DATA    -------------------------------------
DATA::DATA(){
	memset(dataMemory,0,sizeof(dataMemory));
}

DATA::DATA(const char* path){		// Using files to construct.
	memset(dataMemory,0,sizeof(dataMemory));
	fstream fin;
	fin.open(path);
	if (!fin.is_open()) throw open_file_failed();
	
	char c[10];
	unsigned int pos=0;
	while (fin>>c){
		if (c[0]=='@') pos=getDecimalNumber(c+1,8);
		else dataMemory[pos++]=getDecimalNumber(c,2);
	}
	fin.close();
}

void DATA::readFromStandardInput(){
	memset(dataMemory,0,sizeof(dataMemory));
	char c[10];
	unsigned int pos=0;
	while (cin>>c){
		if (c[0]=='@') pos=getDecimalNumber(c+1,8);
		else dataMemory[pos++]=getDecimalNumber(c,2);
	}
}

unsigned char DATA::fetchData(unsigned int pos) const{
	return dataMemory[pos];
}

unsigned int DATA::fetchInt(unsigned int pos) const{
	unsigned int res=0;
	res=((unsigned int)dataMemory[pos]) | 
		((unsigned int)dataMemory[pos+1]<<8) |
		((unsigned int)dataMemory[pos+2]<<16) |
		((unsigned int)dataMemory[pos+3]<<24);
	return res;
}

unsigned int DATA::fetchHalfWord(unsigned int pos) const{
	unsigned int res=0;
	res=((unsigned int)dataMemory[pos]) | 
		((unsigned int)dataMemory[pos+1]<<8);
	return res;
}

unsigned int DATA::fetchByte(unsigned int pos) const{
	unsigned int res=0;
	res=(unsigned int)dataMemory[pos];
	return res;
}

void DATA::writeInt(unsigned int pos,unsigned int val){
	dataMemory[pos]=val & 0xff;val=val>>8;
	dataMemory[pos+1]=val & 0xff;val=val>>8;
	dataMemory[pos+2]=val & 0xff;val=val>>8;
	dataMemory[pos+3]=val & 0xff;
}

void DATA::writeHalfWord(unsigned int pos,unsigned int val){
	dataMemory[pos]=val & 0xff;val=val>>8;
	dataMemory[pos+1]=val & 0xff;
}

void DATA::writeByte(unsigned int pos,unsigned int val){
	dataMemory[pos]=val & 0xff;
}

void DATA::print() const{
	for (int i=0;i<524288;++i) if (dataMemory[i]) cout<<i<<":"<<(unsigned int)dataMemory[i]<<"\n";
}

}
