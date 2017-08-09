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
    set_short("Az �j halhatatlanok szob�ja");
    set_long("Egy �j halhatatlan dolgozik itt.");
    set_exits( ([
        "foter" : ROOM_START,
        "hall" : ROOM_HALL
    ]) );
    set_items( ([ "targy" : "Ez egy t�rgy a szob�dban." ]));
    set_smell("default", "Friss szag terjeng a leveg�ben.");
    set_listen("default", "Hallod a billenty�k kopog�s�t.");
}
