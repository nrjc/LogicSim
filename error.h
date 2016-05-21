#ifndef error_h
#define error_h

#include <string>
#include <unordered_map>

using namespace std;


class error{
	unordered_map<int,string> map;	
	int errorcounter;


public:
  int writeerror (string errordescription); //Writes errordescription as a new error, returns the id associated with the error. 
  void printerror (int id); //Prints the error with error id on the command line. 



  error(); // Constructor

};

#endif 
