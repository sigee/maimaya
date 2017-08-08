#include <std.h>

inherit MONSTER;

void create() {
  ::create();
  set_name("nyul");
  set_id( ({ "nyul","mezo_area_monster" }));
  set_short( "Nyul");
  if(random(2)) set_gender("ferfi");
  else set_gender("no");
  set_long( "Egy szurke nyul, epp egy kis elelmet ragcsal a fogai kozt.");
  set_alignment(-1);
  set_race( "nyul");
  set_level(2);   
}
