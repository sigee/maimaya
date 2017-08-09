#include <std.h>
#include "wizhall.h"

inherit ROOM;

void create() {
    ::create();
    set_property("light", 2);
    set_property("indoors", 1);
    set_short("A Fonokseg szobaja");
    set_long("Ebben a szobaban tudod megnezni a munka elorehaladtat a hibakon "
             "amik  a jatekban talalhatoak. Ezenkivul itt tudod jelenteni "
             "azokat a hibakat amik nem kaptak meg a szerinted kello "
             "figyelmet. Hasznald batran az elozokhoz a tabla segitseget.");
    set_exits( (["le" : BASE_PATH "adv_inner"]) );
}

void reset() {
    object ob;
    ::reset();
    if (!present("tabla")) {
        ob = new("/std/bboard");
        ob->set_name("tabla");
        ob->set_id( ({ "tabla", "uzenotabla" }) );
        ob->set_board_id("mudlib");
        ob->set_max_posts(50);
        ob->move(BASE_PATH "mudlib");
        ob->set_short("A fonokseg uzenotablaja");
        ob->set_long("A nagyon es kevesbe fontos fonoksegi ugyek tablaja.\n");
    }
}
