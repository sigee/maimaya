/*    /daemon/network.c
 *    from the Nightmare Mudlib 3.2
 *    daemon to centralize network routing
 *    created by Descartes of Borg 940130
 */

#include <save.h>
#include <daemons.h> 
#include <security.h> 
#include <network.h> 
 
private mapping __Muds; 
static private int __ReceivedMudlist, __ServerPort; 
static private string __ServerAddr; 
static private mapping __Network; 
 
int mud_exists(string mud);
static void manage_muds();
static private void restore_network();
static private void save_network();

void create() { 
    call_out("setup", 2); 
    __Muds = ([]);
    call_out("ping_muds", 1800); 
    if(file_exists(SAVE_NAMESERVER+".o")) restore_network();
  } 
 
static void setup() { 
    __Network = ([]); 
    __Network["udp"] = ([]);
    __Network["udp"]["port"] = PORT_UDP; 
    if(!__Network["udp"]["socket"]) { 
        __Network["udp"]["socket"] =  
          socket_create(DATAGRAM, "read_callback", "close_callback"); 
        if(__Network["udp"]["socket"] < 0) { 
            write("Oh hell "+__Network["udp"]["socket"]);
            return; 
	  } 
        if(socket_bind(__Network["udp"]["socket"], __Network["udp"]["port"]) <= 0) 
          socket_close(__Network["udp"]["socket"]); 
      } 
    this_object()->send_udp(__ServerAddr = SERVER_ADDR, __ServerPort = SERVER_PORT,  
      sprintf("@@@%s%s@@@\n", SERVICE_UDP_STARTUP, START_MSG)); 
    __ReceivedMudlist = 0; 
    call_out("check_mudlist", 45, -1); 
    manage_muds(); 
  } 
 
static void manage_muds() { 
    call_out("manage_muds", 300); 
    SERVICES_D->send_mudlist_q(__ServerAddr, __ServerPort); 
  } 
 
static void check_mudlist(int i) { 
    string *muds; 
 
    if(__ReceivedMudlist) return; 
    if((++i) >= sizeof(muds = keys(__Muds))) i = 0;
    if(i >= sizeof(muds)) return;
    SERVICES_D->send_mudlist_q(__ServerAddr = __Muds[muds[i]]["HOSTADDRESS"], 
      __ServerPort = __Muds[muds[i]]["PORTUDP"]); 
    call_out("check_mudlist", 45, i); 
  } 
 
static void ping_muds() { 
    string *muds; 
    int i; 
 
    call_out("ping_muds", 1800); 
    i = sizeof(muds = keys(__Muds)); 
    while(i--) { 
        __Muds[muds[i]]["NO CONTACT"]++; 
        SERVICES_D->send_ping_q(__Muds[muds[i]]["HOSTADDRESS"],  
          __Muds[muds[i]]["PORTUDP"]); 
        if(__Muds[muds[i]]["NO CONTACT"]>=MAX_PINGS) { 
            map_delete(__Muds, muds[i]); 
            save_network(); 
        } 
      } 
  } 
 
void send_udp(string host, int port, string msg) { 
    int sock; 
 
    if(file_name(previous_object(0)) != SERVICES_D) return;
    if((sock=socket_create(DATAGRAM,"read_callback","close_callback")) <= 0) { 
        return 0; 
      } 
    socket_write(sock, msg, host+" "+port); 
    socket_close(sock); 
  } 
 
void read_callback(int sock, string msg, string host) { 
    string fun, nom, arg, tmp; 
    string *bits; 
    mapping args; 
    int i, maxi; 
 
    if(!msg) return; 
    tmp + ""; 
    if(!sscanf(msg, "@@@%s||%s@@@%*s", fun, tmp)) { 
        if(!sscanf(msg, "@@@%s@@@%*s", fun)) return; 
        tmp = ""; 
      } 
    sscanf(host, "%s %*s", host); 
    args = allocate_mapping(maxi = sizeof(bits = explode(tmp, "||"))); 
    for(i=0; i<maxi; i++)  
      if(sscanf(bits[i]+"", "%s:%s", nom, arg) == 2) args[nom] = arg; 
    if(fun != SERVICE_UDP_MUDLIST_A && (!args["PORTUDP"] || !args["NAME"] || 
      !(args["HOSTADDRESS"] = host))) return;  
    if(stringp(args["NAME"]) && !mud_exists(args["NAME"])) {
        SERVICES_D->send_ping_q(args["HOSTADDRESS"], args["PORTUDP"]); 
      } 
    else if(stringp(args["NAME"])) 
      __Muds[replace_string(lower_case(args["NAME"]), " ", ".")]["NO CONTACT"]=0;
    if(fun == SERVICE_UDP_MUDLIST_A) { 
        __ReceivedMudlist = 1; 
        remove_call_out("check_mudlist"); 
      } 
    call_other(SERVICES_D, "incoming_"+fun, args); 
  } 
 
void add_mud(mapping borg) { 
    string mud; 
 
    if(file_name(previous_object(0)) != SERVICES_D) return;
    mud = replace_string(lower_case(borg["NAME"]), " ", "."); 
    __Muds[mud] = borg + ([ "REAL NAME": borg["NAME"] ]); 
    save_network(); 
  } 
 
void remove_mud(string mud) { 
    if(file_name(previous_object(0)) != SERVICES_D) return;
    map_delete(__Muds, replace_string(lower_case(mud), " ", ".")); 
    save_network(); 
  } 
 
void send_shutdown() {  
    string *muds; 
    int i; 
     
    if(!((int)master()->valid_apply(({})))) return;
    i = sizeof(muds = keys(__Muds)); 
    while(i--) 
      SERVICES_D->send_shutdown(__Muds[muds[i]]["HOSTADDRESS"],  
        __Muds[muds[i]]["PORTUDP"]); 
  } 
 
string *query_muds() { 
    string *muds, *ret; 
    int i, maxi; 
 
    ret = allocate(maxi = sizeof(muds = keys(__Muds))); 
    for(i=0; i<maxi; i++) ret[i] = __Muds[muds[i]]["REAL NAME"]; 
    return ret; 
  } 
 
mapping query_known_muds() { return copy(__Muds); } 
 
mapping query_mud_info(string mud) {  
    return copy(__Muds[lower_case(replace_string(mud, " ", "."))]); 
  } 
 
int mud_exists(string mud) { 
    return mapp(__Muds[replace_string(lower_case(mud), " ", ".")]); 
  } 
 
string query_real_name(string mud) { 
    return __Muds[replace_string(lower_case(mud), " ", ".")]["REAL NAME"]; 
  } 
 
int query_port(string svc) { return __Network[svc]["port"]; } 
 
static private void save_network() { 
    save_object(SAVE_NAMESERVER); 
  } 

static private void restore_network() {
    restore_object(SAVE_NAMESERVER);
}
 
