#ifndef scanner_h
#define scanner_h
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "names.h"

using namespace std;

typedef enum{namesym, numsym, devsym, consym, monsym, endsym, colon, stop, comma, semicol, opencurly, closecurly, arrow, badsym, eofsym} symbol;

class scanner
{
	public:
		scanner(names* names_mod, const char* defname);
		~scanner();						
		void getsymbol(symbol &s, name &id, int &num);

	private:
		ifstream inf; //Input file
		bool eofile; //This is true if it is the end of file
		char curch; //This is the current character
		names Namestore; //This is a local, synchronised copy of the name table that is passed into the scanner.  
		string punct; //This will contain the current punctuation line
		
		name getname(ifstream *infp, char &curch, bool &eofile);
		int getnumber(ifstream *infp, char &curch, bool &eofile);
		void skipspaces(ifstream *infp,char &curch,bool &eofile);
		string getpunct(ifstream *infp,char &curch,bool &eofile);
		//need to create something that skips all comments. 

};

#endif
