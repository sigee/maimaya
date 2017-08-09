#include <std.h>
#include "wizhall.h"

inherit ROOM;

int go_up();

void create() {
    ::create();
    set_property("light", 2);
    set_property("indoors", 1);
    set_short("Tervezo szoba");
    set_long("Minden otletet itt lehet megvitatni. Ha nem irod fel otleteidet "
             "es mas megteszi helyetted learatva a baberokat magadra vess.");
    set_exits( ([
        "nyugat" : BASE_PATH "adv_inner",
        "fel": BASE_PATH "arch",
     ]) );
    set_pre_exit_functions(({"fel"}), ({ (:go_up:) }));
}

void reset() {
    object ob;
    ::reset();
    if (!present("tabla")) {
        ob = new("/std/bboard");
        ob->set_name("tabla");
        ob->set_id( ({ "tabla", "uzenotabla" }) );
        ob->set_board_id("planning");
        ob->set_max_posts(30);
        ob->move(BASE_PATH "planning_room");
        ob->set_short("Otletek tablaja");
        ob->set_long("Egy tabla az otleteknek, hogy azokat senki ne "
                     "hasznalja.\n");
    }
}

int go_up() {
    if (!archp(this_player())) {
        write("Oda csak fonokok mehetnek be!");
        return 0;
    }
    return 1;
}
