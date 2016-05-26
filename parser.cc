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
	errorsfound=false;

}

bool parser::readin (void){
	bool allinputsconnected=false; //for checking if all inputs are connected at the end of the function
	smz->getsymbol(cursym,curid,curnum); //get the first symbol
	//code to parse the devices block
	if (cursym==devsym){
        devicelist(); // call devicelist function to parse the devices block
        if (cursym==closecurly){
            smz->getsymbol(cursym,curid,curnum); // if devices block is ended with a }, getsymbol and continuring parsing the next block
        }
        else if (cursym!=consym){
            errorparser(26,consym); // there is an unfinished line, prompt the user to finish this line. Parser will move on to parse the connections block
        }
	}
    else{
        errorparser(0,consym); // Devices block not found, continuing parsing the connections block if there is one
    }
    //code to parse the connections block, exact same logic as above applies here
	if (cursym == consym){
        connectionlist();
        if (cursym==closecurly){
            smz->getsymbol(cursym,curid,curnum);
        }
        else if (cursym!=monsym){
            errorparser(26,monsym);
        }
	}
	else{
        errorparser(1,monsym);
    }
    //code to parse the monitor block, exact same logic as above applies here
    if(cursym == monsym){
        monitorlist();
        if (cursym==closecurly){
            smz->getsymbol(cursym,curid,curnum);
        }
        else if (cursym!=eofsym){
            errorparser(26,eofsym);
        }
	}
	else{
        errorparser(2,eofsym);
    }
	if (cursym==eofsym){
        netz->checknetwork(allinputsconnected);
        if (allinputsconnected==false){
            errorparser(20); //NOT ALL INPUTS ARE CONNECTED TO AN OUTPUT
        }
    }
    else{
        errorparser(27); //Bad SYNTAX ERROR. PLEASE READ MENU.
    }
    if (errorsfound){
        err->printerrornum(); // call function to print number of errors
    }
    return !errorsfound; // return false if errors are found
}

// devicelist() parses the whole device block
void parser::devicelist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
	}
	else{
		errorparser(3,closecurly); // no opencurly error
        return;
	}
	//parses allowed devices here
	while (cursym==sclock || cursym==sswitch || cursym==sdtype || cursym==sxor || cursym==sand || cursym==snand || cursym==sor || cursym==snor){
        device(); // calls device function to parse each device in the devicelist
        // if statement to check if a statement is ended correctly with a semicol
		if (cursym==semicol){
			smz->getsymbol(cursym,curid,curnum);
		}
		else{
            errorparser(23,closecurly); // EXPECTED ';'
            return;
        }
        // checks for the end of the device list
        if (cursym==closecurly){
        return;
        }
	}
	// checks for the case whereby no device is defined in the device block.
    if (cursym==closecurly){
        cout<<"!! Warning: Devices block is blank"<<endl;
    return;
	}
	else{
        errorparser(25); // EXPECTED '}'
        return;
    }
}

// device() will parse a device specification statement and return the semicol behind
void parser::device(void){
    // each if block here checks for a certain device and parses it accordingly
	if (cursym==sclock){
        devtypetemp=dmz->devkind(curid); //check the kind of device and store it in devtypetemp, this will be used for making the device in the devices class later on
		smz->getsymbol(cursym,curid,curnum);
		// this if statement checks if the devicename is already being used
		if (netz->finddevice(curid)==NULL){
            // this if statement checks if the devicename is the right syntax
            if (cursym==namesym){
                devnametemp=curid;
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        //checks if clock period is valid
                        if (curnum>0){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);// Initialising clock and setting its frequency to the integer specified.
                            smz->getsymbol(cursym,curid,curnum);
                            return;
                        }
                        else{
                            errorparser(24,semicol);// CLOCK PERIOD MUST BE > 0
                            return;
                        }
                    }
                    else {
                        errorparser(4,semicol); // num expected error
                        return;
                    }
                }
                else{
                    errorparser(5,semicol);// no colon error
                    return;
                }
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else {
                errorparser(7,semicol); // devicename must coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); // DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else if (cursym==sswitch){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        // check if switch is defined to be on(1) or off(0)
                        if (curnum==0||curnum==1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);//everything correct, make device
                            smz->getsymbol(cursym,curid,curnum);
                            return;
                        }
                        else{
                            errorparser(8,semicol); //input must be zero or one !!!
                            return;
                        }
                    }
                    else{
                        errorparser(4,semicol); // input must be a number
                        return;
                    }
                }
                else{
                    errorparser(5,semicol); // no colon error
                    return;
                }
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else{
                errorparser(7,semicol); // devicename must coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); // DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else if (cursym==sand){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>=1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck); // everything okay, make and gate
                            smz->getsymbol(cursym,curid,curnum);
                            return;
                        }
                        else {
                            errorparser(9,semicol); // number of inputs must be smaller than 17 and greater than 0 error
                            return;
                        }
                    }
                }
                else{
                     errorparser(5,semicol);// no colon error
                     return;
                 }
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else {
                errorparser(7,semicol); // devicename must coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); // DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else if (cursym==snand){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>=1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck); // everything okay, make nand gate
                            smz->getsymbol(cursym,curid,curnum);
                            return;
                        }
                        else {
                            errorparser(9,semicol); // number of inputs must be smaller than 17 and greater than 0 error
                            return;
                        }
                    }
                }
                else{
                    errorparser(5,semicol); // no colon error
                    return;
                }
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else{
                errorparser(7,semicol); // devicename must coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); //DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else if (cursym==sor){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>=1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);// everything okay, make or gate
                            smz->getsymbol(cursym,curid,curnum);
                            return;
                        }
                        else{
                            errorparser(9,semicol); // number of inputs must be smaller than 17 and greater than 0 error
                            return;
                        }
                    }
                }
                else{
                    errorparser(5,semicol); // no colon error
                    return;
                }
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else {
                errorparser(7,semicol); // devicename must coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); // DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else if (cursym==snor){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                devnametemp=curid;
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==colon){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==numsym){
                        if (curnum<=16 && curnum>=1){
                            dmz->makedevice(devtypetemp,devnametemp,curnum,okcheck);// everything okay, make nor gate
                            smz->getsymbol(cursym,curid,curnum);
                            return;
                        }
                        else {
                            errorparser(9,semicol); // number of inputs must be smaller than 17 and greater than 0 error
                            return;
                        }
                    }
                }
                else{
                    errorparser(5,semicol); // no colon error
                    return;
                }
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else {
                errorparser(7,semicol); // devicename must not coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); //DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else if (cursym==sdtype){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                dmz->makedevice(devtypetemp,curid,curnum,okcheck); // everything okay, make dtype
                smz->getsymbol(cursym,curid,curnum);
                return;
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else {
                errorparser(7,semicol); // devicename must not coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); // DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else if (cursym==sxor){
        devtypetemp=dmz->devkind(curid);
		smz->getsymbol(cursym,curid,curnum);
		if (netz->finddevice(curid)==NULL){
            if (cursym==namesym){
                dmz->makedevice(devtypetemp,curid,curnum,okcheck);// everything okay, make xor gate
                smz->getsymbol(cursym,curid,curnum);
                return;
            }
            else if (cursym==numsym){
                errorparser(6,semicol); // devicename must not start with a number error
                return;
            }
            else{
                errorparser(7,semicol); // devicename must not coincide with a keyword error
                return;
            }
		}
		else{
            errorparser(18,semicol); // DEVICENAME IS ALREADY BEING USED
            return;
        }
	}
	else {
		errorparser(19,semicol); // UNIDENTIFIED DEVICE TYPE
		return;
	}
}

// connectionlist() parses the whole connections block
void parser::connectionlist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
	}
	else{
        errorparser(3,closecurly); // no opencurly error
    }
    if (cursym!=namesym&&cursym!=closecurly){
        errorparser(14,semicol); // device not defined
        smz->getsymbol(cursym,curid,curnum);
    }
    //this while loop parses connections until } is encountered
	while (cursym==namesym){
		connection();
		if (cursym==semicol) {
			smz->getsymbol(cursym,curid,curnum);
		}
		else{
            errorparser(23,closecurly); // EXPECTED ';'
        }
        if (cursym==closecurly){
        return;
        }
	}
    if (cursym==closecurly){
        cout<<"!! Warning: Connections block is blank"<<endl;
    return;
	}
	else{
        errorparser(25); // EXPECTED '}'
        return;
    }
}

// connection() parses each connection specified by the user
void parser::connection(void){
	if (cursym==namesym){
        //check if device has been defined earlier on
		if (netz->finddevice(curid)!=NULL){
			devnametemp=curid;// parser will connect this device to the network class here
			outputnametemp=blankname;
			// check if device is a dtype because parsing logic for a dtype is different from the rest
			if (netz->finddevice(curid)->kind==7){
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==stop){
                    smz->getsymbol(cursym,curid,curnum);
                    if(cursym==oq){
                        outputnametemp=curid; //stores the output type for connection later
                        smz->getsymbol(cursym,curid,curnum);
                    }
                    else if (cursym==oqbar){
                        outputnametemp=curid; //stores the output type for connection later
                        smz->getsymbol(cursym,curid,curnum);
                    }
                    else{
                        errorparser(11,semicol); // invalid output from dtype
                        return;
                    }
                }
                else{
                    errorparser(21,semicol); // EXPECTED '.'. NEED TO SPECIFY OUTPUT FOR DTYPE
                    return;
                }
			}
			else{
                smz->getsymbol(cursym,curid,curnum);
            }
			if (cursym==arrow){
                // this boolean helps kick off the while loop
				bool firstit=true;
				while (cursym==comma||firstit){
					firstit=false;
					smz->getsymbol(cursym,curid,curnum);
					if (cursym==namesym){
						if (netz->finddevice(curid)!=NULL){
                            // checks if device is a dtype as it has different outputs from other devices, the if block below is for a non-dtype device
							if (netz->finddevice(curid)->kind!=7){
								int noofinputs=0;
								noofinputs=netz->getnumberofinputs(curid);
								devnamtempinp=curid;
								smz->getsymbol(cursym,curid,curnum);
								if (cursym==stop){
									smz->getsymbol(cursym,curid,curnum);
									if (cursym==inputsym){
                                        if (curnum<=noofinputs && curnum>=1){
                                            // check if the input has already being connected
                                            if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                                // if errors are found before this, abandon effort to establish new connections
                                                if (!errorsfound){
                                                    netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck); // everything ok, establish connection
                                                }
                                                smz->getsymbol(cursym,curid,curnum);
                                            }
                                            else{
                                                errorparser(17,semicol);
                                                return;
                                                }// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                            }
                                        else{
                                            errorparser(15,semicol); // current number exceeded 16 or is smaller than 1 error
                                            return;
                                        }
									}
									else{
                                        errorparser(16,semicol); //INVALID INPUT
                                        return;
                                    }
								}
								else{
                                    errorparser(13,semicol); //EXPECTED '.'
                                    return;
                                }
							}
							else{
								devnamtempinp=curid;
								smz->getsymbol(cursym,curid,curnum);
								if (cursym==stop){
									smz->getsymbol(cursym,curid,curnum);
                                    if (cursym==idata){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            if (!errorsfound){
                                                netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse data input to network class
                                            }
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17,semicol);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                            return;
                                        }
                                    }
                                    else if (cursym==iclk){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            if (!errorsfound){
                                                netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse clk input to network class
                                            }
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17,semicol);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                            return;
                                        }
                                    }
                                    else if (cursym==iset){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            if (!errorsfound){
                                                netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse set input to network class
                                            }
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17,semicol);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                            return;
                                        }
                                    }
                                    else if (cursym==iclear){
                                        if (netz->findinput(netz->finddevice(devnamtempinp),curid)->connect==0){
                                            if (!errorsfound){
                                                netz->makeconnection(devnamtempinp,curid,devnametemp,outputnametemp,okcheck);// parse clear input to network class
                                            }
                                            smz->getsymbol(cursym,curid,curnum);
                                        }
                                        else{
                                            errorparser(17,semicol);// PREEXISTING CONNECTION FOUND. CANNOT CONNECT 2 OUTPUTS TO THE SAME INPUT
                                            return;
                                        }
                                    }else{
                                        errorparser(16,semicol);// INVALID INPUT
                                        return;
                                    }
								}else{
                                    errorparser(13,semicol);//  expected '.'
                                    return;
								}
							}
                        }
                        else{
                            errorparser(14,semicol); // DEVICE NOT DEFINED
                            return;
                        }
                    }
					else{
                        errorparser(14,semicol); // DEVICE NOT DEFINED
                        return;
                    }
				}
				if (cursym==semicol){
                    return;
				}else{
                    errorparser(23,semicol);// EXPECTED ';'
                    return;
				}
			}else{
                errorparser(22,semicol);//EXPECTED '->'
                return;
			}
		}
		else{
            errorparser(14,semicol); // DEVICE NOT DEFINED
            return;
        }
	}
	else {
        errorparser(14,semicol); //DEVICE NOT DEFINED
        return;
    }
}

// monitorlist() parses the whole monitor block
void parser::monitorlist(void){
	smz->getsymbol(cursym,curid,curnum);
	if (cursym==opencurly){
		smz->getsymbol(cursym,curid,curnum);
	}
	else errorparser(3); // no opencurly error
    if (cursym!=namesym&&cursym!=closecurly){
        errorparser(14,semicol);
        smz->getsymbol(cursym,curid,curnum);
    }
	// this while loop continues parsing the monitors until a } is found
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
        cout<<"!! Warning: Monitor block is blank"<<endl;
        return;
	}
	else{
        errorparser(25); // EXPECTED '}'
    }
}

// parmonitor parses each output to monitor
void parser::parmonitor(void){
	if (cursym==namesym){
		if (netz->finddevice(curid)!=NULL){
            devnametemp=curid;
            outputnametemp=blankname;
            // checks if device is a dtype, if it is, the output (q or qbar) must be specified
            if (netz->finddevice(curid)->kind==7){
                smz->getsymbol(cursym,curid,curnum);
                if (cursym==stop){
                    smz->getsymbol(cursym,curid,curnum);
                    if (cursym==oq){
                        outputnametemp=curid;
                        mmz->makemonitor(devnametemp,outputnametemp,okcheck);// everything okay, make monitor
                        smz->getsymbol(cursym,curid,curnum);
                    }
                    else if (cursym==oqbar){
                        outputnametemp=curid;
                        mmz->makemonitor(devnametemp,outputnametemp,okcheck);// everything okay, make monitor
                        smz->getsymbol(cursym,curid,curnum);
                    }
                    else{
                        errorparser(11,semicol); // INVALID OUTPUT FROM D-TYPE
                        return;
                    }
                }
                else{
                    errorparser(21,semicol); //Expected ., need to specify DTYPE output.
                    return;
                }
            }
            else{
                mmz->makemonitor(devnametemp,outputnametemp,okcheck); //just make monitor if not a dtype because there can be no ambiguity to the output
                smz->getsymbol(cursym,curid,curnum);
            }
            if (cursym==semicol){
                return;
            }
            else{
                errorparser(23,semicol);
                return;
            }
        }
        else{
            errorparser(14,semicol);
            return;
        }
    }
    else{
		errorparser(14,semicol); // a devicename expected error
		return;
	}
}

// the errorparser function specifies a stop symbol after which parsing can begin again. It also outputs an error message
void parser::errorparser(int errin, symbol stop){
    errorsfound=true;
	err->printerror(errin);
	if(stop!=badsym){
        while((cursym!=stop)&&(cursym!=eofsym)){
            smz->getsymbol(cursym,curid,curnum);
        }
	}
}
