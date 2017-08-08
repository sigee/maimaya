/* Cygnus@Nightmare */
/* FIXED by nialson */

#define SKYDUMMY "(: OBJ(/adm/daemon/astronomy_d), \"la_sky\" :)"
#define NITPICKY 0

#include <std.h>
inherit DAEMON;

string path;
string short;
string night_long;
string day_long;
string long;
string *destinations;
mapping item;

status
check_room(object ob) {
   int i;
   status error;
   string message;
   string *items;
   string desc;
   string func;
   
   error = 0;
   
   if (member_array("std/room.c", deep_inherit_list(ob)) == -1)
      if (path != "/std/room") {
	 write("Warning: " + path + " is not a subclass of /std/room.\n");
	 error = 1; }
   
   if (!stringp(geteuid(ob)))
      write("Warning: geteuid() did not return a string.\n");
   
   short = (string)ob->query_short();
   if (!stringp(short)) {
      write("Warning: query_short() did not return a string.\n");
      error = 1; }

   night_long = (string)ob->query("night long");
   day_long = (string)ob->query("day long");
   long = (string)ob->query_long();
   if((string)ob->query_extra_long() != "") long += ob->query_extra_long();
   if (stringp(night_long) == stringp(day_long)) {
      if (!stringp(night_long) && !stringp(long)) {
	 write("Warning: query_long() did not return a string.\n");
	 error = 1;
      }
   } else
      if (stringp(night_long)) {
	 if (!stringp(long)) {
	    write("Warning: query(\"day light\") did not return a" +
		  " string.\n");
	    error = 1;
	 } else
	    day_long = long;
      } else {
	 if (!stringp(long)) {
	    write("Warning: query(\"night light\") did not return a" +
		  " string.\n");
	    error = 1;
	 } else
	    night_long = long;
      }
   
   item = ([ ]);
   
   items = (string *)ob->query_id();
   if (pointerp(items))
      for( i=0 ; i<sizeof(items) ; i++) {
	 if (!stringp(items[i])) {
	    write("Warning: query_id() array member " + i +
		  " is not a string.");
	 } else {
	    if (!mapp(item[items[i]]))
	       item[items[i]] = ([ ]);
	    desc = (string)ob->query_item_description(items[i]);
	    if (functionp(desc)) {
	       if(NITPICKY||(SKYDUMMY!=identify(desc)))
		  item[items[i]]["description"] = identify(desc);}
	    else if (!stringp(desc)) {
	       write("Warning: query_item_description() for \"" +
		     items[i] + "\" did not return a string.");
	    } else
	       item[items[i]]["description"] = desc;
	 }
      }
   
   destinations = (string *)ob->query_destinations();
   if (pointerp(destinations))
      for (i = 0; i < sizeof(destinations); i++) {
	 if (!stringp(destinations[i])) {
	    write("Warning: query_destinations() array member " + i +
		  " is not a string.");
	    error = 1;
	 } else {
	    message = catch(load_object(destinations[i]));
	    if (stringp(message)) {
	       write("Warning: could not load " + destinations[i] +
		     ": " + message);
	       error = 1;
	    }
	 }
      }
   
   return !error;
}

void
   print_room(object ob)
{
   int i;
   string *items;
   
   write("User IDs: " + getuid(ob) + "/" + geteuid(ob));
   write("Short: " + short);
   if (stringp(night_long)) {
      write("Night Long: " + night_long);
      write("Day Long: " + day_long + "\n");
   } else
      write("Long: " + long + "\n");
   
   items = keys(item);
   write(sizeof(items) + " Item(s):");
   for (i = 0; i < sizeof(items); i++)
      if (stringp(item[items[i]]["description"]))
	 write(items[i] + ": " + item[items[i]]["description"] + "\n");
}

int
   cmd_roomcheck(string room)
{
   object ob;
   
   if (stringp(room)) {
      catch(ob = load_object(room));
      if (!objectp(ob)) {
	 notify_fail(query_verb() + ": cannot find or load \"" +
		     room + "\".");
	 return 0;
      }
   } else
      ob = environment(this_player());
   
   path = base_name(ob);
   
   write("Room Check for: " + path);
   
   if (check_room(ob))
      print_room(ob);
   
   return 1;
}

int help()
{
  write( @EndText
Syntax: roomcheck [<filename>]
Effect: Balance command to check rooms.
        Checks the basic room properties have been set in room <filename>
        If <filename> is not specified, your current environment is used.
See doc: /doc/QC/check.doc
See also: acheck, wcheck
EndText
  );
return 1;
}
