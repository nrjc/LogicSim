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
    vector<int> linenumvec; //This contains a vector of all the lines which contain errors.
    vector<int> errorcodevec;// this contains a vectir of all the error codes


public:
  int writeerror (string errordescription); //Writes errordescription as a new error, returns the id associated with the error.
  void printerror (int id); //Prints the error with error id on the command line, then prints a string containing the error line, then prints a marker to point to the place where the error has occured.
  void printerrornum(); //Prints the total number of errors on the console.
  int gettotalerrornum(); //Returns the total number of errors.
  vector<int> getlinenumvector(); //This gives a vector of the line numbers containing errors as the file is parsed. Used for debugging purposes in conjunction with testparser.
  vector<int> geterrorcodevec();// This gives a vector of the error codes as the file is parsed. Used for debugging purposes in conjunction with testparser.



  error(scanner* scanner_mod); // Constructor

};

#endif
