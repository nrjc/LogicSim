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
	} else {errorparser(0);return false;}
	if (cursym == consym){
			connectionlist();
			smz->getsymbol(cursym,curid,curnum);
	} else {errorparser(1);return false;}
   if (cursym == monsym){
			monitorlist();
			smz->getsymbol(cursym,curid,curnum);
	} else {errorparser(2);return false;}
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
		errorparser(3); // no opencurly error
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
	 devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
            devnametemp=curid;
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
                    dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);// Initialising clock and setting its frequency to the integer specified.
					return;
				}
				else errorparser(4); // num expected error
			}
			else errorparser(5); // no colon error
		}
		else if (cursym==numsym){
			errorparser(6); // devicename must not start with a number error
		}
		else errorparser(7); // devicename must coincide with a keyword error
	}
	else if (cursym==sswitch){
	devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
            devnametemp=curid;//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum==0||curnum==1){
                        dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);//parse 0/1 into device class
						return;
					}
					else{
						errorparser(8); //input must be zero or one !!!
					}
				}
				else{
					errorparser(4); // input must be a number
				}
			}
			else{
				errorparser(5); // no colon error
			}
		}
		else if (cursym==numsym){
			errorparser(6); // devicename must not start with a number error
		}
		else{
			errorparser(7); // devicename must coincide with a keyword error
		}
	}
	else if (cursym==sand){
	devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
            devnametemp=curid;//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
                        dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
						return;
					}
					else errorparser(0); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else errorparser(0); // no colon error
		}
		else if (cursym==numsym){
			errorparser(0); // devicename must not start with a number error
		}
		else errorparser(0); // devicename must coincide with a keyword error
	}
	else if (cursym==snand){
	devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
            devnametemp=curid;//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
                        dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
						//parse num into network class
						return;
					}
					else errorparser(9); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else errorparser(5); // no colon error
		}
		else if (cursym==numsym){
			errorparser(6); // devicename must not start with a number error
		}
		else errorparser(7); // devicename must coincide with a keyword error
	}
	else if (cursym==sor){
	devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
		devnametemp=curid;
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
                        dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
						//parse num into network class
						return;
					}
					else errorparser(9); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else errorparser(5); // no colon error
		}
		else if (cursym==numsym){
			errorparser(6); // devicename must not start with a number error
		}
		else errorparser(7); // devicename must coincide with a keyword error
	}
	else if (cursym==snor){
	devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
		devnametemp=curid;
			//this is the place where you get the user defined name and parse it into the network class
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==colon){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==numsym){
					if (curnum<=16 && curnum>=1){
                        dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
						//parse num into network class#
						return;
					}
					else errorparser(9); // number of inputs must be smaller than 17 and greater than 0 error
				}
			}
			else errorparser(5); // no colon error
		}
		else if (cursym==numsym){
			errorparser(6); // devicename must not start with a number error
		}
		else errorparser(7); // devicename must not coincide with a keyword error
	}
	else if (cursym==sdtype){
	devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
            dmz->makedevice(devtypetemp,curid,curnum,okcheck);
			//this is the place where you get the user defined name and parse it into the network class
			return;
		}
		else if (cursym==numsym){
			errorparser(6); // devicename must not start with a number error
		}
		else errorparser(7); // devicename must not coincide with a keyword error
	}
	else if (cursym==sxor){
	devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==namesym){
            dmz->makedevice(devtypetemp,curid,curnum,okcheck);
			//this is the place where you get the user defined name and parse it into the network class
			return;
		}
		else if (cursym==numsym){
			errorparser(6); // devicename must not start with a number error
		}
		else errorparser(7); // devicename must not coincide with a keyword error
	}
	else {
		errorparser(-1); // unidentified type error
	}
}

void parser::connectionlist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
		connection();
	}else errorparser(3); // no opencurly error
	while (cursym==semicol){
		smz->getsymbol(cursym,curid,curnum);
		if (cursym==closecurly) {
			return;
		}
		connection();
	}
	errorparser(-1); //Error code detected
}

void parser::connection(void){
	if (cursym==namesym){
		if (netz->finddevice(curid)!=NULL){
			devnametemp=curid;// parser will connect this device to the network class here
			outputnametemp=blankname;
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==stop){
				smz->getsymbol(cursym,curid,curnum);
				if(cursym==oq){
					outputnametemp=curid;// parser will tell the network class this output q
					smz->getsymbol(cursym,curid,curnum);
				}
				else if (cursym==oqbar){
					outputnametemp=curid;// parser will tell the network class this output q
					smz->getsymbol(cursym,curid,curnum);
				}
				else errorparser(11); // invalid output from dtype
			}
			if (cursym==arrow){
				bool firstit=true;
				while (cursym==comma||firstit){
					firstit=false;
					smz->getsymbol(cursym,curid,curnum);
					if (cursym==namesym){
						if (netz->finddevice(curid)!=NULL){
							//devicepointer=netz->finddevice(curid);
							//kindindex=devicepointer->kind;
							int kindindex=0;
							kindindex=(netz->finddevice(curid))->kind;
							if (kindindex!=7){
							//cout<<"shit"<<curnum<<endl;
							//cout<<"shit :"<<netz->getnumberofinputs(curid)<<endl;
								int noofinputs=0;
								noofinputs=netz->getnumberofinputs(curid);
								devnamtempinp=curid;
								smz->getsymbol(cursym,curid,curnum);
								if (cursym==stop){
									smz->getsymbol(cursym,curid,curnum);
									if (cursym==inputsym){
										if (curnum<=noofinputs && curnum>=1){
											netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);
											// parse num to network class
											smz->getsymbol(cursym,curid,curnum);
										}
										else errorparser(15); // current number exceeded 16 or is smaller than 1 error
									}
								}
							}
							else{
								devnamtempinp=curid;
								smz->getsymbol(cursym,curid,curnum);
								if (cursym==stop){
									smz->getsymbol(cursym,curid,curnum);
									if (cursym==idata){
										netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse data input to network class
										smz->getsymbol(cursym,curid,curnum);
									}
									else if (cursym==iclk){
										netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse clk input to network class
										smz->getsymbol(cursym,curid,curnum);
									}
									else if (cursym==iset){
										netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse set input to network class
										smz->getsymbol(cursym,curid,curnum);
									}
									else if (cursym==iclear){
										netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse clear input to network class
										smz->getsymbol(cursym,curid,curnum);
									}
									else errorparser(16); //input formats is wrong
								}
							}
						}else errorparser(14); // DEVICE NOT DEFINED
					}else errorparser(6);
				}
				if (cursym==semicol){
					return;
				}else errorparser(-1); //comma expected or semicol expected
			}else errorparser(-1); // for connection, no output or arrow following devicename
		}else errorparser(14); // DEVICE NOT DEFINED
	}else errorparser(-1); 
}

void parser::monitorlist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
	}
	else errorparser(3); // no opencurly error
	while (cursym==namesym){
		parmonitor();
		if (cursym==semicol) {
			smz->getsymbol(cursym,curid,curnum);
		} else {errorparser(-1); } //Multiple device names
		if (cursym==closecurly){
			return;
		}
	}
	if (cursym==closecurly){
			//Should throw a warning. monitor class is blank
			return;
	}
	errorparser(-1); // not ended with ; or ,
}

void parser::parmonitor(void){
	if (cursym==namesym){
		if (netz->finddevice(curid)!=NULL){
			devnametemp=curid;
			outputnametemp=blankname;
			smz->getsymbol(cursym,curid,curnum);
			if (cursym==stop){
				smz->getsymbol(cursym,curid,curnum);
				if (cursym==oq){
					outputnametemp=curid;
					// monitor output q
					mmz->makemonitor(devnametemp,outputnametemp,okcheck);
					smz->getsymbol(cursym,curid,curnum);
					return;
				}
				else if (cursym==oqbar){
					outputnametemp=curid;
					mmz->makemonitor(devnametemp,outputnametemp,okcheck);
					// monitor output qbar
					smz->getsymbol(cursym,curid,curnum);
					return;
				}
				else{
					errorparser(11); // INVALID OUTPUT FROM D-TYPE
					return;
				}
			}
			else if (cursym==semicol){
				mmz->makemonitor(devnametemp,outputnametemp,okcheck);
				return;
			}
			else{
				errorparser(13); // EXPECTED '.' or '.'
				return;
			}
		}
		else{
			cursym=badsym;
			errorparser(14);
			return;
		}
	}
	else{
		errorparser(12); // a devicename expected error
	}
}

void parser::errorparser(int errin){
	cout << "line ";
	smz->getnewline();
	cout <<": " ;
	err->printerror(errin);
	cout << endl;
}
