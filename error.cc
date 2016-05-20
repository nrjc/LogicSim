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
}

int error::writeerror(string errordescription){
	map.insert({errorcounter,errordescription});
	errorcounter++;
	return (errorcounter-1);
	
}

void error::printerror (int id){
	auto search = map.find(id);
    if(search != map.end()) {
        cout << "Error Code "<< search->first << ": "<< search->second << endl;
    }
    else {
        cout << "Error Code Not found";
    }
}



