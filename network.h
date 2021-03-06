#ifndef network_h
#define network_h

#include "names.h"

/* Network specification */

typedef enum {falling, low, rising, high} asignal;
typedef enum {aswitch, aclock, andgate, nandgate, orgate,
	      norgate, xorgate, dtype, siggen, baddevice} devicekind;
typedef vector<int> sigsequence;

struct outputrec {
  name       id;
  asignal    sig;
  outputrec* next;
};
typedef outputrec* outplink;
struct inputrec {
  name      id;
  outplink  connect;
  inputrec* next;
};
typedef inputrec* inplink;
struct devicerec {
  name id;
  inplink ilist;
  outplink olist;
  devicerec* next;
  devicekind kind;
  /* the next elements are only used by some of the device kinds */
  asignal swstate;      // used when kind == aswitch
  int frequency;        // used when kind == aclock
  int counter;          // used when kind == aclock or siggen or dtype
  asignal memory;       // used when kind == dtype
  sigsequence signalsequence; //used when kind == siggen
  bool hold; //used when kind == dtype for implementation of holding time
};
typedef devicerec* devlink;

class network {
  names* nmz;  // the instatiation of the names class that we are going to use.

 public:
  devlink devicelist (void);
    /* Returns list of devices                                             */

  devlink finddevice (name id);
   /* Returns link to device with specified name. Returns NULL if not       */
   /* found.                                                               */

  inplink findinput (devlink dev, name id);
    /* Returns link to input of device pointed to by dev with specified    */
    /* name.  Returns NULL if not found.                                    */

  int getnumberofinputs (name id);
  /* Given a device name, gets the number of inputs that the device can take. */

  outplink findoutput (devlink dev, name id);
    /* Returns link to output of device pointed to by dev with specified   */
    /* name.  Returns NULL if not found.                                    */

  void adddevice (devicekind dkind, name did, devlink& dev);
    /* Adds a device to the device list with given name and returns a link */
    /* to it via 'dev'.                                                    */

  void addinput (devlink dev, name iid);
    /* Adds an input to the device pointed to by 'dev' with the specified  */
    /* name.                                                               */

  void addoutput (devlink dev, name oid);
    /* Adds an output to the device pointed to by 'dev' with the specified */
    /* name.                                                               */

  void makeconnection (name idev, name inp, name odev, name outp, bool& ok);
    /* Makes a connection between the 'inp' input of device 'idev' and the */
    /* 'outp' output of device 'odev'. 'ok' is set true if operation       */
    /* succeeds.                                                           */

  void checknetwork (bool& ok);
    /* Checks that all inputs are connected to an output.                  */

    void resetallcounters();
    /* Resets the counter of all devices. Called when pressing run */

  network (names* names_mod);
  /* Called on system initialisation.                                      */

  void randdevices();
  /*Randomize devicelist for every execution of executedevices() so that
  order of declaration by user does not matter.
  Clock is still at the bottom of the list.                            */


 private:
  devlink firstdev;          // the list of devices
  devlink lastdev;       // last device in list of devices

};

#endif /* network_h */
