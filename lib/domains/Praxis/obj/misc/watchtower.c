//      /obj/monument.c
//      from the Nightmare mudlib
//      The time keeper of Krasna Square
//      created by Daroki@Nightmare

#include <config.h>
#include <daemons.h>
#define DONATORS_LIST ({ "Kalinash", "Kazmandu", "Ariel", "Mikla", "Ninja",\
"Shambere", "Cair", "Drittz", "Beorn", "Vilesilencer", "Elwin", "Silenus",\
"Corvar", "Lassondra", "Bohemund", "Maria", "Arugess", "Drakken", "Dana",\
"Soong", "Slade", "Rearden", "Stonehenge", "Shadowwolf", "Descartes",\
})


void query_long_time();
string query_hour();

inherit "/std/Object";

void create() {
    ::create();
    set_name("monument");
    set_id( ({ "monument", "tower", "clock" }) );
    set_long(
      "This monument stands at the centre of Praxis to remind "
      "everyone of those who have made this reality possible for "
      "everyone to enjoy.  On one face of the monument is a clock "
      "which shows the time in Praxis.  All other sides are "
      "engraved with the names of those who have contributed "
      "in money to this world.  <read clock> will tell you the time.  "
      "<read names> to see who all has donated, and do thank them."
    );    set_weight( 0);

    set_no_clean(1);
}

void init() {
   add_action("read", "read");
}

int read(string str) {
    if(str == "clock") {
      query_long_time();
      return 1;
   }
    else if (str == "names") {
      message("Ninfo", "These people have donated money and made Nightmare possible:\n"+
      format_page(sort_array(DONATORS_LIST, "alpha", this_object()), 5)
      ,this_player());
      return 1;
    }
   notify_fail("Read what?\n");
   return 0;
}

void query_long_time() {
    write(capitalize(season(time()))+" hovers over "+mud_name()+" on the "+
    date(time())+ordinal(date(time()))+" day of "+
      month(time())+" in year "+year(time())+" NM.\n"
    );
    write("The clock face reads "+query_hour()+".\n");
}

mixed query_hour() {
    int tmp, tmp2;
    string str;

    tmp = hour(time());
    if(!tmp) tmp =10;
    else if(tmp > 10) tmp -= 10;
    tmp2 = minutes(time());
    str = (hour(time()) < 10 ? "am" : "pm");
    return tmp + ":" + (tmp2 > 9 ? tmp2 : "0"+tmp2)+" "+str;
}


int get() { return 0; }

string affect_environment() { return "A magnificent monument rises above the square."; }

int alpha(string a, string b) { return strcmp(a,b); }
