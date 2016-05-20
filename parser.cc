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
	err = error_mod;
/* netz->makeconnection(i1, i2, o1, o2, ok);   */
	/* any other initialisation you want to do? */
}

bool parser::readin (void){	
	symbol cursym;
	name curid=0;
	int curnum=0;
	smz->getsymbol(cursym,curid,curnum); //get the first symbol
	if (cursym==devsym){
			devicelist();
			smz->getsymbol(cursym,curid,curnum);
	} else {err->printerror(0);return false;}
	if (cursym == consym){	
			connectionlist();
			smz->getsymbol(cursym,curid,curnum);
	} else {err->printerror(1);return false;}
   if (cursym == monsym){
			monitorlist();
			smz->getsymbol(cursym,curid,curnum);
	} else {err->printerror(2);return false;}
	if (cursym==eofsym){
		return true;}
		else {return false;}
}

void parser::devicelist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
		device();
		smz->getsymbol(cursym,curid,curnum);
	}
	else{
		err->printerror(3); // no opencurly error
	}
	while (cursym==semicol){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==closecurly) {
			return;
		} 
		device();
		smz->getsymbol(cursym,curid,curnum);
	}
		
}

// device() will parse a device specification statement and return the semicol behind

void parser::device(void){
	if (cursym==sclock){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					// pass number to network class
				}
				else err->printerror(4); // num expected error
			}
			else err->printerror(5); // no colon error
		}
		else if (cursym==numsym){
			err->printerror(6); // devicename must not start with a number error
		}
		else err->printerror(7); // devicename must coincide with a keyword error 
	}
	else if (cursym==sswitch){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum==0){
					//parse 0 into network class
						return;
					}
					else if (curnum==1){
						//parse 1 into network class
						return;
					}
					else{
						err->printerror(0); //input must be zero or one !!!
					}
				} 
				else{
					err->printerror(4); // input must be a number
				}
			}
			else{
				err->printerror(5); // no colon error
			}
		}
		else if (cursym==numsym){
			err->printerror(6); // devicename must not start with a number error
		}
		else{
			err->printerror(7); // devicename must coincide with a keyword error
		}
	}
	else if (cursym==sand){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
					}
					else err->printerror(0); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else err->printerror(0); // no colon error
		}
		else if (cursym==numsym){
			err->printerror(0); // devicename must not start with a number error
		}
		else err->printerror(0); // devicename must coincide with a keyword error 
	}
	else if (cursym==snand){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
						return;
					}
					else err->printerror(0); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else err->printerror(0); // no colon error
		}
		else if (cursym==numsym){
			err->printerror(0); // devicename must not start with a number error
		}
		else err->printerror(0); // devicename must coincide with a keyword error 
	}
	else if (cursym==sor){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
					}
					else err->printerror(0); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else err->printerror(0); // no colon error
		}
		else if (cursym==numsym){
			err->printerror(0); // devicename must not start with a number error
		}
		else err->printerror(0); // devicename must coincide with a keyword error 
	}
	else if (cursym==snor){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum); 
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
						//parse num into network class
					}
					else err->printerror(0); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else err->printerror(0); // no colon error
		}
		else if (cursym==numsym){
			err->printerror(0); // devicename must not start with a number error
		}
		else err->printerror(0); // devicename must not coincide with a keyword error 
	}
	else if (cursym==sdtype){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum); 
		}
		else if (cursym==numsym){
			err->printerror(0); // devicename must not start with a number error
		}
		else err->printerror(0); // devicename must not coincide with a keyword error
	}
	else if (cursym==sxor){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum); 
		}
		else if (cursym==numsym){
			err->printerror(0); // devicename must not start with a number error
		}
		else err->printerror(0); // devicename must not coincide with a keyword error
	}
	else {
		err->printerror(0); // unidentified type error
	}
}

void parser::connectionlist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
		connection();
	}else err->printerror(10); // no opencurly error
	while (cursym==semicol){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==closecurly) {
			return;
		} 
		connection();
	}
	err->printerror(12); //Error code detected
}

void parser::connection(void){
	if (cursym==namesym){
		// parser will connect this device to the network class here
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==stop){
			smz->getsymbol(cursym,curid,curnum);
			if(cursym==oq){
				// parser will tell the network class this output q
				return;
			}
			else if (cursym==oqbar){
				// parser will tell the network class this output q
				return;
			}
			else err->printerror(1); // invalid output from dtype
		} 
		if (cursym==arrow){
			bool firstit=true;
			while (cursym==comma||firstit){
				smz->getsymbol(cursym,curid,curnum);
				firstit=false;
				if (cursym==namesym){
					smz->getsymbol(cursym,curid,curnum);
					if (cursym==stop){
						smz->getsymbol(cursym,curid,curnum);
						if (cursym==inputsym){
							if (curnum<=16 && curnum>=1){
								// parse num to network class
								smz->getsymbol(cursym,curid,curnum);
							}
							else err->printerror(2); // current number exceeded 16 or is smaller than 1 error
						}
						else if (cursym==idata){
							// parse data input to network class
							smz->getsymbol(cursym,curid,curnum);
						}
						else if (cursym==iclk){
							// parse clk input to network class
							smz->getsymbol(cursym,curid,curnum);
						}
						else if (cursym==iset){
							// parse set input to network class
							smz->getsymbol(cursym,curid,curnum);
						}
						else if (cursym==iclear){
							// parse clear input to network class
							smz->getsymbol(cursym,curid,curnum);
						}
						else err->printerror(3); //input formats is wrong
					}
				}
			}
			//cout << "current sym:" << cursym <<endl;
			if (cursym==semicol){
				return;
			}else err->printerror(4); //comma expected or semicol expected 
		}else err->printerror(5); // for connection, no output or arrow following devicename
	}			
}

void parser::monitorlist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
	}
	else err->printerror(11); // no opencurly error
	while (cursym==namesym){
		parmonitor();
		if (cursym==semicol) {
			smz->getsymbol(cursym,curid,curnum);
		}
		if (cursym ==closecurly){
			return;
		}
	}

	err->printerror(13); // not ended with ; or ,
}

void parser::parmonitor(void){
	if (cursym==namesym){
		// parser to connect this to the network class
		smz->getsymbol(cursym,curid,curnum);
	}
	else err->printerror(0); // a devicename expected error
}



