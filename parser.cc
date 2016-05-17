#include <iostream>
#include "parser.h"
#include "scanner.h"
#include "names.h"

using namespace std;

parser::parser(network* network_mod, devices* devices_mod, monitor* monitor_mod, scanner* scanner_mod, error* error_mod)
{
	netz = network_mod;  /* make internal copies of these class pointers */
	dmz = devices_mod;   /* so we can call functions from these classes  */
	mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
	smz = scanner_mod;   /* class you say:                               */
	erz = error_mod; /* netz->makeconnection(i1, i2, o1, o2, ok);   */
	/* any other initialisation you want to do? */
}

bool parser::readin (void){	
	symbol cursym=0;
	name curid=0;
	int curnum=0;
	getsymbol(cursym,curid,curnum); //get the first symbol
	while (cursym!=eofsym){
		if (cursym==devsym){
			devicelist();
			getsymbol(cursym,curid,curnum);
		}
		else if (cursym == consym){
			connectionlist();
			getsymbol(cursym,curid,curnum);
		}
		else if (cursym == monsym){
			monitorlist();
			getsymbol(cursym,curid,curnum);
		}
		else error(); //DEVICES,CONNECTIONS,MONITORS not found
	}
}

void parser::devicelist(void){
	getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		device();
	}
	else error(); // no opencurly error
	while (cursym==semicol){
		getsymbol(cursym,curid,curnum);
		device();
	}
	if (cursym==closecurly) {
		getsymbol(cursym,curid,curnum);
	}
	else error(); // not ended with ; or ,
}

// device() will parse a device specification statement and return the semicol behind

void parser::device(void){
	getsymbol(cursym,curid,curnum);
	if (cursym==sclock){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					// pass number to network class
				}
				else error; // num expected error
			}
			else error(); // no colon error
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must coincide with a keyword error 
	}
	else if (cursym==sswitch){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				getsymbol(cursym,curid,curnum);
				if (cursym==0){
					//parse 0 into network class
				}
				else if (cursym==1){
					//parse 1 into network class
				}
				else error(); // input must be either 0 or 1 error
			}
			else error(); // no colon error
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must coincide with a keyword error 
	}
	else if (cursym==sand){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
					}
					else error(); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else error(); // no colon error
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must coincide with a keyword error 
	}
	else if (cursym==snand){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
					}
					else error(); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else error(); // no colon error
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must coincide with a keyword error 
	}
	else if (cursym==sor){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
					}
					else error(); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else error(); // no colon error
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must coincide with a keyword error 
	}
	else if (cursym==snor){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
					}
					else error(); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else error(); // no colon error
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must not coincide with a keyword error 
	}
	else if (cursym==sdtype){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum); 
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must not coincide with a keyword error
	}
	else if (cursym==sxor){
		getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			getsymbol(cursym,curid,curnum); 
		}
		else if (cursym==numsym){
			error(); // devicename must not start with a number error
		}
		else error(); // devicename must not coincide with a keyword error
	}
}

void parser::connectionlist(void){
	getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		connection();
	}
	else error(); // no opencurly error
	while (cursym==semicol){
		getsymbol(cursym,curid,curnum);
		connection();
	}
	if (cursym==closecurly){
		getsymbol(cursym,curid,curnum);
	}
	else error(); // not ended with ; or ,
}

void parser::connection(void){
	getsymbol(cursym,curid,curnum);
	if (cursym==namesym){
		// parser will connect this device to the network class here
		getsymbol(cursym,curid,curnum);
		if (cursym==stop){
			getsymbol(cursym,curid,curnum);
			if(cursym==oq){
				// parser will tell the network class this output q
				getsymbol(cursym,curid,curnum);
			}
			else if (cursym==oqbar){
				// parser will tell the network class this output q
				getsymbol(cursym,curid,curnum);
			}
			else error(); // invalid output from dtype
		}
		else if (cursym!=arrow){
			error(); // for connection, no output or arrow following devicename
		}
		else{
			cursym=comma;
			while (cursym==comma){
				getsymbol(cursym,curid,curnum);
				if (cursym==namesym){
					getsymbol(cursym,curid,curnum);
					if (cursym==stop){
						getsymbol(cursym,curid,curnum);
						if (cursym==inputsym){
							if (curnum<=16 && curnum>=1){
								// parse num to network class
								getsymbol(cursym,curid,curnum);
							}
							else error(); // current number exceeded 16 or is smaller than 1 error
						}
						else if (cursym==idata){
							// parse data input to network class
							getsymbol(cursym,curid,curnum);
						}
						else if (cursym==iclk){
							// parse clk input to network class
							getsymbol(cursym,curid,curnum);
						}
						else if (cursym==iset){
							// parse set input to network class
							getsymbol(cursym,curid,curnum);
						}
						else if (cursym==iclear){
							// parse clear input to network class
							getsymbol(cursym,curid,curnum);
						}
						else error(); //input formats is wrong
					}
				}
			}
			if (cursym==semicol){
				getsymbol(cursym,curid,curnum);
			}
			else error(); //comma expected or semicol expected 
		}
	}			
}

void parser::monitorlist(void){
	getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		monitor();
	}
	else error(); // no opencurly error
	while (cursym==semicol){
		getsymbol(cursym,curid,curnum);
		monitor();
	}
	if (cursym==closecurly) {
		getsymbol(cursym,curid,curnum);
	}
	else error(); // not ended with ; or ,
}

void parser::monitor(void){
	getsymbol(cursym,curid,curnum);
	if (cursym==namesym){
		// parser to connect this to the network class
		getsymbol(cursym,curid,curnum);
	}
	else error(); // a devicename expected error
}


void parser::error(int errn, symbol stop) {
	//add in more here!
	while ((cursym!=stop)&&(cursym!=eofsym)){
		getsymbol(cursym,curid,curnum);
	}
}
