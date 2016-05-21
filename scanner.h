#ifndef scanner_h
#define scanner_h
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "names.h"

using namespace std;

typedef enum{
semicol,// ; 0
devsym,// DEVICE 1
consym,// CONNECTION 2
monsym,// MONITORS 3
namesym, // Defines name. 4 
numsym, // Defines a number 5
sclock, // CLOCK 6
sswitch, // SWITCH 7
sdtype, // DTYPE 8
sxor, // XOR 9
sand, // AND 10
snand, // NAND 11
sor,  //OR 12
snor, //NOR 13
inputsym,//For I1-I16, the type will be inputsymbol, and the integer will be in num. 14
idata, //Input Type - DATA 15
iclk,  //Input Type - CLK 16
iset,  //Input Type -SET 17
iclear, //Input Type - Clear 18
oq,  //Output Type -Q 19
oqbar, //Output Type -QBAR 20
comma,  // , 21
stop, // . 22
colon, // : 23
opencurly, // { 24
closecurly, // } 25
arrow,  //  -> 26
badsym, // symbol cannot be identified 27
eofsym // End of file symbol 28
} symbol;

class scanner
{
	public:
		scanner(names* names_mod, //Pointer to names class
		const char* defname); //Name of file read
		
		~scanner();						
		void getsymbol(symbol& s, name& id, int& num);
		void getnewline(); // The aim of this is to print out the rest of the line

	private:
		ifstream inf; //Input file
		string inputname; //Stores the name of the input file for IOStream. 
		bool eofile; //This is true if it is the end of file
		char curch; //This is the current character
		names* Namestore; //This is a local, synchronised copy of the name table that is passed into the scanner.  
		string punct; //This will contain the current punctuation line
		int linenumber; //This contains the current line number that the parser is operating on.
		
		name getname(ifstream *infp, char &curch, bool &eofile); 
		int getnumber(ifstream *infp, char &curch, bool &eofile);
		void skipspaces(ifstream *infp,char &curch,bool &eofile);
		string getpunct(ifstream *infp,char &curch,bool &eofile); 
		void skipcomment(ifstream *infp,char &curch,bool &eofile); //this function skips characters until it sees an ending block /*/
		void skipcommentline(ifstream *infp,char &curch,bool &eofile); //This function skips characters until it sees an endline symbol.


};

#endif






