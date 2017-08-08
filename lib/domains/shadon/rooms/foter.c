#include <std.h>
#include "../shadon.h";

inherit ROOM;

string query_long_time();
string look_at_map();
mixed query_hour();

void create() {
    ::create();
    set_property("light", 3);
    set_property("night light", 2);
    set_property("no steal", 1);
    set_property("no magic", 1);
    set_property("no attack", 1);
    set_property("no castle", 1);
    set_short("Shadon fotere");
    set_long("A regen Voros ternek nevezett Fo teren allsz. Shadon ket fo "
             "utja keresztezi itt egymast, a Fo utca es a Boc La ut. A ter "
             "keleti szelen egy hatalmas %^BOLD%^%^BLACK%^szikla%^RESET%^ all, "
             "amelybe belevestek Shadon %^BLUE%^terkepet%^RESET%^.  A ter "
             "kozepen egy gyonyoru %^YELLOW%^napora%^RESET%^ talalhato ami "
             "mindig a pontos idot mutatja. A ter nyugati reszen egy "
             "%^CYAN%^szokokut%^RESET%^at latsz.");
    set_items( ([
        "terkep" : (: look_at_map :),
        ({ "kut", "szokokut" }) : "Egy hatalmas szokokut, talan "
                                  "%^GREEN%^kortyol%^RESET%^hatsz is belole.",
        "szikla" : "Egy hatalmas szikla amelybe belevestek Shadon "
                   "%^BLUE%^terkepet%^RESET%^.",
        ({ "ora", "napora" }) : (: query_long_time :)
    ]) );
    set_exits( ([
    ]) );
}

void init() {
    ::init();
    add_action("do_kortyol", "kortyol");
}

string look_at_map() {
    return read_file(ANSI_PATH + "map.rc");
}

string query_long_time() {
    string msg;
    msg = "\n%^BOLD%^";
    switch (season(time())) {
        case "tavasz": msg += "%^CYAN%^"; break;
        case "nyar": msg += "%^YELLOW%^"; break;
        case "osz": msg += "%^RESET%^%^ORANGE%^"; break;
        default: msg += "%^WHITE%^"; break;
    }
    msg += capitalize(season(time())) + " uralja a vilagot, ";
    msg += "" + year(time()) + ". ev " + month(time()) + " havanak ";
    msg += "" + date(time()) + ". napjan. (" + day(time()) + ")";
    msg += "%^RESET%^\n\n";
    msg += "%^BOLD%^%^RED%^A mud pontos ideje%^RESET%^: ";
    msg += "%^BOLD%^%^WHITE%^" + query_hour() + "%^RESET%^\n";
    return msg;
}

mixed query_hour() {
    int hour, minutes;
    hour = hour(time());
    minutes = minutes(time());
    return hour + ":" + (minutes > 9 ? "" : "0") + minutes);
}

int do_kortyol(string str){
    if (!str || str == "") {
        return notify_fail("Megis mibol akarsz kortyolni?\n");
    }
    if (str != "kut" && str != "szokokut") {
        return notify_fail("Abbol itt nem kortyolhatsz!\n");
    }
    if (this_player()->query_quenched() > 4950) {
        return notify_fail("Nem tudsz tobbet inni!\n");
    }
    this_player()->add_quenched(10);
    write("Kortyolsz egyet a kut friss vizebol.");
    say(this_player()->query_cap_name() + " kortyol egyet a kut friss "
        "vizebol.");
    return 1;
}
