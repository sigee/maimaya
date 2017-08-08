#include <std.h>

inherit ROOM;

create() {
  ::create();
  set_properties( ([ "indoors" : 1, "light" : 1, "no killing" : 1, 
      "no stealing" : 1, "no magic" : 1, "no teleport" : 1 ]) );
  set_short("Torture Room");
  set_long("You are in the infamous Torture hall, a small "
  "room used by the Praxis Guard on only the worst of criminals.  "
      "From your vantage point upon a "
      "table in the center of the room, you are unable to gather "
      "much information about your surroundings.  However, you can "
      "see various devices scattered around the room, covered "
      "with dust and cobwebs.");
  set_items( ([ "devices" : "Assorted mechanical devices used in the "
      "performing cruel and unusual punishments.", 
    "table" : "An uncomfortable cement table." ]) );
  set_smell("default", "The pungent odor of rotting corpses fills "
      "the air.");
}

init() {
  ::init();
  if(this_player()->query_ghost()) 
    this_player()->move("/domains/Mountains/cave/cave3");
}
