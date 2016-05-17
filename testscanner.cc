#include "names.h"
#include <iostream>
#include <string>
#include "scanner.h"
#include <map>
string enumtypes[] =
{
   "namesym", "numsym", "devsym", "consym", "monsym", "endsym", "colon", "stop", "comma", "semicol", "opencurly", "closecurly", "arrow", "badsym", "eofsym"
};
int main(int argc, char **argv)
// If this program is put in a file called small.cc, it can
// be compiled with the command  g++ -g -o small small.cc
{
	
  if (argc != 2) {
    cout << "Usage:         " << argv[0] << " [filename]" << endl;
    exit(1);
  }

  names Nametable;
  scanner newscanner(&Nametable,argv[1]);
  symbol s;
  name id;
  int num1;
  while(s!=eofsym){
	  newscanner.getsymbol(s,id,num1);
	  cout << enumtypes[s] <<":"<< Nametable.getnamefromtable(id)<<":" <<num1 <<endl;
	  id=0;num1=-1;
  }
}
