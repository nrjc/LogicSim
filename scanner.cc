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
	linenumber=1;
	inputname=defname;
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
				if (id==4) s=sclock; else
				if (id==5) s=sswitch; else
				if(id==6) s=sdtype; else
				if(id==7) s=sxor; else
				if(id==8) s=sand; else
				if(id==9) s=snand; else
				if(id==10) s=sor; else
				if(id==11) s=snor; else
				if(id>=12&&id<=27)
				{
					s=inputsym;
					num=stoi((Namestore->getnamefromtable(id)).substr(1,2));
				} else 
				if(id==28) s=idata; else
				if(id==29) s=iclk; else
				if(id==30) s=iset; else
				if(id==31) s=iclear; else
				if(id==32) s=oq; else
				if(id==33) s=oqbar; else
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
					case 40: skipcomment(&inf,curch,eofile);getsymbol(s,id,num);break;
					case 41: skipcommentline(&inf,curch,eofile);getsymbol(s,id,num);break;
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
	if(curch=='\n'){
		linenumber+=1;
	}
    if (!isspace(curch)){
		return;
	}
	eofile = !(infp->get(curch));
  }	
}

string scanner::getpunct(ifstream *infp,char &curch,bool &eofile){
	string punct="";
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

void scanner::skipcommentline(ifstream *infp,char &curch,bool &eofile){
	while(!eofile){
		if (curch=='\n'){
			return;
		}
		eofile = !(infp->get(curch));
	}
	return;
}
void scanner::getnewline(){
	//The job of this function is to print out the rest of the line from when the error was spotted.
	string line="";
	int currentposition = inf.tellg();
	char backup = curch;
	bool eofile2=false;
	while (!eofile2 && backup != '\n')
		{
			line+=backup;
			eofile2=!(inf.get(backup));
		}
	cout<<linenumber<<"  " << line;
	inf.seekg (currentposition, inf.beg);
	
}
