#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
#include "names.h"
#include "scanner.h"

using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{
	Namestore = names_mod;
	
	inf.open(defname);	//Open file
	if (!inf)
	{
		cout << "Error: file cannot be opened" << endl;
	}
	eofile = !(inf.get(curch));	//Get first character
}

scanner::~scanner(){
	cout << "scanner has been destroyed" <<endl;
}
void scanner::getsymbol(symbol& s, name& id, int& num)
{
	start:
	skipspaces(&inf, curch, eofile);
	if (eofile){
		s=eofsym;
		inf.close();
	}
	else{
		if(isdigit(curch)){
			s=numsym;
			num=getnumber(&inf, curch, eofile);	
		}
		else{
			if(isalpha(curch)){
				id=getname(&inf, curch, eofile);
				if (id==1) s=devsym; else
				if(id==2) s=consym; else
				if (id==3) s=monsym; else 
				s=namesym; //not a keyword
			}
			else{
				//This operation is invalid because a switch can only operate on an integer. Need rewrite. 
				//Slap all these into names table????
				punct=getpunct(&inf,curch, eofile);
				switch(Namestore->cvtname(punct)){
					case 34: s = colon; break;
					case 35: s = stop;break;
					case 36: s = comma; break;
					case 0: s=semicol; break;
					case 37: s=opencurly; break;
					case 38: s=closecurly; break;
					case 39: s=arrow; break;
					case 40: skipcomment(&inf,curch,eofile);goto start;break;
					default: s=badsym; break;
				}
			}
			}
	}
}

name scanner::getname(ifstream *infp, char &curch, bool &eofile){
	bool checkflag=false;
	namestring str="";
	
	while (!eofile&&(isdigit(curch)||isalpha(curch)||curch=='_')) { //Continues as long as curch is _, or an alphabet / number.
		str+=curch;
		eofile = !(infp->get(curch));
	}
	return Namestore->lookup(str);	
}

int scanner::getnumber(ifstream *infp, char &curch, bool &eofile){
	int placeholder = 0;
	int number=0;
	while (!eofile) {
		if (!isdigit(curch)){
			return number;
		}
		placeholder = curch - '0';
		number = number*10+placeholder;
		eofile = !(infp->get(curch));
	}	
	return number;
}

void scanner::skipspaces(ifstream *infp,char &curch,bool &eofile){
  while (!eofile) {
    if (!isspace(curch)){
		return;
	}
	eofile = !(infp->get(curch));
  }	
  return;
}

string scanner::getpunct(ifstream *infp,char &curch,bool &eofile){
	string punct="";
	//Maybe I should return once the punctuation string is longer than 2 symbols. Additionally, maybe some restructuring has to be done to eliminate the problem of the goto. As Andrew says, maybe add to the start of the loops???/
	while(!eofile){
		if(isdigit(curch)||isalpha(curch)||isspace(curch)){
			return punct;
		}
		punct+=curch;
		eofile = !(infp->get(curch));
	}
	return punct;
}
void scanner::skipcomment(ifstream *infp,char &curch,bool &eofile){
	char prev='.';
	char cur='.';
	while(!eofile){
		prev=cur;
		cur=curch;
		eofile = !(infp->get(curch));
		if (prev=='*' && cur=='/'){
			return;
		}	
	}
	return;
}
