/*    /std/user/nmsh.c
 *    from Nightmare IV
 *    the new Nightmare shell
 *    created by Descartes of Borg 940216
 */

#include <config.h> 
#include "nmsh.h"
 
#define DIRECTORY_STACK_SIZE     5
#define MAX_CMD_ALIASES          60
#define MIN_HISTORY_SIZE         10

private string __CurrentWorkingDirectory;
private mapping __Nicknames, __Aliases, __Xverbs; 
static private int __CWDCount, __CWDBottom, __CWDTop, __CmdNumber; 
static private int __HistoryCount, __HistoryBottom, __HistoryTop, __HistorySize; 
static private string __Prompt; 
static private string *__Stack, *__History; 
 
void create() { 
    __Nicknames = ([]); 
    __Aliases = ([ "l" : "look $*", "bio" : "biography", "i" : "inventory",
      "n" : "go north", "s" : "go south", "e" : "go east", "w" : "go west",
      "ne" : "go northeast", "nw" : "go northwest", "se" : "go southeast",
      "sw" : "go southwest", "d" : "go down", "u" : "go up", "exa" : "look at $*",
    ]);
    __Xverbs = ([ "'" : "say $*", ":" : "emote $*" ]); 
 } 
 
void setup() { 
    if(this_player() != this_object()) return; 
    reset_history(); 
    reset_prompt(); 
    if(!__Nicknames) __Nicknames = ([]);
    if(!__Aliases) __Aliases = ([]);
    if(!__Xverbs) __Xverbs = ([]);
    add_action("cmd_alias", "alias"); 
    add_action("cmd_history", "history");
    add_action("cmd_nickname", "nickname"); 
    if(creatorp(this_object())) { 
        __Stack = allocate(DIRECTORY_STACK_SIZE); 
        __CWDBottom = __CWDTop = __CWDCount = 0; 
        add_action("cmd_cd", "cd"); 
        add_action("cmd_nmsh", "nmsh"); 
        add_action("cmd_pushd", "pushd"); 
        add_action("cmd_popd", "popd"); 
        add_action("cmd_pwd", "cwd");
        add_action("cmd_pwd", "pwd");
        add_action("cmd_work", "work");
    } 
} 
 
nomask static int cmd_alias(string str) { 
    string *a, *b;
    string key, thing; 
    int i;
 
    if(this_player() != this_object()) return 0; 
    if(!str) {
        i = sizeof(a = keys(__Aliases));
        while(i--)
          a[i] = sprintf("%s%s", arrange_string(a[i],15),__Aliases[a[i]]);
        i = sizeof(b = keys(__Xverbs));
        while(i--)
          b[i] = sprintf("$%s%s", arrange_string(b[i],14), __Xverbs[b[i]]);
        this_player()->more(a+b);
        return 1;
    }
    if(sscanf(str, "%s %s", key, thing) != 2) { 
        if(str[0] == '$') { 
            str = str[1..strlen(str)-1]; 
            if(__Xverbs[str]) { 
                map_delete(__Xverbs, str); 
                message("system", sprintf("Alias $%s removed.", str), this_player()); 
            } 
            else message("system", sprintf("No such alias $%s.", str), this_player()); 
            return 1; 
        } 
        if(__Aliases[str]) { 
            map_delete(__Aliases, str); 
            message("system", sprintf("Alias %s removed.", str), this_player()); 
        } 
        else message("system", sprintf("No such alias %s.", str), this_player()); 
        return 1; 
    } 
    if(sizeof(__Xverbs) + sizeof(__Aliases) >= MAX_CMD_ALIASES) { 
        message("system", "You must remove an alias before adding another.", 
          this_player()); 
        return 1; 
    } 
    if(key == "alias") return notify_fail("You are a bonehead.\n");
    if(key[0] == '$') { 
        key = key[1..strlen(key)]; 
        if(__Xverbs[key])  
          message("system", sprintf("Alias for $%s altered to (%s).", 
            key, thing), this_player()); 
        else message("system", sprintf("Alias $%s (%s) added.", key, thing), 
          this_player());      
        __Xverbs[key] = thing; 
    } 
    else { 
        if(__Aliases[key]) 
          message("system", sprintf("Alias for %s altered to (%s).", key, thing), 
            this_player()); 
        else message("system", sprintf("Alias %s (%s) added.", key, thing),this_player()); 
        __Aliases[key] = thing; 
    } 
    return 1; 
} 
 
nomask static int cmd_cd(string str) { 
    if(this_player() != this_object()) return 0; 
    set_cwd(str); 
    return 1; 
} 
 
nomask static int cmd_history(string str) {
    string *cmds;
    int i, maxi, x;

    if(__CmdNumber <= __HistorySize) {
        for(i=0; i<__CmdNumber; i++) 
          message("system", sprintf("%d%s",i+1,
            ((i+1) > 9 ? " "+__History[i] : "  "+__History[i])),
            this_player());
        return 1;
    }
    maxi = sizeof(cmds = __History[__HistoryBottom..__HistorySize-1] +
      __History[0..__HistoryTop]);
    for(i=0; i<maxi; i++) {
        x = __CmdNumber-__HistorySize+i+1;
        message("system", sprintf("%d %s", x, cmds[i]), this_player());
    }
    return 1;
}

nomask static int cmd_nickname(string str) { 
    string *cles;
    string key, thing; 
    int i;
 
    if(this_player() != this_object()) return 0;
    if(!str) {
        i = sizeof(cles = keys(__Nicknames));
        while(i--)
          cles[i] = sprintf("%s%s", arrange_string(cles[i], 15),
            __Nicknames[cles[i]]);
        this_player()->more(cles);
        return 1;
    }
    if(sscanf(str, "%s %s", key, thing) != 2) { 
        if(__Nicknames[str]) { 
            message("system", sprintf("Nickname %s removed.", str), 
              this_player()); 
            map_delete(__Nicknames, str); 
        } 
        else message("system", sprintf("No such nickname %s.", str), 
          this_player()); 
    } 
    else { 
        if(__Nicknames[key])  
          message("system", sprintf("Nickname %s altered to (%s).", key, thing), 
            this_player()); 
        else message("system", sprintf("Nickname %s (%s) added.", key, thing), 
          this_player()); 
        __Nicknames[key] = thing; 
    } 
    return 1; 
} 
 
nomask static int cmd_nmsh(string str) { 
    string *lines; 
    string tmp;
    int i, maxi; 
 
    if(!str) return 0; 
    if(this_player() != this_object()) return 0; 
    if((int)this_player()->query_forced()) return 0; 
    if(!(tmp = read_file(absolute_path(query_cwd(), str)))) 
      return notify_fail(sprintf("nmsh: script %s not found.\n")); 
    maxi = sizeof(lines = explode(tmp, "\n")); 
    for(i=0; i < maxi; i++) { 
        if(lines[i][0] == '#') continue; 
        if(!command(lines[i])) { 
            message("system", sprintf("nmsh: error in executing %s.", str), 
              this_player()); 
            return 1; 
        } 
    } 
    return 1; 
} 
 
nomask static int cmd_pushd(string str) { 
    if(this_player() != this_object()) return 0; 
    if(!set_cwd(str)) return 0; 
    pushd(str); 
    return 1; 
} 
 
nomask static int cmd_popd(string str) { 
    if(this_player() != this_object()) return 0; 
    set_cwd(popd()); 
    return 1; 
} 
 
nomask static int cmd_pwd(string str) {
    if(!query_cwd()) message("system", "No current directory.", this_object());
    else message("system", query_cwd()+":", this_object());
    return 1;
}

nomask static int cmd_work(string str) {
    string *tmp;
    object ob;
    string file;
    int flag;

    if(!str || str == "") ob = environment(this_object());
    else if(str == "!") {
        flag = 1;
        ob = environment(this_object());
    }
    else if(str[0] == '!') {
        flag = 1;
        str = str[1..strlen(str)];
    }
    if(!ob && !(ob = present(str, this_object())))
      ob = present(str, environment(this_object()));
    if(!ob) return notify_fail("No target object found.\n");
    tmp = explode(file = base_name(ob), "/");
    set_cwd("/"+implode(tmp[0..sizeof(tmp)-2], "/"));
    if(flag) {
        message("system", file+".c, "+file_size(file+".c")+" bytes:",
          this_object());
        ed(file+".c");
    }
    return 1;
}

nomask string write_prompt() { 
    string tmp, ret; 
    int x;
 
    if((ret = __Prompt) == DEFAULT_PROMPT) {
        message("prompt", ret, this_object());
        return ret;
    }
    while((x = strsrch(ret, "$")) != -1) {
        if(x == strlen(ret) -1) break;
        switch(ret[x+1]) {
            case 'D': 
                if(!creatorp(this_object())) break;
                if(sscanf(query_cwd(), user_path(query_name())+"%s",
                  tmp)) tmp = "~"+tmp;
                else tmp = query_cwd();
                ret = replace_string(ret, "$D", tmp); 
                break;
            case 'V': case 'v':
                if(query_invis()) {
                    ret = replace_string(ret, "$V", "INVIS"); 
                    ret = replace_string(ret, "$v", "invis"); 
                } 
                else if(hiddenp(this_object())) { 
                    ret = replace_string(ret, "$V", "HID"); 
                    ret = replace_string(ret, "$v", "hid"); 
                } 
                else { 
                    ret = replace_string(ret, "$V", ""); 
                    ret = replace_string(ret, "$v", ""); 
                } 
                break;
            case 'C':
                ret = replace_string(ret, "$C", sprintf("%d", __CmdNumber+1)); 
                break;
            case 'H':
                ret = replace_string(ret, "$H", sprintf("%d", query_max_hp())); 
                break;
            case 'h':
                ret = replace_string(ret, "$h", sprintf("%d", query_hp())); 
                break;
            case 'G':
                ret = replace_string(ret, "$G", sprintf("%d", query_max_mp())); 
                break;
            case 'g':
                ret = replace_string(ret, "$g", sprintf("%d", query_mp())); 
                break;
            case 'I':
                ret = replace_string(ret, "$I", sprintf("%d", query_max_sp())); 
                break;
           case 'i':
                ret = replace_string(ret, "$i", sprintf("%d", query_sp())); 
                break;
            default:
                ret = replace_string(ret, ret[x..x+1], "");
                break;
        }
    }
    message("prompt", ret, this_object());
    return ret;
} 
 
nomask string process_input(string str) { 
    string tmp, xtra, request; 
 
    if(!str || str == "") return ""; 
    else if(query_client() &&
      member_array(query_client(), SUPPORTED_CLIENTS) != -1) {
        if(sscanf(str, "<%s>%s", request, xtra)) {
            process_request(request, xtra ? xtra : "");
            return "";
        }
        else return str;
    }
    else if((tmp = do_history(str)) == "") return ""; 
    if(tmp != str) message("system", tmp, this_object());
    add_history_cmd(tmp); 
    return do_alias(do_nickname(tmp));
} 
 
nomask static void process_request(string request, string xtra) {
    switch(request) {
        case "ALIAS":
            receive("<ALIAS>[n,go north] [s,go south] [e,go east] [w,go west] "
              "[nw,go northwest] [ne,go northeast] [sw,go southwest] "
              "[se,go southeast] [u,go up] [d,go down] [i,inventory] "
              "[bio,biography] [exa,look at $*] [$',say $*] "
              "[$:,emote $*] [l,look]\n");
            break;
        case "NICKNAME": receive("<NICKNAME>\n"); break;
        case "USERS":
            receive("<USERS>"+implode(map_array(filter_array(users(), 
              "request_vis", this_object()), "user_names", this_object()), 
              ", ")+"\n");
            break;
        case "ROOM":
            receive("<ROOM>"+
              (string)environment(this_object())->query_short()+"\n");
            break;
	case "PRESENT":
            receive("<PRESENT>"+
              implode(map_array(filter_array(all_inventory(environment(this_object())),
              "request_vis", this_object()), "user_names", this_object()),
              ", ")+"\n");
            break;
        default:
            receive("<error>Request not supported.\n");
            break;
	}
}

static int request_vis(object ob) {
    return (userp(ob) && !((int)ob->query_invis(this_object())));
  }

static string user_names(object ob) {
    return (string)ob->query_cap_name();
  }

static private int set_cwd(string str) { 
    int x;

    if(!str || str == "") str = user_path(query_name()); 
    if(!str || str == "") str = "/"; 
    if(sizeof(str) > 1 && str[strlen(str)-1] == '/') str = str[0..strlen(str)-2]; 
    str = replace_string(str, "//", "/"); 
    str = (absolute_path(query_cwd(), str) || str);
    str = replace_string(str, "//", "/");
    if((x=file_size(str)) != -2) { 
        if(x > -1) { 
            message("system", sprintf("%s: Path is a file.", str), this_player()); 
            return 0; 
        } 
        else { 
            message("system", sprintf("%s: No such reference.", str), this_player()); 
            return 0; 
        }  
    } 
    __CurrentWorkingDirectory = str; 
    message("system", sprintf("%s:", __CurrentWorkingDirectory), this_player()); 
    return 1; 
} 
 
static private void pushd(string str) { 
    if(__CWDCount++ == DIRECTORY_STACK_SIZE) { 
        __CWDCount--; 
        __CWDBottom = (++__CWDBottom) % DIRECTORY_STACK_SIZE; 
    } 
    __Stack[__CWDTop] = str; 
    __CWDTop = (++__CWDTop) % DIRECTORY_STACK_SIZE; 
} 
 
static private string popd() { 
    if(!__CWDCount) return 0; 
    __CWDCount--; 
    return __Stack[--__CWDTop]; 
} 
 
nomask static private string do_nickname(string str) { 
    if(!__Nicknames) return str; 
    if(str[0..7] == "nickname") return str; 
    return implode(map_array(explode(str, " "), "replace_nickname", this_object()), " "); 
} 
 
nomask static private string do_alias(string str) { 
    string *words; 
    string tmp; 
    int x, i; 
 
    if(!sizeof(words = explode(str, " "))) return "";
    if((x = strlen(words[0])) && (tmp = __Xverbs[words[0][0..0]])) {
        words[0] = words[0][1..x-1];
        return replace_string(tmp, "$*", implode(words, " "));
    }
    if(!(tmp = __Aliases[words[0]])) return implode(words, " "); 
    else str = implode(words[1..sizeof(words)-1], " "); 
    return replace_string(tmp, "$*", str); 
 
} 
 
nomask static private string do_history(string str) { 
    string *args;
    string tmp, cmd, expr;
    int x, slen, i; 
 
    if(str[0] != '!') return str; 
    if(str == "!!" || str == "!-1") {
        if(__CmdNumber == 0) { 
            message("system", "Try entering a command first.", this_player()); 
            return ""; 
        } 
        else return __History[(__CmdNumber-1) % __HistorySize]; 
    } 
    if(sscanf(str, "%ss/%s", cmd, expr) == 2) { 
        if((tmp = do_history(cmd)) == "") return ""; 
        switch(sizeof(args = explode(expr, "/"))) { 
        case 2: 
            if((x = strsrch(tmp, args[0])) == -1) { 
                message("system", "nmsh: History expression not found.", 
                  this_player()); 
                return ""; 
            } 
            else if(!x)
              return sprintf("%s%s",args[1],tmp[strlen(args[0])..strlen(tmp)-1]);
            else return sprintf("%s%s%s", tmp[0..x-1], args[1], 
              tmp[x+strlen(args[0])..strlen(tmp)-1]); 
        case 3: 
            if(args[2] != "g") { 
                message("system", "nmsh: History syntax error.", 
                  this_player()); 
                return ""; 
            } 
            else return replace_string(tmp, args[0], args[1]); 
        default: 
            message("system", "nmsh: History syntax error.", this_player()); 
            return ""; 
        } 
    } 
    if(sscanf(str, "!%d", x) == 1) { 
        if(x < 1) x = 1+ __CmdNumber + x;
        if(x < 1 || x >= 1 + __CmdNumber) { 
            message("system", sprintf("Event %d not found.", x), 
              this_player()); 
            return ""; 
        } 
        if(__CmdNumber - __HistorySize > x) { 
            message("system", sprintf("Event %d is lost from the queue.", x), 
              this_object()); 
            return ""; 
        } 
        else return __History[(x-1) % __HistorySize]; 
    } 
    else { 
        if(__CmdNumber <= __HistorySize)
          i=sizeof(args=map_array(__History[0..__CmdNumber-1],"replace_null",
            this_object()));
        else i=sizeof(args=map_array(__History[__HistoryBottom..__HistorySize-1]+
          __History[0..__HistoryTop], "replace_null", this_object()));
        while(i--) if(!strsrch(args[i], str[1..strlen(str)-1])) return args[i];
        return "";
    } 
} 
 
nomask static string replace_null(string str) { 
    if(!str) return ""; 
    else return str; 
} 
         
nomask static private void add_history_cmd(string str) { 
    __History[__HistoryTop = (++__CmdNumber-1) % __HistorySize] = str;
    if(__CmdNumber < __HistorySize) __HistoryBottom = 0;
    else if(!__HistoryTop) __HistoryBottom = __HistorySize - 1;
    else __HistoryBottom = __HistoryTop + 1;
}
             
nomask static string replace_nickname(string str) { 
    if(str == "") return str; 
    if(str[0] == '\\') return str[1..(strlen(str)-1)]; 
    else if(__Nicknames[str]) return __Nicknames[str]; 
    else return str; 
} 
 
void reset_history() { 
    __CmdNumber = 0; 
    __HistoryCount = __HistoryBottom = __HistoryTop = 0; 
    __History = allocate(__HistorySize = query_history_size()); 
} 
 
void reset_prompt() { 
    __Prompt = query_prompt(); 
    if(!stringp(__Prompt)) __Prompt = "Prompt screwey> ";
    __Prompt = replace_string(__Prompt, "$M", mud_name()); 
    __Prompt = replace_string(__Prompt, "$m", lower_case(mud_name())); 
    __Prompt = replace_string(__Prompt, "$N", (string)this_object()->query_CapName()); 
    __Prompt = replace_string(__Prompt, "$n", query_name());
} 
 
string query_cwd() { return __CurrentWorkingDirectory; } 
 
int query_history_size() { return MIN_HISTORY_SIZE; } 
     
string query_prompt() { return DEFAULT_PROMPT; }

int query_mp() { return 1; } 
 
int query_max_mp() { return 10; } 
 
int query_hp() { return 1; } 
 
int query_max_hp() { return 10; } 
 
int query_sp() { return 1; } 
 
int query_max_sp() { return 10; } 

string get_path() { return query_cwd(); }

varargs int query_invis(object ob) { return 0; }

string query_name() { return 0; }
