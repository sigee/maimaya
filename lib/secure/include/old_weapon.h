private static object gob;
private static string gstr;

void set_hit_func(object ob) { gob = ob; set_hit((: gob, "weapon_hit" :)); }

void set_wield_func(object ob) { gob = ob; set_wield((: gob, "extra_wield" :)); }

void set_wield_string(string str) { gstr = str; set_wield(gstr); }

