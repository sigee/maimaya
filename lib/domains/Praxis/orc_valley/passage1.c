inherit "std/room";

void create() {
  ::create();
    set_property("light", 1);
    set_property("indoors", 1);
    set_short( "A dark passage");
    set_long(
        "You are in a dark passage inside the orc fortress.");
    set_items(
        (["passage" :  "You cannot see much of anything."]) );
    set_exits( 
	      (["east" : "/domains/Praxis/orc_valley/chamber1",
		"west" : "/domains/Praxis/orc_valley/open"]) );
}

