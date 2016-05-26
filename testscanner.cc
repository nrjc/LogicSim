#include "names.h"
#include <iostream>
#include <string>
#include "scanner.h"
#include <map>
string enumtypes[] =
{
   "semicol","devsym","consym","monsym","namesym","numsym","sclock","sswitch","sdtype","sxor","sand","snand","sor","snor","inputsym","idata","clk","iset","iclear","oq"," oqbar","comma","stop","colon","opencurly","closecurly","arrow","badsym","eofsym"
};
int main(int argc, char **argv)
// This small script prints out the symbol, followed by the namestring associated with the nametable, followed by the number. The last two only apply if applicable. Then it dumps the table contents.
//This is used to check the functino of the scanner.
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
  Nametable.dumptablecontents();
}
