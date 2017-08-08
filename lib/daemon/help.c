/*    /daemon/help.c
 *    from Nightmare IV
 *    daemon handling mud help functions and help interface
 *    created by Descartes of Borg 940420
 */

#include <std.h>
#include <daemons.h>
#include <dirs.h>
#include "help.h"

inherit DAEMON;

varargs void help(string topic, string category, int menu) {
    if(!topic) topic = "*general";
    else if(topic[0] == '*') help_menu(topic, 0, 0);
    else cmd_help(topic, category, menu);
}

static void help_menu(string category, string *topics, int ind) {
    string tmp;
    int i, maxi, x;

    if(!topics && !(topics = query_topics(category))) {
        message("help", "\nInvalid help category.", this_player());
        message("prompt", "\nSelection: ", this_player());
        input_to("select_topic", category, topics, ind);
        return;
    }
    if((maxi = ind+40) > sizeof(topics)) maxi = sizeof(topics);
    if(!(int)this_player()->query_env("NO_CLEAR"))
    message("info", sprintf("\n%%^INITTERM%%^Category: %%^GREEN%%^"
      "%s%%^RESET%%^    \t%s System Help    \t"
      "Page: %d\n", category, mud_name(), ind/40+1),
      this_player());
    else
    message("info", sprintf("\nCategory: %%^GREEN%%^"
      "%s%%^RESET%%^    \t%s System Help    \t"
      "Page: %d\n", category, mud_name(), ind/40+1),
      this_player());
    i = ind;
    while(i < maxi) {
        if((x = i + 4) >=maxi) x = maxi;
        tmp = "";
        while(i < x) {
            tmp += arrange_string(topics[i], 17);
            i++;
        }
        message("info", tmp, this_player());
    }
    message("info", sprintf("\n%%^CYAN%%^Available categories:\n%s",
      format_page(query_categories(), 3)), this_player());
    message("info", "\nTo choose another category, enter the name of "
      "the category.", this_player());
    message("info", "To view help on any of the above topics, "
      "enter the name of the topic.", this_player());
    message("info", "To see the next page of topics, enter the "
      "<return> key.", this_player());
    message("info", "To quit from help, enter \"q\" alone.",
      this_player());
    message("prompt", "Selection: ", this_player());
    input_to("select_topic", category, topics, ind);
}

static void select_topic(string str, string category, string *topics,int ind) {
    if(str == "") {
        if(ind+40 < sizeof(topics)) help_menu(category, topics, ind+40);
        else help_menu(category, topics, ind);
        return;
      }
    else if(str == "q") {
        message("help", "\nExit from help.", this_player());
        return;
      }
    else help(str, category, 1);
  }

static string *query_categories() {
    string *tmp;

    tmp = ({ "*player general", "*player commands", "*abilities" });
    if((string)this_player()->query_guild()) tmp += ({ "*guild commands" });
    if(high_mortalp(this_player()) || creatorp(this_player()))
      tmp += ({ "*high mortal general", "*high mortal commands" });
    if(ambassadorp(this_player()))
      tmp += ({ "*ambassador general", "*ambassador commands" });
    if(creatorp(this_player())) 
      tmp += ({ "*creator general", "*creator commands" });
    if(archp(this_player())) tmp += ({ "*admin commands" }); 
    return tmp;
  }

static string *query_topics(string category) {
    string tmp;

    switch(category) {
    case "*player general": return get_dir(DIR_USER_HELP+"/");
    case "*creator general":
        if(!creatorp(this_player())) return 0;
        else return get_dir(DIR_CREATOR_HELP+"/");
    case "*ambassador general":
        if(!creatorp(this_player()) && !archp(this_player())) return 0;
        else return get_dir(DIR_AMBASSADOR_HELP+"/");
    case "*high mortal general":
        if(!creatorp(this_player()) && !high_mortalp(this_player())) return 0;
        else return get_dir(DIR_HM_HELP+"/");
    case "*player commands":
        return (string *)CMD_D->query_commands(DIR_MORTAL_CMDS) +
          (string *)CMD_D->query_commands(DIR_SECURE_MORTAL_CMDS);
    case "*abilities":
        return (string *)CMD_D->query_commands(DIR_CLASS_CMDS);
    case "*guild commands":
        if(!(tmp = (string)this_player()->query_guild())) return 0;
        else return (string *)CMD_D->query_commands(DIR_GUILD_CMDS+"/"+tmp);
    case "*creator commands":
        if(!creatorp(this_player())) return 0;
        return (string *)CMD_D->query_commands(DIR_CREATOR_CMDS) +
          (string *)CMD_D->query_commands(DIR_SYSTEM_CMDS) +
          (string *)CMD_D->query_commands(DIR_AMBASSADOR_CMDS) +
          (string *)CMD_D->query_commands(DIR_SECURE_CREATOR_CMDS) +
          (string *)CMD_D->query_commands(DIR_SECURE_AMBASSADOR_CMDS);
    case "*high mortal commands":
        if(!creatorp(this_player()) && !high_mortalp(this_player())) return 0;
        return (string *)CMD_D->query_commands(DIR_HM_CMDS);
    case "*ambassador commands":
        if(!creatorp(this_player()) && !ambassadorp(this_player())) return 0;
        return (string *)CMD_D->query_commands(DIR_AMBASSADOR_CMDS) +
          (string *)CMD_D->query_commands(DIR_SYSTEM_CMDS) +
          (string *)CMD_D->query_commands(DIR_SECURE_AMBASSADOR_CMDS);
    case "*admin commands":
        if(!archp(this_player())) return 0;
        else return (string *)CMD_D->query_commands(DIR_ADMIN_CMDS) +
          (string *)CMD_D->query_commands(DIR_SECURE_ADMIN_CMDS);
    default: return 0;
    }
  }

void cmd_help(string topic, string category, int menu) {
    object ob;
    string *tmp;
    int i;

    if(category) {
        if(member_array(category, query_categories()) == -1) {
            message("help", sprintf("Category %s not available.",
              category), this_player());
            if(menu) {
                message("prompt", "\nHit <return> to continue: ", 
                  this_player());
                input_to("menu_return", category);
	      }
	  }
        else if(!find_help(topic, category, menu)) {
            message("help", sprintf("The help topic %s does not exist in "
              "the category %s.", topic, category), this_player());
            if(menu) {
                message("prompt", "\nHit <return> to continue: ",
                  this_player());
                input_to("menu_return", category);
	      }
	  }
        return;
      }
    else {
        i = sizeof(tmp = query_categories());
        while(i--) if(find_help(topic, tmp[i], menu)) return;
        if((ob = present(topic, this_player())) &&
          function_exists("help", ob)) {
            ob->help();
            return;
	  }
        else if((ob = present(topic, environment(this_player()))) &&
          function_exists("help", ob)) {
            ob->help();
            return;
	  }
        message("help", sprintf("No help available for the topic %s.",
          topic), this_player());
        if(menu) {
            message("prompt", "\nHit <return> to continue: ", this_player());
            input_to("menu_return", category);
	  }
        return;
      }
  }

static int find_help(string topic, string category, int menu) {
    object ob;
    string str;
    mixed tmp;

    switch(category) {
    case "*player general":
        if(!file_exists(tmp = DIR_USER_HELP+"/"+topic)) return 0;
        break;
    case "*player commands":
        if(!file_exists(tmp = DIR_MORTAL_CMDS+"/_"+topic+".c") &&
          !file_exists(tmp = DIR_SECURE_MORTAL_CMDS+"/_"+topic+".c")) return 0;
        if(!(ob = load_object(tmp)) || !function_exists("help", ob))
          return 0;
        tmp = ob;
        break;
    case "*abilities":
        if(!file_exists(tmp = DIR_CLASS_CMDS+"/_"+topic+".c")) return 0;
        if(!(ob = load_object(tmp)) || !function_exists("help", ob))
          return 0;
        tmp = ob;
        break;
    case "*creator general":
        if(!file_exists(tmp = DIR_CREATOR_HELP+"/"+topic)) return 0;
        break;
    case "*creator commands":
        if(file_exists(tmp = DIR_CREATOR_CMDS+"/_"+topic+".c") &&
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;
        else if(file_exists(tmp = DIR_SYSTEM_CMDS+"/_"+topic+".c") &&
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;
        else if(file_exists(tmp = DIR_AMBASSADOR_CMDS+"/_"+topic+".c") &&
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;
        else if(file_exists(tmp = DIR_SECURE_AMBASSADOR_CMDS+"/_"+topic+".c") &&
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;
        else if(file_exists(tmp = DIR_SECURE_CREATOR_CMDS+"/_"+topic+".c") &&
         (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;     
        else return 0;
        break;
    case "*ambassador general":
        if(file_exists(tmp = DIR_AMBASSADOR_CMDS+"/_"+topic+".c") &&       
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;     
        else if(file_exists(tmp = DIR_SECURE_AMBASSADOR_CMDS+"/_"+topic+".c") &&
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;     
        else return 0;
        break; 
    case "*ambassador commands":
        if(file_exists(tmp = DIR_AMBASSADOR_CMDS+"/_"+topic+".c") &&
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;
        else if(file_exists(tmp = DIR_SYSTEM_CMDS+"/_"+topic+".c") &&
          (ob = load_object(tmp)) && function_exists("help", ob)) tmp = ob;
        else return 0;
        break;
    case "*high mortal general":
        if(!file_exists(tmp = DIR_HM_HELP+"/"+topic)) return 0;
        break;
    case "*high mortal commands":  
        if(!file_exists(tmp = DIR_HM_CMDS+"/_"+topic+".c")) return 0;
        if(!(ob = load_object(tmp)) || !function_exists("help", ob))
          return 0;
        tmp = ob;
        break;
    case "*guild commands":
        if(!file_exists(tmp = sprintf("%s/%s/_%s.c", DIR_GUILD_CMDS,
          (string)this_player()->query_guild(), topic))) return 0;
        if(!(ob = load_object(tmp)) || !function_exists("help", ob))
          return 0;
        tmp = ob;
        break;
    case "*admin commands":
        if(!archp(this_player())) return 0;
        if(!file_exists(tmp = DIR_ADMIN_CMDS+"/_"+topic+".c") &&
          !file_exists(tmp = DIR_SECURE_ADMIN_CMDS+"/_"+topic+".c")) return 0;
        if(!(ob = load_object(tmp)) || !function_exists("help", ob))
          return 0;
        tmp = ob;
        break; 
      }
    if(!(int)this_player()->query_env("NO_CLEAR"))
    message("info", sprintf("\n%%^INITTERM%%^Topic: %%^GREEN%%^%s"
      "%%^RESET%%^  \t%s System Help \tCategory: %%^GREEN%%^%s\n", 
      topic, mud_name(), category), this_player());
    else
    message("info", sprintf("\nTopic: %%^GREEN%%^%s"
      "%%^RESET%%^  \t%s System Help \tCategory: %%^GREEN%%^%s\n",
      topic, mud_name(), category), this_player());
    if(objectp(tmp)) {
        tmp->help();
        if(menu) {
            message("prompt", "\nHit <return> to continue: ", this_player());
            input_to("menu_return", category);
        }
    }
    else this_player()->more(tmp, "help", (menu ? (: "endmore" :) : 0),
      (menu ? category : 0));
    return 1;
  }

static void menu_return(string duh, string category) {
    if(duh != "q") help(category);
}

void endmore(string category) {
    message("prompt", "\nHit <return> to continue: ", this_player());
    input_to("menu_return", category);
}
