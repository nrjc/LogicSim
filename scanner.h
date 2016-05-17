#ifndef scanner_h
#define scanner_h
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "names.h"

using namespace std;

typedef enum{semicol,//0
devsym,//1
consym,//2
monsym,//3
namesym,
numsym,
sclock,//4
sswitch,//5
sdtype,//6
sxor,//7
sand,//8
snand,//9
sor,//10
snor,//11
inputsym,//12
idata,//13
iclk, //14
iset, //15
iclear, //16
oq, //17
oqbar, //18
comma, //19
stop, //20
colon,//21
opencurly, //22
closecurly, //23
arrow, //24
badsym, //25
eofsym //26
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
