//      /bin/adm/_unbanish.c
//      from the Nightmare Mudlib
//      unbanishes a previously banished name
//      created by Shadowwolf@Nightmare
//      rewritten for new security system by Descartes of Borg 930809

#include <std.h>
#include <security.h>
#include <daemons.h>

inherit DAEMON;

int cmd_unbanish(string str) {
    if(!archp(previous_object())) return 0;
    if(!str) return 0;
    write(capitalize(str = lower_case(str))+" is no longer banished.\n");
    catch(call_other(BANISH_D, "unbanish_name", str));
    return 1;
}

void help() {
    write("Syntax: <unbanish [name]>\n\n"
      "Removes a name from the list of banished names.\n"
    );
}
