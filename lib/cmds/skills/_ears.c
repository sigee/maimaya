//    /cmds/skills/_ears.c
//    Nightmare Mudlib
//    A mage spell for listening in on what's going
//    on in a room.
//    by Kalinash@Nightmare April 17, 1994.

#include <std.h>

inherit DAEMON;

int cmd_ears() {
    object who, orb;
    int x, amt;
    string Class;

    who = this_player();
    Class = who->query_class();
    if((int)environment(who)->query_property("no magic"))
        return notify_fail("A magic force prevents your spell.\n");
    if(Class != "mage" && Class != "monk" && Class != "kataan"
      && Class != "cleric")
        return notify_fail("You mumble in confusion.\n");
    if((int)who->query_skill("conjuring") < 10)
        return notify_fail("You speak the arcane words of the spell but "
          "nothing happens.\n");
    if((int)who->query_mp() < 1)
        return notify_fail("Your mind is too fatigued.\n");
    x = who->query_stats("wisdom") + who->query_skill("conjuring");
    amt = x / 10;
    amt = random(10 + amt);
    who->add_mp(-amt*2);
    who->add_skill_points("conjuring", amt*1.5);
    orb = new("/std/obj/ears_orb.c");
    orb->move(environment(who));
    orb->set_caster(this_player());
    orb->set_caster_level((int)this_player()->query_level());
    orb->start_dest(amt);
    message("my_action", "As you complete the incantation, a small orb appears and hovers about the area.", who);
    message("their_action", sprintf("%s chants some archane words.", who->query_cap_name()), environment(who), who);
    return 1;
}

void help() {
    message("help", "Usage : <ears>\n\nThis spell will allow you to conjure up a Wizard's Ear to keep tabs on what is going on in a room.", this_player());
}
