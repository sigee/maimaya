/* Cygnus@Nightmare */

#include <std.h>

inherit DAEMON;

int
cmd_users() {
    int i;
    object *user_ob;
    string name, *user_name;

    user_ob = users();
    user_name = ({ });
    for (i = 0; i < sizeof(user_ob); i++) {
	if (!user_ob[i]->is_player() || user_ob[i]->query_invis())
	    continue;
	name = (string)user_ob[i]->query_name();
	if (stringp(name))
	    user_name += ({ capitalize(name) });
    }
    user_name = sort_array(user_name, "sort_names");
	write(format_page(user_name, 4));
	write(sprintf("Total : %d", sizeof(user_name)));
    return 1;
}

int
sort_names(string name1, string name2) {
    if (name1 == name2)
	return 0;
    if (name1 < name2)
	return -1;
    else
	return 1;
}

int help()
{
  write( @EndText
Syntax: users
Effect: Lists the names of player logged in. 
        A shorter and quicker version of "who"
See also: who, where
See also: say, tell, class
EndText
  );
  return 1;
}
