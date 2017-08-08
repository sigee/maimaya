#include "./virtual.h";

inherit VROOM;

void create() {
  ::create();
  set_property("light", 2);
  set_property("outlands", "mezo");
  set_short("Mezo");
}

void reset() {
  ::reset();
  if (present("mezo_area_monster")) return;
  if (random(8) == 1) {
    string mons = MONS["mezo"][random(sizeof(MONS["mezo"]))];
    new("/domains/outlands/virtual/mons/" + mons)->move(this_object());
  }
}
