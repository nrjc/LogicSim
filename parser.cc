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
	bool allinputsconnected=false;
	smz->getsymbol(cursym,curid,curnum); //get the first symbol
	if (cursym==devsym){
        devicelist();
        smz->getsymbol(cursym,curid,curnum);
	}
    else{
        errorparser(0);
        return false;
    }
	if (cursym == consym){
        connectionlist();
        smz->getsymbol(cursym,curid,curnum);
	}
	else{
        errorparser(1);
        return false;
    }
    if(cursym == monsym){
        monitorlist();
        smz->getsymbol(cursym,curid,curnum);
	}
	else{
        errorparser(2);
        return false;
    }
	if (cursym==eofsym){
        netz->checknetwork(allinputsconnected);
        if (allinputsconnected==true){
            return true;
        }
        else{
            errorparser(20); //NOT ALL INPUTS ARE CONNECTED TO AN OUTPUT
            return false;
        }
    }
    else{
        return false;
    }
}

void parser::devicelist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
	}
	else{
		errorparser(3); // no opencurly error
	}
	while (cursym==sclock || cursym==sswitch || cursym==sdtype || cursym==sxor || cursym==sand || cursym==snand || cursym==sor || cursym==snor){
        device();
		if (cursym==semicol){
			smz->getsymbol(cursym,curid,curnum);
		}
		else{
            errorparser(23); // EXPECTED ';'
        }
        if (cursym==closecurly){
        return;
        }
	}
    if (cursym==closecurly){
    //Should throw a warning. device class is blank
    return;
	}
	else{
        errorparser(25); // EXPECTED '}'
    }
}

// device() will parse a device specification statement and return the semicol behind

void parser::device(void){
	if (cursym==sclock){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;
                //this is the place where you get the user defined name and parse it into the network class
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum>0){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);// Initialising clock and setting its frequency to the integer specified.
                            smz->getsymbol(cursym,curid,curnum);
                            return;
                        }
                        else{
                            errorparser(24);// CLOCK PERIOD MUST BE > 0
                        }
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
		else{
            errorparser(18); // DEVICENAME IS ALREADY BEING USED
        }
	}
	else if (cursym==sswitch){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;//this is the place where you get the user defined name and parse it into the network class
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum==0||curnum==1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);//parse 0/1 into device class
                            smz->getsymbol(cursym,curid,curnum);
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
		else{
            errorparser(18); // DEVICENAME IS ALREADY BEING USED
        }
	}
	else if (cursym==sand){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;//this is the place where you get the user defined name and parse it into the network class
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
                            smz->getsymbol(cursym,curid,curnum);
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
		else{
            errorparser(18); // DEVICENAME IS ALREADY BEING USED
        }
	}
	else if (cursym==snand){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;//this is the place where you get the user defined name and parse it into the network class
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
                            smz->getsymbol(cursym,curid,curnum);
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
		else{
            errorparser(18); //DEVICENAME IS ALREADY BEING USED
        }
	}
	else if (cursym==sor){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
            devnametemp=curid;
                //this is the place where you get the user defined name and parse it into the network class
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
                            smz->getsymbol(cursym,curid,curnum);
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
		else{
            errorparser(18); // DEVICENAME IS ALREADY BEING USED
        }
	}
	else if (cursym==snor){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
            devnametemp=curid;
                //this is the place where you get the user defined name and parse it into the network class
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);
                            smz->getsymbol(cursym,curid,curnum);
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
		else{
            errorparser(18); //DEVICENAME IS ALREADY BEING USED
        }
	}
	else if (cursym==sdtype){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                dmz->makedevice(devtypetemp,curid,curnum,okcheck);
                smz->getsymbol(cursym,curid,curnum);
                //this is the place where you get the user defined name and parse it into the network class
                return;
            }
            else if (cursym==numsym){
                errorparser(6); // devicename must not start with a number error
            }
            else errorparser(7); // devicename must not coincide with a keyword error
		}
		else{
            errorparser(18); // DEVICENAME IS ALREADY BEING USED
        }
	}
	else if (cursym==sxor){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                dmz->makedevice(devtypetemp,curid,curnum,okcheck);
                smz->getsymbol(cursym,curid,curnum);
                //this is the place where you get the user defined name and parse it into the network class
                return;
            }
            else if (cursym==numsym){
                errorparser(6); // devicename must not start with a number error
            }
            else errorparser(7); // devicename must not coincide with a keyword error
		}
		else{
            errorparser(18); // DEVICENAME IS ALREADY BEING USED
        }
	}
	else {
		errorparser(19); // UNIDENTIFIED DEVICE TYPE
	}
}

void parser::connectionlist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
	}else errorparser(3); // no opencurly error
	while (cursym==namesym){
		connection();
		if (cursym==semicol) {
			smz->getsymbol(cursym,curid,curnum);
		}
		else{
            errorparser(23); // EXPECTED ';'
        }
        if (cursym==closecurly){
        return;
        }
	}
    if (cursym==closecurly){
    //Should throw a warning. monitor class is blank
    return;
	}
	else{
        errorparser(25); // EXPECTED '}'
    }
}

void parser::connection(void){
	if (cursym==namesym){
		if (netz->finddevice(curid)!=NULL){
			devnametemp=curid;// parser will connect this device to the network class here
			outputnametemp=blankname;
			if (netz->finddevice(curid)->kind==7){ // check if device is a dtype
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
                else{
                    errorparser(21); // EXPECTED '.'. NEED TO SPECIFY OUTPUT FOR DTYPE
                }
			}
			else{
                smz->getsymbol(cursym,curid,curnum);
            }
			if (cursym==arrow){
				bool firstit=true;
				while (cursym==comma||firstit){
					firstit=false;
					smz->getsymbol(cursym,curid,curnum);
					if (cursym==namesym){
						if (netz->finddevice(curid)!=NULL){
							if (netz->finddevice(curid)->kind!=7){
								int noofinputs=0;
								noofinputs=netz->getnumberofinputs(curid);
								devnamtempinp=curid;
								smz->getsymbol(cursym,curid,curnum);
								if (cursym==stop){
									smz->getsymbol(cursym,curid,curnum);
									if (cursym==inputsym){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            if (curnum<=noofinputs && curnum>=1){
                                                netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);
                                                smz->getsymbol(cursym,curid,curnum);
                                            }
                                            else errorparser(15); // current number exceeded 16 or is smaller than 1 error
                                            }
                                        else{
                                            errorparser(17); // PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                        }
									}
									else{
                                        errorparser(16); //INVALID INPUT
                                    }
								}
							}
							else{
								devnamtempinp=curid;
								smz->getsymbol(cursym,curid,curnum);
								if (cursym==stop){
									smz->getsymbol(cursym,curid,curnum);
									//cout<<"POINTER: "<<netz->finddevice(curid)->kind<<endl;
									//cout<<"CURSYM: "<<cursym<<endl;
									//cout<<"PREVIOUS CONNECTION: "<<netz->findinput(netz->finddevice(devnamtempinp),curid)->connect<<endl;
                                    //cout<<"POINTER: "<<netz->findinput(netz->finddevice(devnamtempinp),curid)->connect<<endl;

                                    if (cursym==idata){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse data input to network class
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                        }
                                    }
                                    else if (cursym==iclk){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse clk input to network class
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                        }
                                    }
                                    else if (cursym==iset){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse set input to network class
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                        }
                                    }
                                    else if (cursym==iclear){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse clear input to network class
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                        }
                                    }
                                    else errorparser(16); // INVALID INPUT
								}
								else{
                                    errorparser(13);//  expected '.'
                                }
							}
						}else errorparser(14); // DEVICE NOT DEFINED
					}
					else if (cursym==numsym){
                        errorparser(6); // DEVICENAME CANNOT START WITH A NUMBER
					}
					else{
                        errorparser(7); // DEVICENAME MUST NOT CONINCIDE WITH A KEYWORD
                    }
				}
				if (cursym==semicol){
					return;
				}else errorparser(23); //EXPECTED ';'
			}else errorparser(22); // EXPECTED '->'
		}else errorparser(14); // DEVICE NOT DEFINED
	}
	else if (cursym==numsym){
        errorparser(6); //DEVICENAME CANNOT START WITH A NUMBER
	}
	else {
        errorparser(7); //DEVICENAME
    }
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
		}
		else{
            errorparser(23); // EXPECTED ';'
        }
        if (cursym==closecurly){
        return;
        }
	}
	if (cursym==closecurly){
        //Should throw a warning. monitor class is blank
        return;
	}
	else{
        errorparser(25); // EXPECTED '}'
    }
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
	err->printerror(errin);
}
