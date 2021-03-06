/*    /daemon/chat.c
 *    from the Nightmare 3.2 Mudlib
 *    daemon to handle all mud chat channels
 *    created by Descartes of Borg 931220
 */

#include <std.h>
#include <config.h>
#include <daemons.h>
#include <chat.h>
#define MSG_NOBLOCK  (1 << 0)                 /* no blocking */
#define MSG_ERROR    ((1 << 6) | MSG_NOBLOCK)              /* error messages */
#define MSG_SYSTEM   ((1 << 10) | MSG_NOBLOCK)             /* sys messages */
#define MSG_CHAN     (1 << 14)

#define ORIGIN_LOCAL         "local"

inherit DAEMON;

string suspect,site,plainmsg,chan,foo,bar,baz;
static private mapping Channels;
static private mapping chanlast;

static private string *local_chans = ({"newbie","cre","gossip","admin","error",
  "priest", "mage", "explorer", "thief", "fighter", "death", "connections" });
static string *syschans = ({ "death", "connections" });


static void create() {
    object pl;
    string *tmp_arr = ({});
    daemon::create();
    //SetNoClean(1);
    Channels = ([]);
    foreach(string kanal in local_chans + syschans){
	if( !Channels[kanal] ) Channels[kanal] = ({});
    }
    foreach(pl in users()) {
	string *chans;
	string channel;

	if( pl && !(chans = (string *)pl->GetChannels()) ) continue;
	foreach(channel in chans) {
	    if( !Channels[channel] ) Channels[channel] = ({});
	    Channels[channel] = distinct_array(Channels[channel] + ({ pl }));
	}
    }
    foreach( string channel in local_chans ){
	tmp_arr += ({ channel + "emote" });
	tmp_arr += ({ channel + ":" });
    }

    local_chans += tmp_arr;
}

string decolor(string str){
    string s1 = "", s2, s3, test;
    int tmp = 2;
    if(sscanf(str,"%s<%s>%s",s1,s2,s3) != 3) 
	tmp = sscanf(str,"<%s>%s",s2,s3);
    if(tmp != 2) return str;
    else {
	test = s1+"<"+s2+">%^RESET%^"+strip_colours(s3);
	return test;
    }
}

varargs int CanListen(object who, string canal){
    return 1;
} 

varargs int CanTalk(object who, string canal){
    return 1;
}

string *eventRegisterMember(string *chans) {
    string *tmp;
    object ob;
    string channel;

    if( !living(ob = previous_object()) ) return ({});
    tmp = ({});
    foreach(channel in chans) {
	/* just check out for secure channels */
	switch(channel) {
	case "admin":
	    if( !archp(ob) ) break;
	case "cre": case "intercre": case "intergossip":
	    if( !creatorp(ob) ) break;
	default:
	    if( !Channels[channel]) Channels[channel] = ({});
	    Channels[channel] = distinct_array(Channels[channel] + ({ ob }));
	    tmp += ({ channel });
	}
    }
    return tmp;
}

string *eventRemoveMember(string *chans) {
    object ob;
    string channel;

    if( !living(ob = previous_object()) ) return({});
    foreach(channel in chans) {
	if( !Channels[channel] ) continue;
	else Channels[channel] -= ({ ob });
	if( !sizeof(Channels[channel]) ) map_delete(Channels, channel);
    }
    return chans;
}

int cmdLast(string feep){

    if(!chanlast||!Channels[feep]||member_array(this_player(), Channels[feep])==-1){

	this_player()->eventPrint(mud_name() + " does not have that channel on record.", MSG_ERROR);
	return 1;
    }
    if(!sizeof(chanlast[feep]))
    {
	this_player()->eventPrint("That channel has no backlog.", MSG_ERROR);
	return 1;
    }
    if(!CanListen(this_player(),feep)){
	write("You lack privileges to that channel.");
	return 1;
    }
    this_player()->eventPrint(implode(chanlast[feep], "\n"));
    return 1;
}

varargs int eventAddLast(string feep, string str, string pchan, string pmsg, string pwho)
{
    chan=feep;
    if(!chanlast)
	chanlast=([]);
    if(!sizeof(chanlast[chan]))
	chanlast[chan] = ({});
    if(sizeof(chanlast[chan]) == 50)
	chanlast[chan] = chanlast[chan][1..sizeof(chanlast[chan])];
    chanlast[chan] += ({ str });
    if(!pchan || pchan == "") pchan = "foo";
    plainmsg = "bar";
    if(pchan) plainmsg = "<" + pchan + "> ";
    if(pmsg) plainmsg += pmsg;
    if(pwho && pwho !="") plainmsg = pwho+" "+plainmsg;
    if(pchan && pchan != "admin"){
	chan = GetLocalChannel(chan);
	//unguarded( (: write_file("/log/chan/"+chan,"["+timestamp()+"] "+plainmsg+"\n") :) );
	if( file_size("/log/chan/"+chan) > 200000) {
	    //unguarded( (: rename("/log/chan/"+chan,"/log/chan/"+chan+"."+timestamp() ) :) );
	}
    }
    else {
	//unguarded( (: write_file("/secure/log/"+chan,"["+timestamp()+"] "+plainmsg+"\n") :) );
	if( file_size("/secure/log/"+chan) > 200000) {
	    //unguarded( (: rename("/secure/log/"+chan,"/secure/log/"+chan+"."+timestamp() ) :) );
	}
    }
    return 1;
}

int cmdChannel(string verb, string str) {
    string varb, msg, name, rc, target, targetkey, target_msg;
    object ob = 0;
    int emote;

    if(str[0..0] == ":" && 
      (member_array(str[1..1], ({ "Q", "O", "P", "D", "I", "X" })) == -1 &&
	sizeof(str) > 2)){
	if(strsrch(verb,"emote") == -1) verb += "emote";
	str = replace_string(str,":","",1);
    }

    if(strsrch(verb, ":") != -1){
	varb = replace_string(verb,":","");
	verb = replace_string(verb,":","emote");
    }
    else if(strsrch(verb, "emote") != -1) varb = replace_string(verb,"emote","");
    else if(verb[<1..<1] == ":") varb = replace_string(verb,":","");
    else varb = verb;

    if( verb == "list" ) {
	string *who;
	string ch, mud;

	if( !str ) {
	    return 0;
	}
	if( sscanf(str, "%s@%s", ch, mud) == 2 ) {
	    //mud = trim(mud);
	    //if(!alphap(last(mud,1))) mud = truncate(mud,1);
	    if( !Channels[ch] ) {
		return 0;
	    }
	    if( member_array(this_player(), Channels[ch]) == -1 ) {
		return 0;
	    }
	    if( ch == (ch = GetRemoteChannel(ch)) ) {
		if(!creatorp(this_player())){
		    write("Remote channel information is not available to players.");
		    return 1;
		}
	    }
	    if( !(mud = (string)INTERMUD_D->GetMudName(mud)) ) {
		this_player()->eventPrint(mud_name() + " is not aware of " 
		  "such a place.", MSG_ERROR);
		return 1;
	    }
	    if(!CanTalk(this_player(),verb)){
		write("You lack privileges to that channel.");
		return 1;
	    }
	    SERVICES_D->eventSendChannelWhoRequest(ch, mud);
	    this_player()->eventPrint("Remote listing request sent.",
	      MSG_SYSTEM);
	    return 1;
	}
	else ch = str;
	if( !Channels[ch] ) {
	    return 0;
	}
	if( member_array(this_player(), Channels[str]) == -1 ) {
	    return 0;
	}
	who = GetChannelList(str);
	msg = "Online: " + implode(who, "   ");
	this_player()->eventPrint(msg, MSG_SYSTEM);
	return 1;
    }
    if(!CanTalk(this_player(),verb)){
	write("You lack privileges to that channel.");
	return 1;
    }
    if( !str || str == "" ) {
	this_player()->SetBlocked(verb);
	return 1;
    }
    if(member_array(verb, syschans) != -1) {
	write("This is not a channel for chatting.");
	return 1;
    }
    if(this_player()->GetProperty("gag")){
	write("You have gag mode enabled. Type: 'gag off' to talk on channels.");
	return 1;
    }

    if( !Channels[verb] ) {
	if( sscanf(verb, "%semote", verb) || sscanf(verb, "%s:", verb) ) {
	    string emote_cmd, remains;
	    mixed *msg_data;
	    int i;
	    if( !Channels[verb] ) {
		return 0;
	    }
	    rc = GetRemoteChannel(verb);
	    sscanf(str, "%s %s", emote_cmd, remains);
	    if( !emote_cmd ) {
		emote_cmd = str;
		remains = 0;
	    }
	    if( !remains ) {
		msg_data = SOUL_D->GetChannelEmote(emote_cmd, "");
		if( !msg_data ) {
		    str = "$N " + str;
		}
	    }

	    else {
		if( ob = find_living(target = convert_name(remains)) ) {
		    msg_data = SOUL_D->GetChannelEmote(emote_cmd, "LIV");
		    if( !msg_data ) {
			str = "$N " + str;
			target = 0;
		    }
		}
		else if( strsrch(target, "@") == -1 || rc == verb ) {
		    string *words = explode(remains, " ");
		    target = "";
		    for(i=0; i<sizeof(words); i++) {
			target += lower_case(words[i]);
			if( ob = find_living(target) ) {
			    if( i < sizeof(words)-1 ) {
				remains = implode(words[(i+1)..], " ");
			    }
			    else {
				remains = 0;
			    }
			    msg_data = SOUL_D->GetChannelEmote(emote_cmd,
			      "LIV STR",
			      remains);
			    break;
			}
		    }
		    if( !ob ) {
			msg_data = SOUL_D->GetChannelEmote(emote_cmd, "STR",
			  remains);
			target = 0;
		    }
		    if( !msg_data ) {
			str = "$N " + str;
			target = 0;
		    }
		}
		else if( rc != verb ) {
		    string *words;
		    i = strsrch(remains, "@", -1);
		    if( i >= strlen(remains)-1 ) {
			msg_data = SOUL_D->GetChannelEmote(emote_cmd, "STR",
			  remains);
			target = 0;
		    }
		    else {
			string mud;

			words = explode(remains[(i+1)..], " ");
			target = remains[0..i];
			remains = "";
			while(sizeof(words)) {
			    mud = implode(words, " ");
			    //mud = trim(mud);
			    //if(!alphap(last(mud,1))) mud = truncate(mud,1);
			    mud = INTERMUD_D->GetMudName(lower_case(mud));
			    if( mud ) {
				target += mud;
				break;
			    }
			    if( remains == "" ) {
				remains = words[<1];
			    }
			    else {
				remains = words[<1] + " " + remains;
			    }
			    words = words[0..<2];
			}
			if( !mud ) {
			    msg_data = SOUL_D->GetChannelEmote(emote_cmd,
			      "STR", remains);
			    target = 0;
			}
			else {
			    if( remains == "" ) {
				msg_data = SOUL_D->GetChannelEmote(emote_cmd,
				  "LIV");
			    }
			    else {
				msg_data = SOUL_D->GetChannelEmote(emote_cmd,
				  "LIV STR",
				  remains);
			    }
			    if( !msg_data ) {
				str = "$N " + str;
				target = 0;
			    }
			}
		    }
		}
	    }
	    if( msg_data ) {
		string sgen = this_player()->GetGender();
		string tgen = 0;

		if( ob ) {
		    target = ob->GetName();
		    tgen = ob->GetGender();
		}
		else if( target ) {
		    string user, mud;

		    sscanf(target, "%s@%s", user, mud);
		    targetkey = target;
		    tgen = SERVICES_D->GetRemoteGender(user, mud);
		    target = SERVICES_D->GetRemoteDisplayName(user, mud);
		    if( !target ) {
			target = capitalize(targetkey);
		    }
		}
		//str = create_message(3, msg_data[0][0],
		  //msg_data[0][1], "$N", sgen, "$O", tgen,
		  //msg_data[1]);
                str = "a bug occurs";
		if( target ) {
		    //target_msg = create_message(POV_TARGET, msg_data[0][0],
		      //msg_data[0][1], "$N", sgen,
		      //"$O", tgen, msg_data[1]);
                      target_msg = "a bug occurs";
		}
	    }
	    emote = 1;
	}
	else {
	    return 0;
	}
    }
    else {
	rc = GetRemoteChannel(verb);
    }
    if( member_array(this_player(), Channels[verb]) == -1 ) return 0;

    if( (int)this_player()->GetBlocked(verb) ) {
	if( (int)this_player()->GetBlocked("all") ) {
	    this_player()->eventPrint("You cannot chat while totally blocked.",
	      MSG_ERROR);
	    return 1;
	}
	this_player()->eventPrint("Turn this channel on to talk on it.", MSG_ERROR);
	return 1;
    }
    if( verb == "admin"  || verb == "cre") {
	if( !(name = (string)this_player()->GetCapName()) )
	    name = capitalize((string)this_player()->GetKeyName());
    }
    else name = (string)this_player()->GetName();
    if( target_msg ) {
	target_msg = replace_string(target_msg, "$O's", "your");
    }
    if(strsrch(str,"$N") == -1 && emote) str = "$N "+str;

    eventSendChannel(name, verb, str, emote, target, target_msg);
    if(member_array(GetRemoteChannel(verb),INTERMUD_D->GetChannels()) != -1){
	if( ob ) {
	    SERVICES_D->eventSendChannel(name, rc, str, emote, target,
	      target_msg);
	}
	else {
	    SERVICES_D->eventSendChannel(name, rc, str, emote, targetkey,
	      target_msg);          
	}
    }
    return 1;
}

varargs void eventSendChannel(string who, string ch, string msg, int emote,
  string target, string targmsg) {
    object channeler = find_player(lower_case(who));
    string pchan,pmsg;
    pchan=ch;
    if(!channeler) channeler = this_player();

    if(this_player() && this_player() != channeler) channeler = this_player();

    if(!strsrch(base_name(previous_object()), "/realms/") ||
      !strsrch(base_name(previous_object()), "/open/")) {
	return 0;
    }

    if(member_array(ch, syschans) != -1) {
	emote = 0;
    } 
    if(channeler){
	if(!CanTalk(channeler, ch) && member_array(ch, syschans) == -1){

	    return;
	}
    }

    if( file_name(previous_object()) == SERVICES_D) {
	ch = GetLocalChannel(ch);
	if( emote && sizeof(who)) msg = replace_string(msg, "$N", who);
    }
    else if( origin() != ORIGIN_LOCAL && previous_object() != master() &&
      file_name(previous_object()) != PARTY_D && member_array(ch, syschans) == -1){
	return;
    }
    if(!Channels[ch] && file_name(previous_object()) != SERVICES_D){
	return;
    }
    if( emote ) {
	object *obs;
	object ob;
	string this_msg, tmp;
	if( target && (ob = find_player(convert_name(target))) ) {
	    target = (string)ob->GetName();
	}
	switch(ch)
	{
	case "connections":
	    this_msg = "%^WHITE%^";
	    break;
	case "death":
	    this_msg = "%^RED%^";
	    break;
	case "cre":
	    this_msg = "%^GREEN%^";
	    break;
	case "admin":
	    this_msg = "%^MAGENTA%^";
	    break;
	case "dchat":
	    this_msg = "%^CYAN%^";
	    break;
	case "intergossip":
	    this_msg = "%^GREEN%^";
	    break;
	case "intercre":
	    this_msg = "%^YELLOW%^";
	    break;
	case "ds":
	    this_msg = "%^YELLOW%^";
	    break;
	case "newbie":
	    this_msg = "%^B_YELLOW%^";
	    break;
	case "gossip":
	    this_msg = "%^B_BLUE%^";
	    break;
	default:
	    this_msg = "%^BLUE%^";
	}
	this_msg += "<" + ch + ">%^RESET%^ ";
	msg = replace_string(msg, "$N", who);
	if( target ) {
	    msg = replace_string(msg, "$O", target);
	    targmsg = replace_string(targmsg, "$N", who);
	    targmsg = capitalize(replace_string(targmsg, "$O", "you"));
	}
	tmp = this_msg + msg;
	eventAddLast(ch, tmp, pchan, msg);
	if(Channels[ch]){
	    obs = filter(Channels[ch], (: $1 && !((int)$1->GetBlocked($(ch))) :));
	    foreach(object listener in obs) {
		int ignore;
		if(sscanf(who,"%s@%s",suspect,site) < 2) {
		    suspect = who;
		    site = "@"+mud_name();
		}
		else site = "@"+site;
		if( listener == ob ) continue;
		if(sizeof(listener->GetMuffed()))
		    foreach(string jerk in listener->GetMuffed()){
		    if(jerk && lower_case(suspect) == lower_case(jerk)) ignore = 1;
		    if(jerk && lower_case(site) == lower_case(jerk)) ignore = 1;
		}
		if(listener->GetNoChanColors()) tmp = decolor(tmp);
		if(!ignore && CanListen(listener,ch) && !(listener->GetProperty("mute"))) 
		    listener->eventPrint(tmp, MSG_CHAN);
		ignore = 0;
	    }
	    if( member_array(ob, obs) != -1 ) {
		if( ob && !((int)ob->GetBlocked(ch)) ) {
		    int ignore;
		    tmp = this_msg + targmsg;
		    if(sizeof(ob->GetMuffed()))
			foreach(string jerk in ob->GetMuffed()){
			if(jerk && lower_case(suspect) == lower_case(jerk)) ignore = 1;
			if(jerk && lower_case(site) == lower_case(jerk)) ignore = 1;
		    }
		    if(ob->GetNoChanColors()) tmp = decolor(tmp);
		    if(!ignore && CanListen(ob,ch)&& !(ob->GetProperty("mute"))) 
			ob->eventPrint(tmp, MSG_CHAN);
		    ignore = 0;
		}
	    }
	}
	suspect = "";
	site = "";
    }
    else {
	object *obs;
	string tmsg;

	tmsg = who + " ";

	switch(ch)
	{
	case "cre":
	    tmsg += "%^GREEN%^";
	    break;
	case "connections":
	    tmsg += "%^WHITE%^";
	    break;
	case "death":
	    tmsg += "%^RED%^";
	    break;
	case "admin":
	    tmsg += "%^MAGENTA%^";
	    break;
	case "dchat":
	    tmsg += "%^CYAN%^";
	    break;
	case "intergossip":
	    tmsg += "%^GREEN%^";
	    break;
	case "intercre":
	    tmsg += "%^YELLOW%^";
	    break;
	case "ds":
	    tmsg += "%^YELLOW%^";
	    break;
	case "newbie":
	    tmsg += "%^B_YELLOW%^";
	    break;
	case "gossip":
	    tmsg += "%^B_BLUE%^";
	    break;
	default:
	    tmsg += "%^BLUE%^";
	}
	tmsg += "<"+ch+">%^RESET%^ " + msg;
	pmsg = msg;
	msg = tmsg;
	eventAddLast(ch, msg, pchan, pmsg, who);
	if(Channels[ch]) {
	    obs = filter(Channels[ch], (: $1 && !((int)$1->GetBlocked($(ch))) :));
	    foreach(object ob in obs){
		int ignore;
		if(sscanf(who,"%s@%s",suspect,site) < 2) {
		    suspect = who;
		    site = "@"+mud_name();
		}
		else site = "@"+site;

		if(sizeof(ob->GetMuffed()))
		    foreach(string jerk in ob->GetMuffed()){
		    if(jerk && lower_case(suspect) == lower_case(jerk)) ignore = 1;
		    if(jerk && lower_case(site) == lower_case(jerk)) ignore = 1;
		}
		if(ob->GetNoChanColors()) msg = decolor(msg);
		if(!ignore && CanListen(ob,ch)&& !(ob->GetProperty("mute"))) 
		    ob->eventPrint(msg, MSG_CHAN);

		ignore = 0;
		suspect ="";
		site = "";
	    }
	}
    }
}

string *GetChannelList(string ch) {
    string *ret;
    object who;

    if( file_name(previous_object()) == SERVICES_D ) ch = GetLocalChannel(ch);
    else if( origin() != ORIGIN_LOCAL ) return ({});
    if( !Channels[ch] ) return ({});
    ret = ({});
    foreach(who in Channels[ch]) {
	if( !who || (int)who->GetInvis() || (int)who->GetBlocked(ch) )
	    continue;
	ret += ({ (string)who->GetName() });
    }
    return ret;
}

string *GetLocalChannels(){
    return copy(local_chans);
}

string GetLocalChannel(string ch) {
    switch(ch) {
    case "imud_code":
	return "intercre";

    case "imud_gossip":
	return "intergossip";

    case "dutch":
	return "dutch";

    case "dead_test4":
	return "ds_test";

    case "dead_souls":
	return "ds";

    }
    return ch;
}

string GetRemoteChannel(string ch) {
    switch(ch) {
    case "intercre": 
	return "imud_code";

    case "intergossip":
	return "imud_gossip";

    case "foundation":
	return "ie_flibcode";

    case "dutch":
	return "dutch";

    case "ds_test":
	return "dead_test4";

    case "ds":
	return "dead_souls";

    }
    return ch;
}

string *GetChannels() { return copy(keys(Channels)); }
string *GetSystemChannels() { return copy(syschans); }
