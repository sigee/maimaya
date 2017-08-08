//	/bin/adm/_register.c
//	from the Nightmare mudlib
//	for restricting access for certain sites
//	created by Shadowwolf@Nightmare?
// Edit stuff added by Manny@Nightmare 940819

#include <std.h>
#include <security.h>
#include <daemons.h>
#include <dirs.h>

inherit DAEMON;

void end_edit(string site);
void abort();

int cmd_register(string str) {
    if(!archp(this_player())) return 0;
    if(!str) {
      notify_fail("Syntax: register <site-ip>\n\n");
      return 0;
    }
    write("%^RED%^Enter all applicable information, including names, "
        "offenses and email addresses.");
    rm(DIR_TMP+"/"+this_player()->query_name()+".tmp");
    this_player()->edit(DIR_TMP+"/"+this_player()->query_name()+".tmp",
        (: end_edit :), (: abort :), str);
    return 1;
}

void end_edit(string site) {
  string *lines;
  string tmpfile, res, str;
  int x, num;
  
  tmpfile = read_file(DIR_TMP+"/"+this_player()->query_name()+
      ".tmp");
  rm(DIR_TMP+"/"+this_player()->query_name()+".tmp");
  tmpfile = replace_string(tmpfile, "\n", " ");
  num = sizeof(lines = explode(wrap(tmpfile, 60), "\n"));
  str = " - "+site+" placed on Registration\n";
  str += "   by "+(string)previous_object()->query_cap_name()+": "+
      ctime(time())+"\n";
  for(x=0; x<num; x++) 
    str +=  "      * "+lines[x]+"\n";
  
  if(res = catch(call_other(BANISH_D, "register_site", site))) 
    write("Error in registering site: "+res+"\n");
  
  log_file("watch/register", "\n" + str);
  write("%^RED%^%^BOLD%^"+site+" is now on registeration!");
}

void abort() {
  rm(DIR_TMP+"/"+this_player()->query_name()+".tmp");
  write("%^RED%^Site not registered!");
}

void help() {
    write("Syntax: <register [site]>\n\n");
    write("Makes a given site needs to register their characters with\n");
    write("law before being able to play the mud.  A reason must be given\n");
    write("and it must be good.  In general, only law should be using\n");
    write("this command.  And if you are not law, you better mail law.\n");
    write("This command only works with numeric addresses, ie:\n");
    write("    register 134.181.*                ok\n");
    write("    register 134.181.1.12             ok\n");
    write("    register orlith.bates.edu         BAD\n");
    write("See also: unbanish, whobanished, wholetin, whoregistered, whowatched\n");
}
