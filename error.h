#ifndef error_h
#define error_h

#include <string>
#include <unordered_map>
#include "scanner.h"

using namespace std;


class error{
	unordered_map<int,string> map;
	int mapcounter;
    int totalwarningcount;
    int totalerrorcount;
    scanner* scnr;


public:
  int writeerror (string errordescription); //Writes errordescription as a new error, returns the id associated with the error.
  void printerror (int id); //Prints the error with error id on the command line.
  void printerrornum(); //Prints the total number of errors on the console.



  error(scanner* scanner_mod); // Constructor

};

#endif
