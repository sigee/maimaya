#include <std.h>

inherit DAEMON;

void send_messages(object tp, object ob, string Class, string limb);

int cmd_mend(string str) {
    string whom, limb, Class;
    int amount, cost, healing, i, wisdom;
    object tp, ob;
    string * limbs;

    if(!str) {
	notify_fail("Mend whom?\n");
	return 0;
    }
    tp = this_player();
    if(tp->query_casting()) {
	notify_fail("You are in the middle of another spell!\n");
	return 0;
    }
    if(!alignment_ok(this_player())) {
        notify_fail("You have betrayed the source of your powers.\n");
        return 0;
    }
    if((string)this_player()->query_name() == "guest" ||
        this_player()->query_ghost()) {
        notify_fail("You cannot do that.\n");
        return 0;
    }
    healing = (int)tp->query_skill("healing");
    wisdom = (int)tp->query_stats("wisdom");
    Class = (string)tp->query_class();
    if(str == "me" || str == "myself") ob = tp;
    else ob = present(str, environment(tp));
    if(!ob) {
        if(sscanf(str, "%s %s", whom, limb) == 2) {
	    if(whom == "me" || whom == "myself") ob = tp;
	    else ob = present(whom, environment(tp));
        }
    }
    if(!ob) {
	notify_fail("Mend whom?\n");
	return 0;
    }
    if(!limb) cost = 11;
    else if(limb == "all") cost = 22;
    else if(member_array(limb, (string *)ob->query_limbs()) == -1) {
	notify_fail("You do not have a "+limb+".\n");
	return 0;
    }
    else cost = 7;
    if(ob != tp) cost = random(cost-1)+1;
    if((int)tp->query_mp() < cost) {
	notify_fail("Too low on magic power.\n");
	return 0;
    }
    this_player()->set_magic_round();
    tp->add_mp(-cost);
    amount = random(wisdom/2 + 2) + healing/10;
    if(tp == ob) tp->add_skill_points("healing", amount);
    else {
	tp->add_skill_points("healing", amount * 2);
    if(this_player() != ob) {
    if((string)tp->query_class()=="monk" && ob->query_alignment() < 0)
      tp->add_alignment(-amount);
    else tp->add_alignment(amount);
    }
    }
    if(!limb) {
	ob->add_hp(amount);
	send_messages(tp, ob, Class, 0);
    }
    else if(limb == "all") {
	limbs = (string *)ob->query_limbs();
	for(i=0; i<sizeof(limbs); i++) {
	    ob->heal_limb(limbs[i], (amount / 2));
	}
	ob->add_hp(amount / 2);
	send_messages(tp, ob, Class, limb);
    }
    else {
	ob->heal_limb(limb, amount);
	send_messages(tp, ob, Class, limb);
    }
    return 1;
}

void send_messages(object tp, object ob, string Class, string limb) {
    string what, thing;

    if(Class == "monk" || Class == "cleric") {
	thing = "prayer";
	what = "a prayer of healing";
    }
    else if(Class == "mage") {
	thing = "magic";
	what = "a spell of healing";
    }
    else {
	thing = "nonsense";
 	what = "nonsense";
    }
    if(tp == ob) {
       tell_object(tp, "You mend your own wounds through "+thing+".");
       tell_room(environment(tp), tp->query_cap_name()+" mutters "+what
            +".", ({tp}));
    }
    else {
	tell_object(tp, "You mend "+ob->query_cap_name()+"'s wounds through "+thing+"");
	tell_object(ob, tp->query_cap_name()+" mends your wounds through "+thing+"");
	tell_room(environment(tp), tp->query_cap_name()+" mutters "+what+"", ({tp, ob}));
    }
}


int help()
{
  write( @EndText
Syntax: mend <player>
Effect:a healing magic command
        restores some of <player>'s lost hit points
EndText
  );
  return 1;
}
