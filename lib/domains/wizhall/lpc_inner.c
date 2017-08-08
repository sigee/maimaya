#include <std.h>
#include "wizhall.h"

inherit ROOM;

void create() {
    ::create();
    set_property("light", 2);
    set_property("indoors", 1);
    set_short("Kodolas szobaja");
    set_long("A halhatatlanok itt beszelik meg a kodolassal kapcsolatos "
             "problemaikat. Itt folyik meg az ujj parancsok es egyebb "
             "programozasi lehetosegek ismertetese is. Ebben a tabla segitsege "
             "veheto igenybe. Eszakra talahato a halhatatlanok csarnoka.");
    set_exits( (["eszak" : BASE_PATH "adv_inner"]) );
}

void reset() {
    object ob;
    ::reset();
    if (!present("tabla")) {
        ob = new("std/bboard");
        ob->set_name("tabla");
        ob->set_id( ({ "tabla", "tablaja", "uzenotabla" }) );
        ob->set_board_id("coding");
        ob->set_max_posts(30);
        ob->move(BASE_PATH "lpc_inner");
        ob->set_short("A kodolas dolgainak tablaja");
        ob->set_long("A tabla a kodolassal kapcsolatos kerdesek problemak "
                     "megvitatasanak a helye.\n");
    }
}
