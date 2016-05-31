#include <iostream>
#include "network.h"

using namespace std;

/***********************************************************************
 *
 * Returns list of devices.
 *
 */
devlink network::devicelist (void)
{
  return firstdev;
}


/***********************************************************************
 *
 * Returns link to device with specified name. Returns NULL if not
 * found.
 *
 */
devlink network::finddevice (name id)
{
  devlink d;
  bool found;
  found = false;
  d = firstdev;
  while ((d != NULL) && (! found)) {
    found = (d->id == id);
    if (! found)
      d = d->next;
  }
  return d;
}


/***********************************************************************
 *
 * Returns link to input of device pointed to by dev with specified
 * name.  Returns NULL if not found.
 *
 */
inplink network::findinput (devlink dev, name id)
{
  inplink i;
  bool found;
  i = dev->ilist;
  found = false;
  while ((i != NULL) && (! found)) {
    found = (i->id == id);
    if (! found)
	i = i->next;
  }
  return i;
}


/***********************************************************************
 *
 * Returns link to output of device pointed to by dev with specified
 * name.  Returns NULL if not found.
 *
 */
outplink network::findoutput (devlink dev, name id)
{
  outplink o;
  bool found;
  o = dev->olist;
  found = false;
  while ((o != NULL) && (! found)) {
    found = (o->id == id);
    if (! found)
	o = o->next;
  }
  return o;
}


/***********************************************************************
 *
 * Adds a device to the device list with given name and returns a link
 * to it via 'dev'.
 *
 */
void network::adddevice (devicekind dkind, name did, devlink& dev)
{
  dev = new devicerec;
  devlink d;
  devlink prev=NULL;
  dev->id = did;
  dev->kind = dkind;
  dev->ilist = NULL;
  dev->olist = NULL;
  if (dkind == dtype) {        // dtypes should be at the head of the list
    if (lastdev == NULL){
	lastdev = dev;
	}
    dev->next = firstdev;
    firstdev = dev;
  }
  else if (dkind==aclock||dkind==siggen) {        // aclock and siggen devices must go last
    dev->next = NULL;
    if (lastdev == NULL) {
      firstdev = dev;
      lastdev = dev;
    }
    else {
      lastdev->next = dev;
      lastdev = dev;
    }
  }
  else{
    if (lastdev == NULL) {
      firstdev = dev;
      lastdev = dev;
      dev->next=NULL;
    }
    else {
        for(d=firstdev;(d!=NULL&&d->kind==dtype);d=d->next){
            prev=d; //NOT SURE WHAT HAPPENS ON EDGE CASE??
        }
        if(prev!=NULL){
            prev->next = dev;
            dev->next = d;
        }else{
            dev->next = firstdev;
            firstdev = dev;
        }

    }
  }
}


/***********************************************************************
 *
 * Adds an input to the device pointed to by 'dev' with the specified
 * name.
 *
 */
void network::addinput (devlink dev, name iid)
{
  inplink i = new inputrec;
  i->id = iid;
  i->connect = NULL;
  i->next = dev->ilist;
  dev->ilist = i;
}


/***********************************************************************
 *
 * Adds an output to the device pointed to by 'dev' with the specified
 * name.
 *
 */
void network::addoutput (devlink dev, name oid)
{
  outplink o = new outputrec;
  o->id = oid;
  o->sig = low;
  o->next = dev->olist;
  dev->olist = o;
}


/***********************************************************************
 *
 * Makes a connection between the 'inp' input of device 'idev' and the
 * 'outp' output of device 'odev'. 'ok' is set true if operation
 * succeeds.
 *
 */
void network::makeconnection (name idev, name inp, name odev, name outp, bool& ok)
{
  devlink din, dout;
  outplink o;
  inplink i;
  din = finddevice (idev);
  dout = finddevice (odev);
  ok = ((din != NULL) && (dout != NULL));
  if (ok) {
    o = findoutput (dout, outp);
    i = findinput (din, inp);
    ok = ((o != NULL) && (i != NULL));
    if (ok)
      i->connect = o;
  }
}


/***********************************************************************
 *
 * Checks that all inputs are connected to an output.
 *
 */
void network::checknetwork (bool& ok)
{
  devlink d;
  inplink i;
  ok = true;
  for (d = firstdev; d != NULL; d = d->next)
    for (i = d->ilist; i != NULL; i = i->next)
      if (i->connect == NULL) {
	cout << "Unconnected Input : ";
	nmz->writename (d->id);
	if (i->id != blankname) {
	  cout << ".";
	  nmz->writename (i->id);
	}
	cout << endl;
	ok = false;
      }
}
/***********************************************************************
 *
 * Given a device name, gets the number of inputs that the device can take.
 * Returns -1 when device cannot be found.
 */
int network::getnumberofinputs (name id)
{
    devlink device1=finddevice(id);
    if(device1==NULL){
        return -1; //Error! Device not found.
    }
    int counter=0;
    inplink p;
    for (p=device1->ilist; p != NULL; p = p->next){
        counter+=1;
    }
    return counter;
}
/***********************************************************************
 *
 * Resets the counter of all devices. Called when pressing run
 *
 *
 *
 *
 */
void network::resetallcounters()
{
  devlink d;
    for (d=firstdev; d != NULL; d = d->next){
        d->counter=0;
    }
}

/***********************************************************************
 *
 * The constructor for the network module.
 * Remember the version of the names module that is used here and
 * shared with other modules.
 * Initialise the list of devices.
 *
 */
network::network (names* names_mod)
{
  nmz = names_mod;
  firstdev = NULL;
  lastdev = NULL;
}

/***********************************************************************
 *
 * Randomize devicelist for every execution of executedevices() so that
 * order of declaration by user does not matter.
 * Clock is still at the bottom of the list.
 *
 */


 void network::randdevices ()
 {
    int randomiser=0;
    devlink d;
    vector<devlink> newdevicelist;
    for (d = devicelist (); d != NULL && d->kind != aclock && d->kind!=siggen; d = d->next) {
        newdevicelist.push_back(d);
    }
    firstdev=d;
    int i=0;
    while (newdevicelist.size()>0){
        randomiser=rand() % newdevicelist.size();
        newdevicelist[randomiser]->next = firstdev;
        firstdev = newdevicelist[randomiser];
        newdevicelist.erase(newdevicelist.begin()+randomiser);
    }
 }
