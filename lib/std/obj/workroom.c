/*
    /std/obj/workroom.c
    from Nightmare IV
    a standard workroom style object for new immortals
    created by Descartes of Borg 940316
*/

#include <rooms.h>
#include <std.h>

inherit ROOM;

void create() {
    ::create();
    set_property("light", 2);
    set_property("indoors", 1);
    set_short("Az új halhatatlanok szobája");
    set_long("Egy új halhatatlan dolgozik itt.");
    set_exits( ([
        "foter" : ROOM_START,
        "hall" : ROOM_HALL
    ]) );
    set_items( ([ "targy" : "Ez egy tárgy a szobádban." ]));
    set_smell("default", "Friss szag terjeng a levegõben.");
    set_listen("default", "Hallod a billentyûk kopogását.");
}
