#ifndef __OBJECT_H
#define __OBJECT_H

int move(mixed dest);
int remove();
int allow_get(object ob);
int allow_drop(object ob);
int allow_put(object ob);
int id(string str);
string *parse_command_id_list();
string *parse_command_plural_id_list();
string *parse_command_adjectiv_id_list();
static string pluralize_id(string str);
int ReadObject(string str);
void set_id(string *arr);
string *query_id();
void set_adjectives(string *arr);
string *query_adjectives();
void set_name(string str);
string query_name();
void set_cap_name(string str);
string query_CapName();
string query_cap_name();
void set_short(mixed val);
string query_short();
void set_long(mixed val);
varargs string query_long(string str);
varargs void set_read(mixed val, mixed unused);
varargs mixed query_read(string str);
void set_destroy();
int query_destroy();
void set_value(int x);
int query_value();
void set_mass(int x);
int query_mass();
void add_mass(int x);
static void set_last_location(object ob);
object query_last_location();
void hide(int x);
void set_invis(mixed val);
varargs int query_invis(object ob);
void remove_invis_test(function f);
void set_material(string str);
string query_material();
void set_vendor_type(string str);
string query_vendor_type();
void set_prevent_get(mixed val);
mixed query_prevent_get();
void set_prevent_drop(mixed val);
mixed query_prevent_drop();
void set_prevent_put(mixed val);
mixed query_prevent_put();
void set_properties(mapping borg);
mapping query_properties();
void set_property(string prop, mixed val);
mixed query_property(string prop);
void add_property(string prop, mixed val);
void remove_property(string prop);

#endif /* __OBJECT_H */
