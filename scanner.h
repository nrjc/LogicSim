typedef enum{namesym, numsym, devsym, consym, monsym, endsym, colon, stop, comma, semicol, opencurly, closecurly, arrow, badsym, eofsym} symbol;

//this is the constructor of the scanner class
scanner(names* names_mod, const char* defname);
~scanner();

void getsymbol(symbol& s, name& id, int& num);
