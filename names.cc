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
	
  /* over to you */
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
	//There might be a problem here if you try to pull a nameid that doesn't exist. Will sort out later.
	cout << map.right.find(id)->second << endl;
	
}

int names::namelength (name id)
{
	//There might be a problem here if you try to pull a nameid that doesn't exist. Will sort out later. 
	//In this case, might also cause error because of type reassignment from namestring to string. Once again, will check again.
  string tablestring = map.right.find(id)->second;
  return tablestring.length();
  
}
