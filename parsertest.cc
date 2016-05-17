#include <iostream>
#include "parser.h"
#include "scanner.h"
#include "names.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"

int main(int argc, char **argv)

{
	
  if (argc != 2) {
    cout << "Usage:         " << argv[0] << " [filename]" << endl;
    exit(1);
  }

  names Nametable;
  scanner newscanner(&Nametable,argv[1]);
  monitor* newmonitor;
  devices* newdevice;
  network* newnetwork;
  parser newparser(newnetwork,newdevice,newmonitor,&newscanner);
  

}
