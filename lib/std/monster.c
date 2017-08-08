//	/std/monster.c
//	from the Nightmare mudlib
//	inheritable code for npc's
//	created by Descartes of Borg september 1992

#include <std.h>
#include <move.h>
#include <objects.h>
#include <daemons.h>

inherit LIVING;

object tmp;
static status heart_beat_on;
int level;
static int speed;
static int moving;
mapping emotes, speech, spells;
string body_type;
int __NextHeal;
mixed __Aggressive;
string __Race;
private static mixed __Die;
string *route;
int position, patrol;

int test_heart();
mixed query_aggressive();
void set_spell_chance(int x);
void set_spells(string *arr);
string return_chat();
string return_achat();
void set_exp(int x);
void set_level(int x);
int query_level();
void move_around();
void set_emotes(int x, string *arr, int att);
void set_patrol(int i, string *str);

void create() {
    ::create();
    emotes = ([]);
    enable_commands();
    init_limb_data();
    init_stats();
    init_skills(0);
    init_living();
    set_heart_beat(heart_beat_on = 1);
    speed = 0;
    set_gender("neuter");
    speech = ([]);
    spells = ([]);
    __NextHeal = time()+ 30;
}

void init() {
    mixed tmp;

    if(!heart_beat_on) {
        set_heart_beat(heart_beat_on = 1);
        while(__NextHeal < time()) {
            do_healing(calculate_healing());
            __NextHeal += 30; }
    }
    if(this_player()->query_invis()) {
        if(creatorp(this_player())) return;
	if(random(101) < (int)this_player()->query_level()) return;
    }
    if(this_player()->query_ghost()) return;
    if(this_player()->is_player()) {
        if(tmp = query_aggressive()) {
            if(intp(tmp) && tmp > (int)this_player()->query_stats("charisma"))
              kill_ob(this_player(), 0);
            else if(stringp(tmp)) call_other(this_object(), tmp);
        }
    }
}

void set_name(string str) {
   set_living_name(str);
   ::set_name(str);
   if(query_level() < 1) set_level(1);
}

void heart_beat() {
    string *langs;
    string lang, pre;

    living::heart_beat();
   ok_to_heal ++;
    if(__NextHeal > time()) calculate_healing();
    else while(__NextHeal < time()) {
        do_healing(calculate_healing());
        __NextHeal += 30;
    }
   continue_attack();
   if(speed && moving >= speed) move_around();
   else moving++;
    if(!query_current_attacker()) pre = "";
    else pre = "attack ";
    if(emotes[pre+"chance"] > random(100))
      if(this_object())
      message("environment", emotes[pre+"msg"][random(sizeof(emotes[pre+"msg"]))],
        environment(this_object()), ({ this_object() }) );
    if(sizeof(langs = keys(speech))) {
      lang = langs[random(sizeof(langs))];
      if(this_object() && random(100) < speech[lang][pre+"chance"])
          this_object()->force_me("speak in "+lang+" "+
            speech[lang][pre+"msg"][random(sizeof(speech[lang][pre+"msg"]))]);
    }
    magic_round = 0;
    if(!test_heart()) set_heart_beat(heart_beat_on = 0);
}

void die(object ob) {
   object money_ob;
   object *contents;
   int i, tmp_size;
    string *currs;

    if(functionp(__Die)) {
        if(!((int)((*__Die)(ob)))) return;
    }
    else if(stringp(__Die))
      message("other_action",__Die,environment(this_object()),({this_object()}));
    else message("other_action", query_cap_name()+
      " drops %^RED%^dead%^RESET%^ before you.", environment(this_object()),
      ({ this_object() }));
    tmp = new(OB_CORPSE);
    tmp->set_name(query_cap_name());
    tmp->copy_body(this_object());
    tmp->move(environment(this_object()));
    tmp_size=sizeof(currs=query_currencies());
    if(tmp_size) {
      money_ob = new("/std/obj/coins");
      for(i=0; i<tmp_size; i++) {
        money_ob->add_money(currs[i], query_money(currs[i]));
        add_money(currs[i], -query_money(currs[i]));
      }
      money_ob->move(tmp);
    }
   contents = all_inventory(this_object());
   for(i=0;i<sizeof(contents);i++) {
      if(contents[i]->move(tmp)) 
         if(!contents[i]->remove())
            destruct(contents[i]);
   }
   remove();
}

int query_npc() { return 1; }

varargs void move_player(mixed dest, string message) {
   object prev;
   string here,going,temp1,temp2,temp3;
   prev = environment(this_object());
   here = file_name(prev);
   if (stringp(dest)) {
     if (sscanf(dest,"/%s",temp1)!=1) {
       sscanf(here,"/%s",temp1);
       going = "";
       while(sscanf(temp1,"%s/%s",temp2,temp3)==2) {
         going = going + "/"+temp2;
         temp1 = temp3;
       }
       temp1 = dest;
       if (file_size(going+"/"+temp1+".c") != -1) {
         dest = going+"/"+temp1;
       }
     }
   }
   if(move(dest) == MOVE_OK) {
        message("other_action", query_cap_name()+" arrives.",
          environment(this_object()), ({ this_object() }));
      tell_room(prev, query_cap_name() + " leaves " + message + ".\n");
      }
}

void set_patrol(int i, string *str) {
    speed = i;
    moving = 1;
    patrol = 1;
    route = ({});
    route += str;
    position = 0;
}

/* Call_out taken out of move_around by Hanse 1/4/93 */
void move_around() {
    object env;
    string *enters;
   string *exits;
    if(!this_object()) return;
    if(env = environment(this_object())) {
       exits = (string*)environment(this_object())->query_exits();
        enters = (string *)env->query_enters();
        if(!sizeof(exits)) exits = 0;
        if(!sizeof(enters)) enters = 0;
    }
   else exits = 0;
    if(exits || enters) {
        if(patrol) {
            command(route[position]);
            ++position;
            if(position == sizeof(route)) position = 0;
        }
    else if(exits) command("go "+exits[random(sizeof(exits))]);
    else command("enter "+enters[random(sizeof(enters))]);
    }
   moving=0;
}

void set_moving(int i) {
    if(!heart_beat_on) set_heart_beat(heart_beat_on = 1);
    moving = 1;
}

void set_speed(int i) { speed = i; }

void set_level(int x) {
    level = x;
    set_overall_ac(x + 1);
    set_max_hp(58 + (x * 65));
    set_max_mp(58 + (x * 65));
    set_max_sp(x*65);
    set_skill("melee", (x*4));
    set_skill("attack", (x*4));
    set_skill("defense", (x*4));
    set_skill("blade", (x*4));
    set_skill("knife", (x*4));
    set_skill("blunt", (x*4));
    set_skill("projectile", (x*4));
    set_skill("two handed", (x*4));
    set_stats("constitution", x*3/2);
    set_stats("strength", x*3/2);
    set_stats("intelligence", x*3/2);
    set_stats("wisdom", x*3/2);
    set_stats("dexterity", x*3/2);
    set_stats("charisma", x*3/2);
    set_exp( (int)ADVANCE_D->get_exp(x) );
    set_mp(query_max_mp());
    set_hp(query_max_hp());
    set_sp(query_max_sp());
}

int query_level() { return level; }

// Added by Valodin, June 28, 1993
// Sets the body type to a certain race using the race daemon
void set_body_type(string str) {
  mapping monster_bod;
    int mag, max_mag;
  string *mon_limbs;
  int i, max;

  if(!str || !RACE_D->is_monster_race(str))
    str = "human";
    mag = query_mp();
    max_mag = query_max_mp();
  init_limb_data();
  monster_bod = (mapping)RACE_D->monster_body(str, query_max_hp());
  for(i = 0, max = sizeof(mon_limbs = keys(monster_bod)); i < max; i++)
    add_limb(mon_limbs[i], monster_bod[mon_limbs[i]]["limb_ref"], 
	     monster_bod[mon_limbs[i]]["max_dam"], 0, 0);
  set_wielding_limbs((string *)RACE_D->query_monster_wielding_limbs(str));
  set_fingers((int)RACE_D->query_monster_fingers(str));
  body_type = str;
    set_mp(mag);
    set_max_mp(max_mag);
}

string query_body_type()
{
  return body_type;
}

void set_spell_chance(int x) {
    if(!spells) spells = ([]);
    spells["chance"] = x;
}

void set_spells(string *arr) {
    if(!spells) spells = ([]);
    spells["commands"] = arr;
}

int query_spell_chance() { 
    if(spells) return spells["chance"];
    else return 0;
}

string *query_spells() {
    if(spells) return spells["commands"];
    else return 0;
}

string get_random_spell() {
    if(!spells) return 0;
    if(sizeof(spells["commands"]) <2) return spells["commands"][0];
    else return spells["commands"][random(sizeof(spells["commands"]))];
}

void set_exp(int x) {
    if( x > (int)ADVANCE_D->get_exp(level) )
	player_data["general"]["experience"] = (int)ADVANCE_D->get_exp(level);
    else player_data["general"]["experience"] = x;
}

// These two functions remain for backwards Nightmare 1.* and 2.* compat

void set_chats(int x, string *arr) { set_emotes(x, arr, 0); }

void set_achats(int x, string *arr) { set_emotes(x, arr, 1); }

void set_emotes(int x, string *arr, int att) {
    emotes[(att ? "attack chance" : "chance")] = x;
    emotes[(att ? "attack msg" : "msg")] = arr;
}

mapping query_speech() { return speech; }

void set_speech(int x, string lang, string *arr, int att) {
    if(!speech[lang]) speech[lang] = ([]);
    if(!speech[lang]["attack msg"])
        speech[lang]["attack msg"] = ({});
    if(!speech[lang]["msg"])
        speech[lang]["msg"] = ({});
    if(att) {
        speech[lang]["attack chance"] = x;
        speech[lang]["attack msg"] = arr;
    }
    else {
        speech[lang]["chance"] = x;
        speech[lang]["msg"] = arr;
    }
}

void set_alignment(int x) { player_data["general"]["alignment"] = x; }


int test_heart() {
    object env;
    object *inv;
    int i;

    if(!this_object()) return 1;
    if(!(env = environment(this_object()))) return 0;
    if(query_current_attacker() || speed) return 1;
    i = sizeof(inv = all_inventory(env));
    while(i--) 
      if(interactive(inv[i]) || inv[i]->query("aggressive")) return 1;
    return 0;
}

int query_heart_status() { return heart_beat_on; }

void receive_message(string cl, string msg) {
    this_object()->catch_tell(msg);
}

void set_languages(string *langs) {
    int i;
    
    i = sizeof(langs);
    while(i--) set_lang_prof(langs[i], 10);
}

void set_die(mixed val) {
    __Die = val;
}

mixed query_die() { return __Die; }

void set_race(string str) {
    __Race = str;
    LANG_D->init_languages(this_object());
}

string query_race() { return __Race; }

void set_aggressive(mixed val) { __Aggressive = val; }

mixed query_aggressive() { return __Aggressive; }

string query_short() {
    return query_invis() ? 0 : living::query_short();
}

int query_player_killer() { return 1; }
