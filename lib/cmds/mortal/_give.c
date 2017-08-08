//      _give.c
//      Mostly the _give.c from the TMI distribution mudlib.
//      Written by Sulam 12.12.91
//      Help added 1.28.92 by Brian
//      Fixed for the Nightmare monetary system by Descartes of Borg 
//      31 October 1992
// Light checking added by Hanse 4/11/92  :P

#include "move.h"
#include <std.h>

inherit DAEMON;

int
help();

cmd_give(string str) {
   object ob, to;
   string what, who, type;
   int i, amount;
    int dummy;
    object *inv;
   
    if (!stringp(str)) return help();
    dummy = total_light(this_player());
    if(dummy < -1) {
      write("It is too dark.\n");
      return 1;
   }
   if (sscanf(str,"%s to %s", what, who) != 2) {
      return help();
   }
   to = present(who, environment(this_player()));
   if (!to) {
      notify_fail("Give "+what+" to who?\n");
      return 0;
   }
   ob = present(what, this_player());
   if (!ob) {
      ob = parse_objects(this_player(), what);
      if(!ob) {
          if(sscanf(what, "%d %s coins", amount, type) !=2) {
              notify_fail("You must have an object to give it away!\n");
              return 0;
          }
          if (amount<0) {
	    notify_fail("Yeah, right.\n");
	    return 0;
          }
          if(amount > (int)this_player()->query_money(type)) {
              notify_fail("You don't have that much!\n");
              return 0;
          }
          this_player()->add_money(type, -amount);
          to->add_money(type, amount);
           write("You give " + amount + " " + type + " coins to " +
                  to->query_cap_name() + ".");
           tell_room(environment(this_player()), this_player()->query_cap_name()
           + " gives some money to " + to->query_cap_name() + ".", ({ this_player(), to }) );
          tell_object(to, this_player()->query_cap_name() +
           " gives you " + amount + " " + type + " coins.");
           return 1;
        }
   }
   if (!living(to)) {
      notify_fail("Only living objects can accept items.\n");
      return 0;
   }
   if(to->query_ghost()) {
        notify_fail(to->query_cap_name()+" is unable to hold anything in that state!\n");
        return 0;
    }
    if(!ob->get()) {
        notify_fail("You cannot give that away!\n");
        return 0;
    }
   i = (int) ob->move(to);
   switch(i) {
      case MOVE_OK: {
         write("You give " + ob->query_short() + " to " +
            to->query_cap_name() + ".");
         say(this_player()->query_cap_name()+" gives "+ob->query_short()+
            " to " + to->query_cap_name() + ".", to);
         tell_object(to,this_player()->query_cap_name()+" gives you "+
            ob->query_short() + ".");
         return 1;
         }
      case MOVE_NO_ROOM: {
         notify_fail(to->query_short()+" can't carry any more.\n");
         return 0;
         }
      default: {
         notify_fail("Oops, can't do that.\n");
         return 0;
         }
   }
}

int
help() {
    message("help","Syntax: give <item> to <player>\n\n"
        "This command will make you give an item in your inventory "
        "to the player specified.  You must be in the same room for "
        "this to occur.",this_player());
  return 1;
}
