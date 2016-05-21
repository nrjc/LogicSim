#include <iostream>
#include "parser.h"
#include "scanner.h"
#include "names.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
#include "error.h"

int main(int argc, char **argv)

{

  if (argc != 2) {
    cout << "Usage:         " << argv[0] << " [filename]" << endl;
    exit(1);
  }

  names Nametable;
  scanner newscanner(&Nametable,argv[1]);
  error newerror;

  network newnetwork(&Nametable);
  devices newdevice(&Nametable,&newnetwork);
  monitor newmonitor(&Nametable,&newnetwork);

  parser newparser(&newnetwork,&newdevice,&newmonitor,&newscanner,&newerror);
  newparser.readin();
  newdevice.debug(true);


}
