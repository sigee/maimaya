#include <std.h>
#include "wizhall.h"

inherit ROOM;

void init() {
    ::init();
    if (!archp(this_player())) {
        this_player()->move_player(BASE_PATH "planning_room", "le");
    }
}

void create() {
    ::create();
    set_property("light", 2);
    set_property("indoors", 1);
    set_short("A kisfonokok szobaja");
    set_long("A kisfookok talakozo helye valahola csillagokban. Korulotted "
             "a fenyessen ragyogo csillagog apro csilogo porszemekkent usznak "
             "a semmiben. A kisfonokok itt diskuralnak egymassal.");
    set_exits( (["le" : BASE_PATH "planning_room"]) );
}

void reset() {
    object ob;
    ::reset();
    if (!present("tabla")) {
        ob = new("/std/bboard");
        ob->set_name("tabla");
        ob->set_id( ({ "tabla", "uzenotabla" }) );
        ob->set_board_id("arch");
        ob->set_max_posts(30);
        ob->move(BASE_PATH "arch");
        ob->set_short("A kisfonokok tablaja.");
        ob->set_long("Egy uszo tabla a semmiben.Ide irhatod feluzeneteidet.\n");
    }
}
