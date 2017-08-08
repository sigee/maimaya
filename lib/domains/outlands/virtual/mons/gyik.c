#include <std.h>

inherit MONSTER;

void create(){
  ::create();
  set_name("gyik");
  set_id( ({ "gyik","mezo_area_monster" }));
  set_short("Gyik");
  if(random(2)) set_gender("ferfi");
  else set_gender("no");
  set_long("Egy mezei gyik, riadtan tekint korbe.");
  set_alignment(-1);
  set_race("gyik");
  set_level(1);
}
