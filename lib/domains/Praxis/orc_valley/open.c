#include <std.h>

inherit ROOM;

void create() {
    ::create();
    set_property("light", 2);
    set_property("night light", 1);
    set_property("no castle", 1);
    set_short( "An open courtyard in the orc fortress");
    set_long(
        "The courtyard around you is surrounded by ancient stone walls "
        "that gives this fortress the feeling of impregnability.  An "
        "impressive archway south opens out into Orc Valley.  "
        "There are passages east and west.");
    set_items(
        (["courtyard" : "An open yard walled in by stone walls on every side.",
	  "yard" : "It has no roof, but the walls are so high you "
	    "can see only sky.",
	  "stone" : "A grey stone of unknown origin.  There are "
	    "writings on it.",
	  "walls" : "You cannot get beyond them unnaturally.",
	  "fortress" : "It looks extremely old.",
	  "arch" : "A huge, heavily guarded entrance to the fortress.",
	  "archway" : "It forms the huge, heavily guarded entrance to "
	    "the fortress.",
	  "valley" : "You cannot see any of it from inside the courtyard.",
	  "passages" : "They lead to the inner parts of the fortress.",
	  "passage" : "You have no idea where it might lead."]) );
    set_exits( 
	      (["east" : "/domains/Praxis/orc_valley/passage1",
		"west" : "/domains/Praxis/orc_valley/passage2",
		"north" : "/domains/Praxis/orc_valley/shaman",
		"south" : "/domains/Praxis/orc_valley/guard"]) );
    set_search_string( 0, "Search what?");
    set_search_string( "walls", "You notice a secret passage in the north wall.");
    set_search_string("wall", "Which wall?");
    set_search_string("north wall", "You notice a secret passage.");
}




