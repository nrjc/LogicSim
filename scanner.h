#ifndef scanner_h
#define scanner_h
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "names.h"

using namespace std;

typedef enum{
semicol,// ;
devsym,// DEVICE
consym,// CONNECTION
monsym,// MONITORS
namesym, // Defines name. 
numsym, // Defines a number
sclock, // CLOCK 
sswitch, // SWITCH
sdtype, // DTYPE
sxor, // XOR
sand, // AND
snand, // NAND
sor,  //OR
snor, //NOR
inputsym,//For I1-I16, the type will be inputsymbol, and the integer will be in num.
idata, //Input Type - DATA
iclk,  //Input Type - CLK
iset,  //Input Type -SET
iclear, //Input Type - Clear
oq,  //Output Type -Q
oqbar, //Output Type -QBAR
comma,  // ,
stop, // .
colon, // :
opencurly, // {
closecurly, // }
arrow,  //  ->
badsym, // symbol cannot be identified
eofsym // End of file symbol
} symbol;

class scanner
{
	public:
		scanner(names* names_mod, //Pointer to names class
		const char* defname); //Name of file read
		
		~scanner();						
		void getsymbol(symbol& s, name& id, int& num);

	private:
		ifstream inf; //Input file
		bool eofile; //This is true if it is the end of file
		char curch; //This is the current character
		names* Namestore; //This is a local, synchronised copy of the name table that is passed into the scanner.  
		string punct; //This will contain the current punctuation line
		
		name getname(ifstream *infp, char &curch, bool &eofile); 
		int getnumber(ifstream *infp, char &curch, bool &eofile);
		void skipspaces(ifstream *infp,char &curch,bool &eofile);
		string getpunct(ifstream *infp,char &curch,bool &eofile); 
		void skipcomment(ifstream *infp,char &curch,bool &eofile); //this function skips characters until it sees an ending block /*/
		void skipcommentline(ifstream *infp,char &curch,bool &eofile); //This function skips characters until it sees an endline symbol.

};

#endif
