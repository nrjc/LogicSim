typedef enum{namesym, numsym, devsym, consym, monsym, endsym, comma, semicol, equals, badsym, eofsym} symbol;

//this is the constructor of the scanner class
scanner(names* names_mod, const char* defname);
~scanner();

void getsymbol(symbol& s, name& id, int& num);
