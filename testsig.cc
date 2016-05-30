#include <iostream>
#include <iomanip>
#include <vector>
#include <array>


#include "names.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"

int main(int argc, char **argv)
//This Script tests each of the files defined above, to check if the error lines and error codes agree with what actually happens.
{

  names Nametable;
  sigsequence signalin;
  signalin.push_back(1);



  network newnetwork(&Nametable);
  devices newdevice(&Nametable,&newnetwork);
  monitor newmonitor(&Nametable,&newnetwork);
  /* Testing the MADNESS OF THE SIGGEN  */
  newdevice.makesiggen(Nametable.lookup("SIGGEN1"),signalin);
    //Nametable.dumptablecontents();
  cout << newnetwork.devicelist()->kind <<endl;
  sigsequence newout= newnetwork.devicelist()->signalsequence;
  cout << newout.size() <<endl;
  for(std::vector<int>::size_type i = 0; i != newout.size(); i++) {
    cout << newout[i]<<endl;
}
cout<<"--------------"<<endl;
bool oka=true;
 newmonitor.makemonitor(Nametable.lookup("SIGGEN1"),blankname,oka);
 for (int i=0;i<10;i++){
    newdevice.executedevices(oka);
    newmonitor.recordsignals();
 }
 newmonitor.displaysignals();

  newdevice.debug(true);

}
