#include "names.h"
#include <iostream>
#include <string>

int main(int argc, char **argv)
// If this program is put in a file called small.cc, it can
// be compiled with the command  g++ -g -o small small.cc
{
	/*
  ifstream inf; // input file
  if (argc != 2) {
    cout << "Usage:         " << argv[0] << " [filename]" << endl;
    exit(1);
  }

  inf.open(argv[1]);
  if (!inf) {
    cout << "Error: cannot open file " << argv[1] << " for reading" << endl;
    exit(1);
  }*/
  
  names Nametable;
  cout << Nametable.lookup("hahaha") <<endl;
  cout << Nametable.cvtname("hahaha") <<endl;
  cout << Nametable.cvtname("whoops") <<endl;
  name inserted = Nametable.cvtname("hahaha");
  Nametable.writename(inserted);
  cout<<"has length "<<Nametable.namelength(inserted)<<endl;
  cout << Nametable.getnamefromtable(inserted);
  
 // inf.close();
}
