/*    /adm/obj/master.c
 *    from Nightmare 3.3
 *    the master object, responsible for security
 *    created by Descartes of Borg 940910
 *    error handling by Beek@The Idea Exchange 941004
 */

#include <config.h>
#include <objects.h>
#include <rooms.h>
#include <cfg.h>
#include <privs.h>
#include <dirs.h>
#include <save.h>
#include <daemons.h>
#include "master.h"

static private object __Unguarded;
static private string __PlayerName;
static private object __NewPlayer;
static private mapping __Groups, __ReadAccess, __WriteAccess;

void create() {
    __Unguarded = 0;
    __NewPlayer = 0;
    __PlayerName = 0;
    new_read();
    new_write();
    new_groups();
}

void new_read() {
    mapping tmp;

    tmp = ([]);
    load_access(CFG_READ, tmp);
    __ReadAccess = tmp;
}

void new_write() {
    mapping tmp;

    tmp = ([]);
    load_access(CFG_WRITE, tmp);
    __WriteAccess = tmp;
}

void new_groups() {
    mapping tmp;

    tmp = ([]);
    load_access(CFG_GROUPS, tmp);
    __Groups = tmp;
}

static private void load_access(string cfg, mapping ref) {
    string *lines;
    string file, fl, ac;
    int i, maxi;

    if(!(file = read_file(cfg)))
      error("Failed to find config file: "+cfg);
    maxi = sizeof(lines = explode(file, "\n"));
    for(i=0; i<maxi; i++) {
        if(!lines[i] || lines[i] == "" || lines[i][0] == '#') continue;
        if(sscanf(lines[i], "(%s) %s", fl, ac) != 2)
          error("Error in loading config file "+cfg+" at line "+
            (i+1));
        ref[fl] = explode(ac, ":");
    }
}

void flag(string str) {
    string file, arg;
    int i, x;

    if(previous_object()) return;
    if(sscanf(str, "for %d", x) == 1) {
        for(i=0; i<x; i++) {}
        return;
    }
    if(sscanf(str, "call %s %s", file, arg)) {
        write("Got "+(string)call_other(file, arg)+" back.\n");
        return;
    }
    write("Master: unknown flag.\n");
}

string *epilog(int x) {
    string *lines, *files;
    string content;
    int i;

    call_out("socket_preload", 5);
    if(!(content = read_file(CFG_PRELOAD))) return ({});
    i = sizeof(lines = explode(content, "\n"));
    files = ({});
    while(i--) {
        if(!lines[i] || lines[i] == "" || lines[i][0] == '#') continue;
        files += ({ lines[i] });
    }
    return files;
}

string privs_file(string file) {
    string base;
    int ext;

    sscanf(file, "%s#%d", base, ext);
    if("/"+base == OB_USER) {
        string *grps;
        string str;
        int i;

        if(!__PlayerName) return 0;
        str = __PlayerName;
        i = sizeof(grps = keys(__Groups));
        while(i--)
          if(member_array(__PlayerName, __Groups[grps[i]]) != -1)
            str = str + ":" + grps[i];
        return str;
    }
    return file_privs(file);
}

void preload(string str) {
    string err;
    int t;

    if(!file_exists(str+".c")) return;
    t = time();
    write("Preloading: "+str+"...");
    if(err=catch(call_other(str, "???")))
      write("\nGot error "+err+" when loading "+str+".\n");
    else {
        t = time() - t;
        write("("+(t/60)+"."+(t%60)+")\n");
    }
}

void socket_preload() {
    string *items;
    int i, max;

    max = sizeof(items=explode(read_file(CFG_PRELOAD_SOCKET), "\n"));
    for(i=0; i<max; i++) {
        if(!items[i] || items[i] == "" || items[i][0] == '#') continue;
        catch(call_other(items[i], "???"));
    }
}

int valid_write(string file, object ob, string fun) {
    string *ok;

    if(ob == master()) return 1;
    if(file[0] != '/') file = "/"+file;
    if(!(ok = match_path(__WriteAccess, file))) ok = 0;
    if(check_access(ob, fun, file, ok, "write")) return 1;
    log_file("access", "Write access denied: "+file+" : "+
      identify(previous_object(-1))+" : "+fun+"\n");
    return 0;
}

int valid_read(string file, object ob, string fun) {
    string *ok;
    if(ob == master()) return 1;
    if(file[0] != '/') file = "/"+file;
    if(!(ok = match_path(__ReadAccess, file))) ok = 0;

    //tc("\n-------");
    //tc("valid_read("+identify(file)+", "+identify(ob)+", "+identify(fun)+")");
    //tc("match_path(__ReadAccess, "+identify(file)+"): "+identify(ok));
    //tc("check_access: "+check_access(ob, fun, file, ok, "read"));
    //tc("-----------\n");

    if(check_access(ob, fun, file, ok, "read")) return 1;
    log_file("access", "Read access denied: "+file+" : "+
      identify(previous_object(-1))+" : "+fun+"\n");
    return 0;
}

int valid_apply(string *ok) {
    return check_access(previous_object(1),0,previous_object(0), ok, "apply");
}

int check_access(object ob, string fun, mixed file, string *ok, string oper) {
    object *stack;
    string *privs;
    string priv;
    int i;

    if(objectp(file)) file = base_name(file);
    if(ok && sizeof(ok) && ok[0] == "all") return 1;
    if(__Unguarded == ob) {
        string tmp;

        if((tmp = base_name(ob)) == OB_USER) {
            //debug_message("master(): checking "+file+" access for "+__PlayerName);
            if(!__PlayerName) i = sizeof(stack = ({ob})+previous_object(-1));
            else if(file == DIR_USERS+"/"+__PlayerName[0..0]+"/"+__PlayerName+__SAVE_EXTENSION__)
              return 1;
            else i = sizeof(stack = ({ ob }));
        }
        else if(tmp == file) return 1;
        else i = sizeof(stack = ({ ob }));
    }
    else if(__Unguarded && base_name(ob) == OB_SIMUL_EFUN) {
        if(__Unguarded == previous_object(1))
          stack = ({ previous_object(1) });
        else stack = ({ ob }) + previous_object(-1);
    }
    else i = sizeof(stack = previous_object(-1) + ({ ob }));
    while(i--) {
        if(!stack[i] || stack[i] == this_object()) continue;
        if(file_name(stack[i]) == OB_SIMUL_EFUN) continue;
        if(!(priv = query_privs(stack[i]))) return 0;
        if(!ok && oper == "read") continue;
        privs = explode(priv, ":");
        if(member_array(PRIV_SECURE, privs) != -1) continue;
        if(member_array(file_privs(file), privs) != -1) continue;
        if(!ok && oper == "write") {
            if(userp(stack[i]) && check_user(stack[i], fun, file, oper))
              continue;
            else return 0;
        }
        if(sizeof(privs & ok)) continue;
        if(userp(stack[i]) && check_user(stack[i], fun, file, oper)) continue;
        if(userp(stack[i]) && check_domain(stack[i], fun, file,oper)) continue;
        return 0;
    }
    return 1;
}

nomask static int check_user(object ob, string fun, string file, string oper) {
    string nom;
    int x;

    if(!sscanf(file, REALMS_DIRS+"/%s/%*s", nom)) return 0;
    nom = user_path(nom)+"adm/access";
    if(file_size(nom+".c") < 0) return 0;
    catch(x = (int)call_other(nom, "check_access", ob, fun, file, oper));
    return x;
}

nomask static int check_domain(object ob, string fun, string file, string o) {
    string nom;
    int x;

    if(!sscanf(file, DOMAINS_DIRS+"/%s/%*s", nom)) return 0;
    nom = DOMAINS_DIRS+"/"+nom+"/adm/access";
    if(file_size(nom+".c") < 0) return 0;
    catch(x = (int)call_other(nom, "check_access", ob, fun, file, o));
    return x;
}

object connect() {
    object ob;
    string err;

    if(err=catch(ob = clone_object(OB_LOGIN))) {
        write("It looks like someone is working on the user object.\n");
        write(err);
        destruct(ob);
    }
    return ob;
}

object compile_object(string str) {
    string nom, tmp, where, which;
    object ob;
    //debug_message("master(): compile_object(\""+str+"\")");
    if(sscanf(str, REALMS_DIRS+"/%s/%*s", nom)){
      //debug_message("alpha");
      tmp = sprintf("%svirtual/server", user_path(nom));
    }
    else if(sscanf(str, DOMAINS_DIRS+"/%s/%*s", nom)){
      //debug_message("bravo");
      tmp = sprintf("%s/%s/virtual/server", DOMAINS_DIRS, nom);
    }
    else if(strsrch(str, ESTATES_DIRS) == 0){
      //debug_message("charlie");
      tmp = sprintf("%s/adm/server", ESTATES_DIRS);
    }
    else if(sscanf(str, DIR_USERS+"/%*s/%s", nom)) {
        //debug_message("yoohoo!");
        if(!__NewPlayer) return 0;
        //debug_message("over here!");
        if((string)__NewPlayer->query_name() != nom) return 0;
        //debug_message("hi sailor!");
        __PlayerName = nom;
        ob = new(OB_USER);
        //debug_message("master() str: "+str);
        if(file_exists(str+__SAVE_EXTENSION__)) ob->restore_player(nom);
        else if(file_size(DIR_USERS) != -2) mkdir(DIR_USERS);
        else if(file_size(DIR_USERS+"/"+nom[0..0]) != -2)
          mkdir(DIR_USERS+"/"+nom[0..0]);
        ob->set_name(nom);
        __PlayerName = 0;
        //debug_message("master() ob: "+identify(ob));
        return ob;
    }
    else {
       //debug_message("delta");
    }
    if(file_size(tmp+",c") < 0) {
        if(sscanf(str, "%s.%s", where, which) != 2) return 0;
        if(sscanf(str, REALMS_DIRS+"/%s/%*s", nom))
          tmp = sprintf("%svirtual/%s_server", user_path(nom), which);
        else if(sscanf(str, DOMAINS_DIRS+"/%s/%*s", nom))
          tmp = sprintf("%s/%s/virtual/%s_server", DOMAINS_DIRS, nom, which);
        if(file_size(tmp+".c") < 0) return 0;
        else return (object)call_other(tmp, "compile_object", where);
    }
    return (object)call_other(tmp, "compile_object", str);                      
}

static void crash(string err) {
    write_file(DIR_LOGS+"/crashes", mud_name()+" crashed "+ctime(time())+" with error "+
      err+".\n");
    shout("Atmos tells you think "+mud_name()+" is crashing!\n");
    shout("Atmos forced you to: quit\n");
    users()->force_me("quit");
}

int valid_seteuid(object ob, string id) {
    string fn, uid;

return 1;
    if((uid = getuid(ob)) == id) return 1;
    if(uid == "Root") return 1;
    if(file_name(ob) == OB_SIMUL_EFUN) return 1;
    if(uid == "System" && id != "Root" && id != "Backbone")
      return 1;
    return 0;
}

int valid_hide(object who) {
    string priv;

    if(!objectp(who)) return 0;
    if(environment(who) && hiddenp(environment(who))) return 1;
    if(!(priv = query_privs(who))) return 0;
    else return (member_array(PRIV_SECURE, explode(priv, ":")) != -1);
}

int valid_override(string file, string nom) {
    return (file == OB_SIMUL_EFUN || file == "/secure/SimulEfun/overrides");
}

int valid_socket(object ob, string fun, mixed *info) {
    object *obs;
    string tmp;
    int i;

    i = sizeof(obs = previous_object(-1));
    while(i--) {
        if(!obs[i]) continue;
        if(userp(obs[i])) continue;
        if(!(tmp = query_privs(obs[i]))) return 0;
        if(!sizeof(explode(tmp,":") & ({ PRIV_SECURE,PRIV_MUDLIB}))) return 0;
    }
    return 1;
}

int valid_function(function f) { return 1; }

string get_wiz_name(string file) {
    string nom, dir, tmp;

    if(file[0] != '/') file = "/"+file;
    if(sscanf(file, REALMS_DIRS+"/%s/%s", nom, tmp) == 2) return nom;
    if(sscanf(file, DOMAINS_DIRS+"/%s/%s", nom, tmp) == 2) return nom;
    sscanf(file, "/%s/%s", nom, tmp);
    return nom;
}

mixed apply_unguarded(function f) {
    object previous_unguarded;
    string base, err, tmp;
    mixed val;

    if(base_name(previous_object(0)) != OB_SIMUL_EFUN) {
        error("Illegal unguarded apply.");
        return 0;
    }
    previous_unguarded = __Unguarded;
    __Unguarded = previous_object(1);
    err = catch(val = evaluate(f) );
    __Unguarded = previous_unguarded;
    if(err) error(err);
    return val; 
}

string error_handler(mapping mp, int caught) {
    string ret;

    ret = "---\n"+standard_trace(mp);
    if (caught) {
        write_file("/log/catch", ret);
    } else {
        write_file("/log/runtime", ret);
    }
    if (this_player(1)) {
        tell_object(this_player(1), sprintf("%sTrace written to /log/%s\n", mp["error"], (caught ? "catch" : "runtime")));
        this_player(1)->set_error(mp);
    }
    return 0;
}

void log_error(string file, string msg) {
    string nom, home;

    if(this_player(1) && (find_object(OB_SIMUL_EFUN) && creatorp(this_player(1))))
      message("error", msg, this_player(1));
    if(!(nom = get_wiz_name(file))) nom = "log";
    catch(write_file(DIR_ERROR_LOGS+"/"+nom, msg));
}

varargs string standard_trace(mapping mp, int flag) {
    string obj, ret;
    mapping *trace;
    int i,n;

    ret = mp["error"] + "Object: " + trace_line(mp["object"], mp["program"], mp["file"], mp["line"]);
    ret += "\n";
    trace = mp["trace"];

    n = sizeof(trace);

    for (i=0; i<n; i++) {
    if (flag) ret += sprintf("#%d: ", i);

        ret += sprintf("'%s' at %s", trace[i]["function"], trace_line(trace[i]["object"], trace[i]["program"], trace[i]["file"], trace[i]["line"]));
    }
    return ret;
}

string trace_line(object obj, string prog, string file, int line) {
    string ret;
    string objfn = obj ? file_name(obj) : "<none>";

    ret = objfn;
    if (different(objfn, prog)) ret += sprintf(" (%s)", prog);
    if (file != prog) ret += sprintf(" at %s:%d\n", file, line);
    else ret += sprintf(" at line %d\n", line);
    return ret;
}

int different(string fn, string pr) {
    int tmp;

    sscanf(fn, "%s#%d", fn, tmp);
    fn += ".c";
    return (fn != pr) && (fn != ("/" + pr));
}

void master_log_file(string file, string msg) {
    if(file_name(previous_object()) != OB_SIMUL_EFUN) return;
    if(file_size(file) > MAX_LOG_SIZE) rename(file, file+".old");
    write_file(file, msg);
}

void destruct_env_of(object ob) {
    if(!interactive(ob)) return;
    message("environment", "Whoop whoop whoop.... BOOM!!!  And now "
      "nothing exists any longer!", ob);
    ob->move(ROOM_VOID);
}

string make_path_absolute(string file) {
    return absolute_path((string)this_player()->get_path(), file);
}

int player_exists(string str) {
    if(!str) return 0;
    str = DIR_USERS+"/"+str[0..0]+"/"+str+__SAVE_EXTENSION__;
    return (file_size(str) > -1);
}

string get_root_uid() { return "Root"; }

string get_bb_uid() { return "Backbone"; }

string creator_file(string str) {
    string base;
    int ext;

    sscanf(str, "%s#%d", base, ext);
    if("/"+base == OB_USER)
      return (__PlayerName ? __PlayerName : "Mudlib");
    return (string)call_other(OB_SIMUL_EFUN, "creator_file", str);
}

string domain_file(string str) {
    string nom, tmp;

    if(str[0] != '/') str = "/"+str;
    if(sscanf(str, DOMAINS_DIRS+"/%s/%s", nom, tmp) == 2) return nom;
    return 0;
}

string author_file(string str) {
    string nom, tmp;

    if(str[0] != '/') str = "/"+str;
    if(sscanf(str, REALMS_DIRS+"/%s/%s", nom, tmp) == 2) return nom;
    return 0;
}

static int slow_shutdown() {
    write_file(DIR_LOGS+"/game_log", "Armageddon loaded by master: "+ctime(time())+".\n");
    new(OB_SHUT)->move(ROOM_START);
    SHUT_D->reboot_mud(4);
    return 1;
}

int save_ed_setup(object who, int code) {
    string file;

    if(!intp(code)) return 0;
    rm(file = user_path((string)who->query_name())+".edrc");
    return write_file(file, code+"");
}

int retrieve_ed_setup(object who) {
    string file;
    int x;

    file = user_path((string)who->query_name())+".edrc";
    if(!file_exists(file)) return 0;
    return to_int(read_file(file));
}

string get_save_file_name(string file) {
    string str;

    str = (string)this_player()->query_name();
    if(file_size(user_path(str)) == -2)
      return user_path(str)+"dead.edit";
    else return DIR_TMP+"/"+str+".dead.edit";
}

int is_locked() { return MUD_IS_LOCKED; }

string *parse_command_id_list() { return ({ "one", "thing" }); }

string *parse_command_plural_id_list() { return ({ "ones", "things","them"}); }

string *parse_command_adjectiv_id_list() {
    return ({ "the", "an", "a" });
}

string *parse_command_prepos_list() {
    return ({ "in", "with", "without", "into", "for", "on", "under",
      "from", "between", "at", "to", "over", "near" });
}

string parse_command_all_word() { return "all"; }

void create_save() {
    string str;

    if(!stringp(str = (string)previous_object()->query_name())) return;
    if(file_size(DIR_USERS+"/"+str[0..0]) == -2) return;
    if(str[0] < 'a' || str[0] > 'z') return;
    mkdir(DIR_USERS+"/"+str[0..0]);
}

object player_object(string nom) {
    object ob;
    string err;
    string pfile = DIR_USERS+"/"+nom[0..0]+"/"+nom;

    //debug_message("1");
    if(base_name(ob = previous_object(0)) != OB_LOGIN) return 0;
    //debug_message("2");
    set_eval_limit(1000000000);
    __NewPlayer = ob;
    //mkdir(DIR_USERS+"/"+nom[0..0]);
    //if(file_size(pfile) == -1) write_file(pfile,"");
    //debug_message("master() about to try loading: "+pfile);
    err = catch(ob = load_object(pfile));
    __NewPlayer = 0;
    set_eval_limit(-1);
    if(err){
    //debug_message("error: "+err);
    error(err);
    }
    //if(!ob) ob = new("/std/user");
    //if(!ob) ob = ;
    return ob;
}

string *query_group(string grp) { return copy(__Groups[grp]); }

mapping query_groups() { return copy(__Groups); }
