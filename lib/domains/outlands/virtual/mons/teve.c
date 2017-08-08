#include <std.h>

inherit MONSTER;

void create() {
  ::create();
  set_name("teve");
  set_id( ({ "teve","sivatag_area_monster" }));
  set_short("Teve");
  set_gender("ferfi");
  set_long("Egy ketpupu teve, a sivatag hajoja.");
  set_alignment(-1);
  set_race("teve");
  set_level(8+random(3));
}
