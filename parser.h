#ifndef parser_h
#define parser_h

#include "names.h"
#include "scanner.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
#include "error.h"
using namespace std;

class parser {
	private:
		network* netz; // instantiations of various classes for parser to use.
		devices* dmz;
		monitor* mmz;
		scanner* smz;
		error* err;
		symbol cursym;
		name curid;

		devicekind devtypetemp; //Plaeholder variable to store the device kind before it is created.
		name devnametemp; //Plaeholder variable to store the device name before it is created. Also stores the device name of the output.
		name outputnametemp;
		name devnamtempinp; //Placeholder variable to store the device name of the input that will be connected to by the output.
		name inputnametemp;
		devlink devlinktemp; //Placeholder variable to store links to devices
		bool okcheck;
		bool errorsfound; //Is false when there are no compile errors, is true if there are compile errors

		int curnum;
		void devicelist(void);
		void connectionlist(void);
		void monitorlist(void);
		void device(void);
		void connection(void);
		void parmonitor();
		void errorparser(int errin, symbol stop=badsym);


	public:
		bool readin();
  /* put other stuff that the class uses internally here */
  /* also declare internal functions                     */
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */

		parser(network* network_mod, devices* devices_mod, monitor* monitor_mod, scanner* scanner_mod, error* error_mod);
    /* the constructor takes pointers to various other classes as parameters */
};

#endif /* parser_h */


