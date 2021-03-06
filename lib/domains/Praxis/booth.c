//     /d/standard/booth.c
//     from the Nightmare mudlib
//     a nice, simple polling booth
//     created by Gregon@Nightmare Jan 1994
 
#include <daemons.h>
#define BOOTH_VOTES "/domains/Praxis/data/booth_votes"
#define BOOTH_QUESTION "/domains/Praxis/data/booth_question"
inherit "std/room";
 
int a,b;
mapping vote;
 
void create() {
  ::create();
  set_short("Nightmare referendum room");
  set_property("light", 2);
  set_property("indoors", 1);
  set_long(
       "This is the referendum room of Nightmare.  Players come here to vote "+
       "on issues that the wizards desire input on.  The current voting "+
       "question is posted on the wall <read question> will display it."
       );
    
  set_exits( (["down" : "/domains/Praxis/adv_main"]) );
  vote=([]);
  restore_object(BOOTH_VOTES);
}
 
void init() {
  ::init();
  add_action("vote", "vote");
  add_action("read", "read");
  if(archp(this_player())) add_action("check", "check");
  
}
 
int vote(string str) {
    string me;
 
    me=(string)this_player()->query_name();
    if(str!="a" && str!="b") {
       notify_fail("You may only vote for a or b.\n");
       return 0;
    }
    if(MULTI_D->non_voter(me)) {
       write("You are listed as a second character.  You may not vote.");
       return 1;
    }
    if(vote[me]) {
       write("You have already voted.");
       return 1;
    }
    if(str=="a") {
      vote[me]="a";
      a++;
      write("Vote counted.  Thank you.");
      save_object(BOOTH_VOTES);
      return 1;
    }
    if(str=="b") {
      vote[me]="b";
      b++;
      write("Vote counted.  Thank you.");
      save_object(BOOTH_VOTES);
      return 1;
    }
    
}
 
int read(string str) {
    if(!str || str != "question") return 0;
    write("The current voting question is this:\n");
    this_player()->more(BOOTH_QUESTION); 
    return 1;
}
 
int check(){
   write("A: "+a+"\nB: "+b);
   return 1;
}
