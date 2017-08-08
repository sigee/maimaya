/*    /cmds/system/_man.c
 *    from Nightmare Mudlib 3.2
 *    a command to access the man documents for efuns and such
 *    created by Descartes of Borg 940122
 */

#include <std.h>
#include <dirs.h>

inherit DAEMON;

#define MAN_PAGES ({ "SimulEfun", "efun", "applies", "lfun" })

int cmd_man(string str) {
    string d, tmp;
    string *pages;
    int i;

    if(!str) return notify_fail("Usage: <man [function]>\n");
    i = sizeof(MAN_PAGES);
    pages = ({});
    while(i--) 
      if(file_exists(tmp = sprintf("%s/%s/%s", DIR_DOCS, MAN_PAGES[i],
        str))) pages += ({ tmp });
    if(!(i = sizeof(pages))) return notify_fail("No such man page.\n");
    else if(i > 1) 
      message("system", "Showing only the first of "+i+" man pages.",
        this_player());
    this_player()->more(pages[0]);
    return 1;
  }
