// fixed by drakken 5.Nov.94
#include <std.h>

inherit DAEMON;

int cmd_peer(string str) {
    string *dirlist, *destlist;
    string ret, tmp, l, dest;
    object who;
    int i, x;

    if(!str) {
        message("my_action", "You peer around the room.", this_player());
        message("other_action", sprintf("%s peers around the room",
          this_player()->query_cap_name()), environment(this_player()),
          ({ this_player() }));
        return 1;
    }
    if((x=effective_light(this_player())) > 6)
        return notify_fail("It is much too bright.\n");
    if(x < 0) return notify_fail("It is too dark.\n");
    if(who = present(str, environment(this_player()))) {
        message("my_action", "You peer at "+(string)who->query_cap_name()+".",
          this_player());
        message("other_action", (string)this_player()->query_cap_name()+
          " peers at "+(string)who->query_cap_name()+".", 
          environment(this_player()), ({ this_player(), who }));
        message("other_action", (string)this_player()->query_cap_name()+
          " peers at you.", who);
        return 1;
    }
    if(!sizeof(dirlist= (string *)environment(this_player())->query_exits()))
      return notify_fail("There are no openings through which to peer!\n");
    if(member_array(str, dirlist) == -1) {
        if(member_array(str, (string)environment(this_player())->query_exits())
          == -1) return notify_fail("You cannot see much like that!\n");
        else return notify_fail("You cannot peer into something like that!\n");
    }
    if(!(dest = (string)environment(this_player())->query_exit(str)))
      return notify_fail("You cannot see much in that direction.\n");
     tmp = (string)(environment(this_player())->query_door(str));
     if (tmp && !environment(this_player())->query_open(tmp))
      return notify_fail("The "+tmp+" blocks your view.\n");
    dest = replace_string(dest, ".c", ""); //Kalinash 1-14-94
    if(!unguarded((: file_exists, dest+".c" :)))
      return notify_fail("You cannot see much in that direction.\n");
    if((int)dest->query_property("light") < 0) 
      message("my_action", "It is too dark.", this_player());
    else if((int)dest->query_property("light") > 6)
      message("my_action", "It is too bright that way.", this_player());
    else {
      message("room_description", (string)dest->query_long(0), this_player());
      ret = (string)dest->describe_living_contents(({}));
      if (ret && ret != "")
	  message("living_item", ret, this_player());
      ret = (string)dest->describe_item_contents(({}));
        message("inanimate_item", ret, this_player());
    }
    message("other_action", this_player()->query_cap_name()+
      " peers "+str+".", environment(this_player()), ({this_player()}));
    return 1;
}

int help()
{
    message("help","Syntax: <peer direction>\n\n"
     "Allows you to look into the neighbouring room in the given "
     "direction if nothing blocks your sight.", this_player() );
   return 1;
}
