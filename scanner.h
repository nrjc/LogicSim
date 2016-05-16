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
		void getsymbol(symbol& s, name& id, int& num);

	private:
		symbol s; // The symbol
		ifstream inf; //Input file
		bool eofile; //This is true if it is the end of file
		char curch; //This is the current character
		names Namestore; //This is an initialisation of the names class, and contains all names.  
		string punct; //This will contain the current punctuation line
		string namestr; //String that is used to contain current names
		
		name getname(ifstream *infp, char &curch, bool &eofile, namestring &str);
		int getnumber(ifstream *infp, char &curch, bool &eofile, int &number);
		void skipspaces(ifstream *infp,char &curch,bool &eofile);
		string getpunct(ifstream *infp,char &curch,bool &eofile);

};

#endif
