inherit "/std/weapon";

create() {
    ::create();
   set_id(({"sword", "the_one_orc_slayer","orcslayer","fine sword"}));
   set_name("orcslayer");
   set_short("a fine sword");
    set_long("It is a very finely crafter sword with the picture of a dying orc on it.\nThere are writings on it.\n");
   set_mass(611);   set_value( 200);

   set_wc(7);
   set_ac(2);
   set_type("blade");
    set_wield_string("You feel a great hatred for orcs as you wield the sword.\n");
    set_hit((:this_object(), "weapon_hit":));    set_read( "Orcslayer\n");

   set_destroy();
}

int weapon_hit(object ob) {
    int x, y;

    if((string)ob->query_race() != "orc") return 0;
    y = random(10);
    x = (int)this_player()->query_alignment();
    if(x> 0) x = (x/100);
    else x = 0;
    y += x;
    write("The Orcslayer draws more orc blood!\n");
    say(this_player()->query_cap_name()+" Orcslayer draws more orc blood!\n", this_player());
    return y;
}
