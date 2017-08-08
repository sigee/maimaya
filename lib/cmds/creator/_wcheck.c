/* Cygnus@Nightmare */

#include <balance.h>
#include <std.h>

inherit DAEMON;

string path;
int is_weapon;
string short;
string long;
string name;
string *id;
string type;
int ac, wc, value, weight;
string wield_string;
object wield_func_ob, hit_func_ob;
int decay_rate;
string read_msg;

status
check_weapon(object ob)
{
    status error;
    int i;

    error = 0;

    if (member_array("std/weapon.c", deep_inherit_list(ob)) == -1)
	if (path != "/std/weapon") {
	    write("Warning: " + file_name(ob) +
		" is not a subclass of /std/weapon.\n");
	    error = 1;
	}

    is_weapon = (int)ob->is_weapon();
    if (!intp(is_weapon)) {
	write("Warning: is_weapon() did not return an int.\n");
	error = 1;
    } else
	if (!is_weapon) {
	    write("Warning: is_weapon() did not return true.\n");
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
	if (member_array(type, WEAPON_TYPES) == -1) {
	    write("Warning: query_type() returned unknown weapon " +
		"type: " + type + ".\n");
	    error = 1;
	}

    ac = (int)ob->query_ac();
    if (!intp(ac)) {
	write("Warning: query_ac() did not return an int.\n");
	error = 1;
    } else
	if (ac < WEAPON_MIN_AC[type] ||
	    ac > WEAPON_MAX_AC[type]) {
	    write("Warning: query_ac() is not balanced.\n");
	    error = 1;
	}

    wc = (int)ob->query_wc();
    if (!intp(wc)) {
	write("Warning: query_wc() did not return an int.\n");
	error = 1;
    } else
	if (wc < WEAPON_MIN_WC[type] ||
	    wc > WEAPON_MAX_WC[type]) {
	    write("Warning: query_wc() is not balanced.\n");
	    error = 1;
	}

    value = (int)ob->query_value();
    if (!intp(value)) {
	write("Warning: query_value() did not return an int.\n");
	error = 1;
    } else
	if (value < WEAPON_MIN_VALUE[type] ||
	    value > WEAPON_MAX_VALUE[type]) {
	    write("Warning: query_value() is not balanced.\n");
	    error = 1;
	}

    weight = (int)ob->query_weight();
    if (!intp(weight)) {
	write("Warning: query_weight() did not return an int.\n");
	error = 1;
    } else
	if (weight < WEAPON_MIN_WEIGHT[type]) {
	    write("Warning: query_weight() is not balanced.\n");
	    error = 1;
	}

    wield_string = (string)ob->query_wield_string();

    wield_func_ob = (object)ob->query_wield_func();
    if (objectp(wield_func_ob))
	if (!stringp(function_exists("extra_wield", wield_func_ob))) {
	    write("Warning: function \"extra_wield\" does not exist in " +
		" object " + file_name(wield_func_ob));
	    error = 1;
	}

    hit_func_ob = (object)ob->query_hit_func();
    if (objectp(hit_func_ob))
	if (!stringp(function_exists("extra_hit", hit_func_ob))) {
	    write("Warning: function \"extra_hit\" does not exist in " +
		" object " + file_name(hit_func_ob));
	    error = 1;
	}

    decay_rate = (int)ob->query_decay_rate();
    if (!intp(decay_rate)) {
	write("Warning: query_decay_rate() did not return an int.\n");
	error = 1;
    }

    read_msg = (string)ob->query_read_msg();

    return !error;
}

void
print_weapon(object ob)
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
    write("Weapon Class: " + wc + "  ");
    write("Value: " + value + ((int)ob->query_destroy() ? "*" : "") + "  ");
    write("Weight: " + weight);
    write("\n");

    if (stringp(wield_string))
	write("Wield String:\n" + wield_string);

    if (objectp(wield_func_ob))
	write("Wield Function: \"" + file_name(wield_func_ob) +
	    "\"->extra_wield()\n");

    if (objectp(hit_func_ob))
	write("Hit Function: \"" + file_name(hit_func_ob) +
	    "\"->extra_hit()\n");

    write("Decay Rate: " + decay_rate + "\n");

    if (stringp(read_msg))
	write("Read Message:\n" + read_msg);
}

int
cmd_wcheck(string weapon)
{
    object ob;

    if (!stringp(weapon)) {
	notify_fail("usage: " + query_verb() + " <weapon-id>\n");
	return 0;
    }

    ob = present(weapon, this_player());
    if (!objectp(ob)) {
	notify_fail(query_verb() + ": \"" + weapon + "\" not present.\n");
	return 0;
    }

    path = base_name(ob);

    write("Weapon Check for: " + path + "\n");

    if (check_weapon(ob))
	print_weapon(ob);

    return 1;
}

int help()
{
  write( @EndText
Syntax: wcheck <name>
Effect: Balance command to check weapons.
        Checks the weapon called <name> in your inventory for weight, wc, etc
See doc: /doc/QC/check.doc
See also: acheck, rcheck
EndText
  );
return 1;
}
