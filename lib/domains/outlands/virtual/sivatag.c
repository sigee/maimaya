#include "./virtual.h";

inherit VROOM;

void create() {
  ::create();
  set_property("light", 3);
  set_property("outlands", "sivatag");
  set_short("Sivatag");
}

void reset() {
  ::reset();
  if (present("sivatag_area_monster")) return;
  if (random(8) == 1) {
    string mons = MONS["sivatag"][random(sizeof(MONS["sivatag"]))];
    new("/domains/outlands/virtual/mons/" + mons)->move(this_object());
  }
} 