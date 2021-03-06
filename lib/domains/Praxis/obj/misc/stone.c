//  /domains/Praxis/obj/misc/stone.c
//  Stone for throwing at criminals and stuff.
//  For the Nightmare mortal law system
//  Created by Manny@Nightmare 940901
 
#include <std.h>
 
object target;
void set_target(object who);
 
inherit OBJECT;
 
create() {
  ::create();
  set_name("stone");
  set_id( ({ "stone", "throwing stone" }) );
  set_short("a throwing stone");
  set_long( (: "long_func" :) );
  set_weight(2);
  set_value(0);
}
 
long_func() {
  if(target) {
    message("say", "A throwing stone just waiting to be thrown at "+
        target->query_cap_name()+"'s forehead.", this_player());
  } else {
    message("say", "A throwing stone whithout direction.", 
        this_player());
  }
}
 
init() {
  ::init();
  add_action("throw_stone", "throw");
}
 
int throw_stone(string str) {
  string what, who;
  string limb;
  
  if(!str) {
    notify_fail("Throw what?\n");
    return 0;
  }
  if(!sscanf(str, "%s at %s", what, who) == 2) {
    notify_fail("Throw what at whom?\n");
    return 0;
  }
  if(!id(what)) {
    notify_fail("Throw what?\n");
    return 0;
  }
  if(who != (string)target->query_name()) {
    message("say", "The stone misses completely.", this_player());
    message("say", this_player()->query_cap_name()+" throws a "
        "stone in some random direction.", environment(this_player()),
        this_player());
    return 1;
  }
  switch(random(4)) {
    case 0:
      message("say", "You hit "+target->query_cap_name()+" square "
          "in the forehead!!", this_player());
      message("say", this_player()->query_cap_name()+" hits "+
          target->query_cap_name()+" in the forehead with a stone.",
          environment(this_player()), ({ this_player(), target }) );
      message("say", this_player()->query_cap_name()+" hits "
          "you squarely in the head with a small stone!!", target); 
      limb = "head";
      break;
    case 1:
      message("say", target->query_cap_name()+" screams in agony "
          "as you hit "+target->query_possessive()+" squarely in "
          "the nose!", this_player());
      message("say", target->query_cap_name()+" screams in agony "
          "as "+this_player()->query_cap_name()+" hits "
          +target->query_possessive()+" squarely in "
          "the nose!", environment(this_player()),
          ({ this_player(), target }) );
      message("say", "You scream in agony as a stone plows into your "
          "nose!!", target);
      limb = "head";
      break;
    case 2:
      message("say", "OUCH! You stone lands right on "+
          target->query_possessive()+" nards!!", this_player());
      message("say", this_player()->query_cap_name()+" is quite "
          "a good shot with those stones...", 
          environment(this_player()), ({ this_player(), target }));
      message("say", this_player()->query_cap_name()+" pegs you "
          "right in the nards!! OUCH!", target);
      limb = "torso";
      break;
    case 3:
      message("say", "You plant a stone right in "+
          target->query_possessive()+" stomach.", this_player());
      message("say", this_player()->query_cap_name()+" mercilessly "
          "beats on "+target->query_cap_name()+" with the stones.",
          environment(this_player()), ({ this_player(), target }));
      message("say", this_player()->query_cap_name()+" plugs a stone "
          "into your stomach, knocking your breath from you.", target);
      limb = "torso";
      break;
  }
  target->do_damage(limb, random(50)+10);
  if(((int)target->check_on_limb(limb) == 2) || (target->query_hp() < 0)) 
    target->die();
  return 1;
}
 
void set_target(object who) { target = who; }
