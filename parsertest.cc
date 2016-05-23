#include <iostream>
#include "parser.h"
#include "scanner.h"
#include "names.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
#include "error.h"

char* filestotest[]= {"./test/test1","./test/test2","./test/test3","./test/test4","./test/test5"};
int numoftest = 5;


int main(int argc, char **argv)
{
  for (int i=0;i<numoftest;i++){
  names Nametable;
  scanner newscanner(&Nametable,filestotest[i]);
  error newerror(&newscanner);

  network newnetwork(&Nametable);
  devices newdevice(&Nametable,&newnetwork);
  monitor newmonitor(&Nametable,&newnetwork);

  newdevice.debug(true);

  parser newparser(&newnetwork,&newdevice,&newmonitor,&newscanner,&newerror);
  newparser.readin();
}

}
