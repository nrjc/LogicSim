#ifndef parser_h
#define parser_h

#include "names.h"
#include "scanner.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
using namespace std;

class parser {
	private:
		network* netz; // instantiations of various classes for parser to use.
		devices* dmz; 
		monitor* mmz;
		scanner* smz;
		symbol cursym;
		name curid;
		int curnum;
		void devicelist(void);
		void connectionlist(void);
		void monitorlist(void);
		void device(void);
		void connection(void);
		void parmonitor();
		void error();
	
	public:
		bool readin();
  /* put other stuff that the class uses internally here */
  /* also declare internal functions                     */
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */

		parser(network* network_mod, devices* devices_mod,
		monitor* monitor_mod, scanner* scanner_mod);
    /* the constructor takes pointers to various other classes as parameters */
};

#endif /* parser_h */

	
