//      /bin/user/_chfn.c
//      from the Nightmare Mudlib
//      command to change finger info
//      created by Sulam@TMI 911208
//      modified for the Nightmare 3.0 security by Descartes of Borg 930814

#include <std.h>
#include <security.h>

inherit DAEMON;

int cmd_chfn(string arg) {
    if( (string)this_player()->query_name() == "guest" ) return 0;
    write("Changing finger information on " +
	(string) this_player()->query_name() + " at Nightmare.\n");
    write("Default values are printed inside '[]'.\n");
    write("To accept the default, type <return>.\n");
    write("\n");
    write("Name ["+(string)this_player()->query_rname()+"] : ");
    input_to("new_name");
    return 1;
}

static void
new_name(string rname) {
    if( rname && strlen( rname ) > 0 ) {
	this_player()->set_rname(rname);
	this_player()->save_player((string)this_player()->query_name());
    }
    write("Email address ["+this_player()->query_email()+"] : ");
    input_to("new_email");
}

static void
new_email(string e) {
    if( e && strlen( e ) > 0 ) {
	this_player()->set_email(e);
    }
}

int
help() {
  write("Command: chfn\nSyntax: chfn (no args)\n"+
        "This asks for and updates your information for\n"+
        "the finger command.  Please use it if you have\n"+
        "not yet.\n"
        "See also: finger, describe, timezone, passwd\n"
  );
  return 1;
}
