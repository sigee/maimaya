// /domains/Praxis/obj/misc/gallows.c
//  Gallows for the Nightmare Mortal Law system
//  Written by Manny@Nightmare  940823

#include <std.h>

object hangman;
void set_hangman(object who);

inherit OBJECT;

create() {
  ::create();
  set_name("gallows");
  set_id( ({ "gallows", "noose", "platform" }) );
  set_short("the gallows");
  set_long( (: "long_func" :) );
  set_prevent_get("You cannot get that!");
}

string long_func() {
  string str, name;
  
  str = "A large wooden platform with two upright timbers and "
    "a crossbeam with a rope and noose.";
  if(hangman) {
    if(name = present("the corpse of "+hangman->query_name(),
        environment(this_object())))
      str += "\n"+name+" is hanging from the noose.";
    else str += "\n"+hangman->query_cap_name()+" is standing "
      "upon the platform, noose around "+hangman->query_possessive()+" neck.";
  }
  return str;
}

void set_hangman(object who) { hangman = who; }
