#ifndef names_h
#define names_h

#include <string>
#include <boost/bimap.hpp>

using namespace std;

//const int maxnames  = 200;  /* max number of distinct names */
const int maxlength = 20;    /* max chars in a name string   */
const int blankname = -1;   /* special name                 */

typedef int name;
typedef string namestring;

class names{
	name counter;
	boost::bimap<namestring, name> map;


public:
  name lookup (namestring str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not already in the name table, it is          */
    /* automatically inserted.                                             */

  name cvtname (namestring str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not in the name table then 'blankname' is     */
    /* returned.                                                           */

  void writename (name id);
    /* Prints out the given name on the console                            */

  int namelength (name id);
    /* Returns length ie number of characters in given name                */

  namestring getnamefromtable(name id);
  /* Returns the namestring for a given name. */

  void dumptablecontents();
  /* dumps the entire contents of the table for debugging purposes */

  names (void);
    /* names initialises the name table.  This procedure is called at      */
    /* system initialisation before any of the above procedures/functions  */
    /* are used.                                                           */
};

#endif /* names_h */
