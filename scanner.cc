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

}
void scanner::getsymbol(symbol& s, name& id, int& num)
{
	skipspaces(&inf, curch, eofile); //Skips to next non-whitespace character.
	lastparsedsym=inf.tellg();
	if (eofile){ //Exits with eofsymbol if eof found.
		s=eofsym;
		inf.close();
	}
	else{
		if(isdigit(curch)){
			s=numsym;
			num=getnumber(&inf, curch, eofile); //If character is a digit, run the character string.
		}
		else{
			if(isalpha(curch)){
				id=getname(&inf, curch, eofile); //If alphabet is detected, it is a name. Lookup the nameid of the name discovered, and return the right symbol to parser.
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
					num=stoi((Namestore->getnamefromtable(id)).substr(1,2)); //If it is an input type of I1-I16, return symbol inputsym and the gate number in num.
				} else
				if(id==28) s=idata; else
				if(id==29) s=iclk; else
				if(id==30) s=iset; else
				if(id==31) s=iclear; else
				if(id==32) s=oq; else
				if(id==33) s=oqbar; else
				if(id==42) s=siggensym; else
				s=namesym; //not a keyword
			}
			else{ //if it is not a alphabet or integer or whitespace, it is a punctuation.
				punct=getpunct(&inf,curch, eofile); //Get the punctuation string.
				switch(Namestore->cvtname(punct)){ //Return the punctuation as appropriate.
					case 34: s = colon; break;
					case 35: s = stop;break;
					case 36: s = comma; break;
					case 0: s=semicol; break;
					case 37: s=opencurly; break;
					case 38: s=closecurly; break;
					case 39: s=arrow; break;
					case 40: skipcomment(&inf,curch,eofile);getsymbol(s,id,num);break; //If a /* is found, skip all the way till the */ and run getsymbol again to return the symbol after comment
					case 41: skipcommentline(&inf,curch,eofile);getsymbol(s,id,num);break; //If a // is found, skip all the way till line ends and run getsymbol again to return the symbol after comment.
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
		placeholder = curch - '0'; //Evil Hack converts the character into a single integer
		number = number*10+placeholder; // Constructs the final integer from a sequence of characters
		eofile = !(infp->get(curch));
	}
	return number;
}

void scanner::skipspaces(ifstream *infp,char &curch,bool &eofile){
  while (!eofile) {
	if(curch=='\n'){
        lastparsedline=(infp->tellg());//Ideally this should give the location of the line right before the error
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
	while(!eofile){ //Gets a punctuation string by iterating through file and returning when a digit, alphanumeric or space is returned.
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
		if (prev=='*' && cur=='/'){ // Returns if a * and a / are detected next to each other.
			return;
		}
	}
	cout << "WARNING:: UNCLOSED COMMENT BLOCK DETECTED!" <<endl;
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


void scanner::linedisplayerror(int &linenum,string &line1, string &line2){
    linenum=linenumber;
    line1="";
    line2="";
	int currentposition = inf.tellg();
	bool eofile2=false;
	char backup;
	inf.seekg(lastparsedline,inf.beg);
	while (!eofile2) //Keeps adding characters into the line to be displayed until a newline character or endfile found.
		{
            eofile2=!(inf.get(backup));
            if (backup=='\n') break;
			line1+=backup;
		}
    for(int i=1;i<(lastparsedsym-lastparsedline);i++){ //Counter increments starting from i=1 because the carrot character has a width as well
        line2+=" ";
    }
    line2+="^";
	inf.seekg (currentposition, inf.beg);

}
