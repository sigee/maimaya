#include "./virtual.h";

inherit VROOM;

void create() {
  ::create();
  set_property("light", 2);
  set_property("outlands", "orszagut");
  set_short("Orsz�g�t");
}