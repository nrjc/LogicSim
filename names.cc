#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <boost/bimap.hpp>

using namespace std;


/* Name storage and retrieval routines */

names::names(void)  /* the constructor */
{
	counter=-1;
	//Populating the names class with reserved names and punctuation.
	this->lookup("");
	this->lookup(";"); //0
	this->lookup("DEVICES"); //1
	this->lookup("CONNECTIONS"); //2
	this->lookup("MONITORS"); //3
	this->lookup("CLOCK");//4
	this->lookup("SWITCH"); //5
	this->lookup("DTYPE");//6
	this->lookup("XOR");//7
	this->lookup("AND");//8
	this->lookup("NAND");//9
	this->lookup("OR");//10
	this->lookup("NOR");//11
	this->lookup("I1");//12
	this->lookup("I2");//13
	this->lookup("I3");//14
	this->lookup("I4");//15
	this->lookup("I5");//16
	this->lookup("I6");//17
	this->lookup("I7");//18
	this->lookup("I8");//19
	this->lookup("I9");//20
	this->lookup("I10");//21
	this->lookup("I11");//22
	this->lookup("I12");//23
	this->lookup("I13");//24
	this->lookup("I14");//25
	this->lookup("I15");//26
	this->lookup("I16");//27
	this->lookup("DATA");//28
	this->lookup("CLK");//29
	this->lookup("SET");//30
	this->lookup("CLEAR");//31
	this->lookup("Q");//32
	this->lookup("QBAR");//33
	this->lookup(":");//34
	this->lookup(".");//35
	this->lookup(",");//36
	this->lookup("{");//37
	this->lookup("}");//38
	this->lookup("->");//39
	this->lookup("/*");//40
	this->lookup("//");//41

}

name names::lookup (namestring str)
{
	auto search=map.left.find(str);
	if (search!=map.left.end()){
		return search->second;
	}
	else{
		//truncates name if larger than maxlength.
		if (str.length() > maxlength){
			str = str.substr(0,maxlength);
			cout << "Warning: name `"<<str<<"' was truncated." <<endl;
		}
		map.insert({str,counter});
		counter++;
		return (counter-1);
	}
}

name names::cvtname (namestring str)
{
	auto search=map.left.find(str);
	if (search!=map.left.end()){
		return search->second;
	}
	else{
		return blankname;
	}
}

void names::writename (name id)
{
	cout << getnamefromtable(id) << endl;

}

namestring names::getnamefromtable(name index){
	auto search=map.right.find(index);
	if (search!=map.right.end()){
		return search->second;
	}
	else{
		cout << "Error, name with index: "<< index <<" not found."<<endl;
		return "";
	}
}

 void names::dumptablecontents(){
	 for( const auto& n : map.left ) {
				std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";
			}
 }


int names::namelength (name id)
{
  return getnamefromtable(id).length();
}
