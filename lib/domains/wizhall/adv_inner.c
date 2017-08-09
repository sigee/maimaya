#include <std.h>
#include "wizhall.h"

inherit ROOM;

int go_up();

void create() {
    ::create();
    set_property("light", 2);
    set_property("indoors", 1);
    set_property("no steal", 1);
    set_short("A halhatatlanok csarnoka");
    set_long("Ebben a csarnokbanban talalkoznak egymassal a halhatatlanok "
             "es itt intezik a dolgaikat. Ilyenek peldaul az uj oletek "
             "megviatatasa, azon tervek hogy egyes halhatatlanok milyen "
             "teruleteket, klanokat, kuldeteseket szeretnenek alkotni, illetve "
             "milyen feladatokat jelolnek ki szamukra. Innen a lepcson lefele "
             "talalhato az egyes otletek, feladatok elbiralasanak kis zuga.");
    set_exits( ([
        "fel" :  BASE_PATH "mudlib",
        "del" : BASE_PATH "lpc_inner",
        "lepcso": BASE_PATH "/app_room",
        "kelet" : BASE_PATH "planning_room"]) );
    set_pre_exit_functions(({"fel"}), ({ (:go_up:) }));
}

int go_up() {
    if (!archp(this_player())) {
        write("Oda csak fonokok mehetnek be!");
        return 0;
    }
    return 1;
}

int receive_objects(object ob) {
    if (!living(ob)) return 1;
    if (!userp(ob)) return 0;
    if (!creatorp(ob)) {
        message("my_action", "Ide csak halhatatlanok lephetnek be!", ob);
        return 0;
    }
    return ::receive_objects(ob);
} 

void reset() {
    object ob;
    ::reset();
    if (!present("tabla")) {
        ob = new("std/bboard");
        ob->set_name("tabla");
        ob->set_id( ({ "tabla", "uzenotabla" }) );
        ob->set_board_id("immortal");
        ob->set_max_posts(30);
        ob->move(BASE_PATH "adv_inner");
        ob->set_short("Halhatatlanok uzenotablaja");
        ob->set_long("Ide irjak fel a halhatatlanok ugyes-bajos dolgaikat, problemaikat.\n");
    }
}
