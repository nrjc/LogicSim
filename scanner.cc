#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
char curch;
void scanner::getsymbol(symbol &s, name &id, int &num)
{
	skipspaces();
	if (eofile){
		s=eofsym;
	}
	else{
		if(isdigit(curch)){
			s=numsym;
			getnumber(num);
		}
		else{
			if(isalpha(curch)){
				getname(id);
				if (id==devicename) s=devsym; else
				if(id==connectname) s=consym; else
				//
				//
				s=namesym; //not a keyword
			}
			else{
				switch(curch){
					case '=': s = equals; break;
					case ';': s=semicol;break;
					case ',': s = comma; break;
					default: s=badsym; break;
				}
				getch();
			}
			}
	}
}
