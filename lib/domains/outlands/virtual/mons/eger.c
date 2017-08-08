#include <std.h>

inherit MONSTER;

void create() {
  ::create();
  set_name("eger");
  set_id( ({ "eger","mezo_area_monster" }));
  set_short( "Eger");
  if(random(2)) set_gender("ferfi");
  else set_gender("no");
  set_long( "Egy mezei eger, riadtan tekint korbe.");
  set_alignment(-1);
  set_race( "eger");
  set_level(1);   
}
