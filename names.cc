#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <boost/bimap.hpp>

using namespace std;


/* Name storage and retrieval routines */

names::names(void)  /* the constructor */
{
	counter=1;
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
	
}

name names::lookup (namestring str)
{
	// Maybe I should add in logic that stops adding things to the bimap when we've exceeded maxnames. 
	// Also need some logic to cut strings that are too long, and prevent them from being added to the table. 
	auto search=map.left.find(str);
	if (search!=map.left.end()){
		return search->second;
	}
	else{
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

namestring getnamefromtable(name index){
	//There might be a problem here if you try to pull a nameid that doesn't exist. Will sort out later.
	auto search=map.right.find(index);
	if (search!=map.right.end()){
		return search->second;
	}
	else{
		cout << "Error, name with index: "<< index <<" not found."<<endl;
		return "";
	}
}

int names::namelength (name id)
{
	//In this case, might also cause error because of type reassignment from namestring to string. Once again, will check again.
  return getnamefromtable(id).length();
}
