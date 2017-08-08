//      /bin/dev/_acheck.c
//      from the Nightmare Mudlib
//      a tool for the balancing and checking of armour
//      created by Cygnus@Nightmare 93????

#include <std.h>
#include <balance.h>

inherit DAEMON;

string path;
int is_armour;
string name;
string *id;
string short;
string long;
string type;
int ac, value, weight;
string *limbs;
string wear_string;
object wear_func_ob;
int decay_rate;
string read_msg;

status
check_armour(object ob)
{
    status error;
    int i;

    error = 0;

    if (member_array("std/armour.c", deep_inherit_list(ob)) == -1)
	if (path != "/std/armour") {
	    write("Warning: " + path +
		" is not a subclass of /std/armour.\n");
	    error = 1;
	}

    is_armour = (int)ob->is_armour();
    if (!intp(is_armour)) {
	write("Warning: is_armour() did not return an int.\n");
	error = 1;
    } else
	if (!is_armour) {
	    write("Warning: is_armour() did not return true.\n");
	    error = 1;
	}

    name = (string)ob->query_name();
    if (!stringp(name)) {
	write("Warning: query_name() did not return a string.\n");
	error = 1;
    }

    id = (string *)ob->query_id();
    if (!pointerp(id)) {
	write("Warning: query_id() did not return an array.\n");
	error = 1;
    } else
	for (i = 0; i < sizeof(id); i++)
	    if (!stringp(id[i])) {
		write("Warning: query_id() array member " + i +
		    " is not a string.\n");
		error = 1;
	    }

    short = (string)ob->query_short();
    if (!stringp(short)) {
	write("Warning: query_short() did not return a string.\n");
	error = 1;
    }

    long = (string)ob->query_long();
    if (!stringp(long)) {
	write("Warning: query_long() did not return a string.\n");
	error = 1;
    }

    type = (string)ob->query_type();
    if (!stringp(type)) {
	write("Warning: query_type() did not return a string.\n");
	error = 1;
    } else
	if (member_array(type, ARMOUR_TYPES) == -1) {
	    write("Warning: query_type() returned unknown armour " +
		"type: " + type + ".\n");
	    error = 1;
	}

    ac = (int)ob->query_ac();
    if (!intp(ac)) {
	write("Warning: query_ac() did not return an int.\n");
	error = 1;
    } else
	if (ac < ARMOUR_MIN_AC[type] ||
	    ac > ARMOUR_MAX_AC[type]) {
	    write("Warning: query_ac() is not balanced.\n");
	    error = 1;
	}

    value = (int)ob->query_value();
    if (!intp(value)) {
	write("Warning: query_value() did not return an int.\n");
	error = 1;
    } else
	if (value < ARMOUR_MIN_VALUE[type] ||
	    value > ARMOUR_MAX_VALUE[type]) {
	    write("Warning: query_value() is not balanced.\n");
	    error = 1;
	}

    weight = (int)ob->query_weight();
    if (!intp(weight)) {
	write("Warning: query_weight() did not return an int.\n");
	error = 1;
    } else
	if (weight < ARMOUR_MIN_WEIGHT[type]) {
	    write("Warning: query_weight() is not balanced.\n");
	    error = 1;
	}

    limbs = (string *)ob->query_limbs();
    if (!pointerp(limbs)) {
	write("Warning: query_limbs() did not return an array.\n");
	error = 1;
    } else
	for (i = 0; i < sizeof(limbs); i++)
	    if (!stringp(limbs[i])) {
		write("Warning: query_limbs() array member " + i +
		    " is not a string.\n");
		error = 1;
	    }

    wear_string = (string)ob->query_wear_string();

    wear_func_ob = (object)ob->query_wear_func();
    if (objectp(wear_func_ob))
	if (!stringp(function_exists("extra_wear", wear_func_ob))) {
	    write("Warning: function \"extra_wear\" does not exist in " +
		" object " + file_name(wear_func_ob));
	    error = 1;
	}

    read_msg = (string)ob->query_read_msg();

    return !error;
}

void
print_armour(object ob)
{
    int i;

    write("User IDs: " + getuid(ob) + "/" + geteuid(ob) + "\n");

    write("Name: " + name + "\n");

    write("IDs:");
    for (i = 0; i < sizeof(id); i++)
	write(" \"" + id[i] + "\"");
    write("\n");

    write("Short:\n" + short + "\n");
    write("Long:\n" + long + "\n");

    write("Type: " + type + "\n");

    write("Armour Class: " + ac + "  ");
    write("Value: " + value + ((int)ob->query_destroy() ? "*" : "") + "  ");
    write("Weight: " + weight);
    write("\n");

    write("Limbs:");
    for (i = 0; i < sizeof(limbs); i++)
	write(" \"" + limbs[i] + "\"");
    write("\n");

    if (stringp(wear_string))
	write("Wear String:\n" + wear_string);

    if (objectp(wear_func_ob))
	write("Wear Function: \"" + file_name(wear_func_ob) +
	    "\"->extra_wear()\n");

    if (stringp(read_msg))
	write("Read Message:\n" + read_msg);
}

int
cmd_acheck(string armour)
{
    object ob;

    if (!stringp(armour)) {
	notify_fail("usage: " + query_verb() + " <armour-id>\n");
	return 0;
    }

    ob = present(armour, this_player());
    if (!objectp(ob)) {
	notify_fail(query_verb() + ": \"" + armour + "\" not present.\n");
	return 0;
    }

    path = base_name(ob);

    write("Armour Check for: " + path + "\n");

    if (check_armour(ob))
	print_armour(ob);

    return 1;
}

int help()
{
  write( @EndText
Syntax: acheck <name>
Effect: Balance command to check armor.
        Checks the armor called <name> in your inventory for weight, wc, etc
See doc: /doc/QC/check.doc
See also: rcheck, wcheck
EndText
  );
  return 1;
}
