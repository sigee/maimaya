#include <std.h>
#include "./virtual.h";

inherit ROOM;

string MoreLong;

void add_more_long(string l);
string query_more_long();

void add_more_long(string what){
  if (!MoreLong) {
    MoreLong = what;
  } else {
    MoreLong += what;
  }
}

string query_more_long() { return MoreLong; }

string query_long() {
  string bef, tmp;

  bef = ::query_long();
  if (!bef) {
    bef = "";
  }
  tmp = query_more_long();
  if (tmp) {
    return bef + tmp;
  }
  return bef;
}

void reset(){
/*  string *mons;
  ::reset();
  if(!present(this_object()->query_property("outlands")+"_area_monster"))
   if(random(8)==1){
    mons = MONS[this_object()->query_property("outlands")];
    new(OUTLANDS_DIR+"mons/"+mons[random(sizeof(mons))] )->move(this_object());
  }
*/
}
