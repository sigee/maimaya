//	_lore.c
//	Mage ability
//	created by Bohemund 14 January 1994
//      based upon Descartes' _sense.c

#include <std.h>

inherit DAEMON;

int cmd_lore(string str) {
    object ob;
    object *inv;
    int i, skip, which, conjure;
    string what, Class;
    mixed *arr;

    if(!str) {
	notify_fail("Gather lore about what?\n");
	return 0;
    }
    Class = (string)this_player()->query_class();
    if(Class != "mage") {
	notify_fail("Your mind boggles in confusion.\n");
	return 0;
    }
    ob = present(str, this_player());
    if(!ob) {
	inv = all_inventory(this_player());
	if(sscanf(str, "%s %d", what, which) != 2) {
	    notify_fail("You don't have that!\n");
	    return 0;
	}
	for(i=0, skip = 0; i<sizeof(inv) && !ob; i++) {
	    if(inv[i]->id(what)) {
		skip ++;
		if(skip == which) ob = inv[i];
	    }
	}
	if(!ob) {
	    notify_fail("You don't have that!\n");
	    return 0;
	}
    }
    conjure = (int)this_player()->query_skill("conjuring");
    if((int)this_player()->query_mp() < 13) {
	notify_fail("Too low on magic power.\n");
	return 0;
    }
    say(this_player()->query_cap_name()+" mutters quietly as "+
        this_player()->query_subjective()+" focuses on the "+
        (string)ob->query_name()+".", this_player());
  this_player()->set_magic_round();
  if(conjure < random(101)) {
  this_player()->add_mp(-13);
  this_player()->add_skill_points("conjuring", random(13));
	write("You fail to discern anything special about it.");
	return 1;
    }
    else {
	this_player()->add_mp(-13);
	this_player()->add_skill_points("conjuring", 13);
	arr = (string *)ob->query_property("magic item");
	if(!arr) write("You fail to discern anything special about it.");
	else if(arr == ({})) write("You fail to discern anything "
          "special about it.");
	else write("You discern a magical force eminating from it...\n"
        "You sense the word \"%^BOLD%^"+arr[random(sizeof(arr))]+"%^RESET%^\".");
	return 1;
    }
}

void help() {
    write("Syntax: <lore [item]>\n\n"
	"Allows a mage to discern any magic powers inherent in an object. "
	"There is no way to tell the difference between a failure to discern "
	"magic in the object and the lack of magic in an object.\n");
}
