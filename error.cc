#include "error.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

error::error(){
	errorcounter=0;
	this->writeerror("DEVICE BLOCK NOT FOUND"); //0
	this->writeerror("CONNECTION BLOCK NOT FOUND"); //1
	this->writeerror("MONITOR BLOCK NOT FOUND"); //2
	this->writeerror("EXPECTED '{'"); //3
	this->writeerror("EXPECTED AN INTEGER"); //4
	this->writeerror("EXPECTED ':'"); //5
	this->writeerror("DEVICENAME MUST NOT START WITH A NUMBER"); //6
	this->writeerror("DEVICENAME MUST NOT CONINCIDE WITH A KEYWORD"); //7
	this->writeerror("INPUT MUST BE A 0 / 1"); //8
	this->writeerror("NUMBER OF INPUTS MUST BE BETWEEN 1-16"); //9
	this->writeerror("UNIDENTIFIED TYPE"); //10
	this->writeerror("INVALID OUTPUT"); //11
	this->writeerror("EXPECTED DEVICENAME"); //12
	this->writeerror("EXPECTED '.'"); //13
	this->writeerror("DEVICE NOT DEFINED"); //14
	this->writeerror("TOO MANY INPUTS OR INPUT CANNOT BE SMALLER THAN 1"); //15
	this->writeerror("INVALID INPUT"); //16
	this->writeerror("PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT"); //17
	this->writeerror("DEVICENAME IS ALREADY BEING USED"); //18
	this->writeerror("UNIDENTIFIED DEVICE TYPE"); //19
	this->writeerror("NOT ALL INPUTS ARE CONNECTED TO AN OUTPUT"); //20
	this->writeerror("EXPECTED '.'. NEED TO SPECIFY OUTPUT FOR DTYPE"); //21
	this->writeerror("EXPECTED '->'"); //21

}

int error::writeerror(string errordescription){
	map.insert({errorcounter,errordescription});
	errorcounter++;
	return (errorcounter-1);

}

void error::printerror (int id){
	auto search = map.find(id);
    if(search != map.end()) {
        cout << "Error Code "<< search->first << ": "<< search->second ;
    }
    else {
        cout << "Error Code Not found";
    }
}



