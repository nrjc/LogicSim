#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <boost/bimap.hpp>

using namespace std;


/* Name storage and retrieval routines */

names::names(void)  /* the constructor */
{
	counter=0;
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
