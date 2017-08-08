//	/bin/dev/_sponsor.c
//	from the Nightmare mudlib
//	a command to allow mentors to sponsor new wizards
//	created by Descartes of Borg 31 january 1993
//	modified by Drakken 4 Nov 1994

#include <std.h>
#include <security.h>

inherit DAEMON;

int cmd_sponsor(string str) {
    string file;
    string *lines;
    int i;
    object who;

     if(!str) return notify_fail("Sponsor whom?\n");
    who = find_living(str);
     if(!who) return notify_fail("No "+capitalize(str)+" on the mud.\n");
     if((string)who->query_position() != "new immortal")
	return notify_fail(capitalize(str)+" is still a mortal!\n");
	notify_fail(capitalize(str)+" is still a mortal!\n");
    if(!archp(this_player()) && (string)this_player()->query_position() != "mentor")
	return notify_fail("You are not allowed to sponsor anyone!\n");
    who->set_level(1);
    who->set_position("immortal");
    write_file("/log/adm/sponsor", str+" ("+(string)this_player()->query_name()+
	":"+ctime(time())+")\n");
    message("info", sprintf("You are now a full immortal!\nYour home "
      "directory is %s.", user_path(str)), who);
    message("info", sprintf("%s is now a full immortal!", capitalize(str)),
      users(), ({ who }));
    mkdir("/realms/"+lower_case(str));
    write_file(user_path(str)+"workroom.c", read_file("/std/obj/workroom.c"));
    return 1;
}

void help() {
    write("Syntax: <sponsor [player]>\n\n"+
	"Mentors and above may use this command to sponsor new immortals\n"+
	"from level 20 immortality to level 21, and it makes the directory\n"+
	"as well.\n");
}
