#include <std.h>
#include "wizhall.h"

inherit ROOM;

void create() {
    ::create();
    set_property("light", 2);
    set_property("indoors", 1);
    set_short("Rabolintas also zuga");
    set_long("Ez a kis zug a javaslatok elbiralasanak a helye. Itt lehet "
             "megviatatni minden problemat, javaslatot amik az uj "
             "fejlesztesekkel  kapcsolatban merulnek fel. Ezenkivul itt "
             "fogadjak el az ujj teruletek alkotasarol szolo terveket. Innen "
             "felfele talalhato a halhatatlanok csarnoka.");
    set_exits( ([
        "fel": BASE_PATH "adv_inner"
    ]) );
}

void reset() {
    object ob;
    ::reset();
    if (!present("tabla")){
        ob = new("std/bboard");
        ob->set_name("tabla");
        ob->set_id( ({ "tabla", "uzenotabla" }) );
        ob->set_board_id("teruletektervek");
        ob->set_max_posts(30);
        ob->move(BASE_PATH "app_room");
        ob->set_short("Teruletek es fejlesztesek tablaja");
        ob->set_long("Ezen a tablan tortenik minden terulettekkel es "
                     "feladatokkal kapcsolatos megbeszeles.\n");
    }
}
