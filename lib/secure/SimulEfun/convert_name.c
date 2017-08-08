/*    /adm/SimulEfun/convert_name.c
 *    from Nightmare IV
 *    changes any cap name to a true name
 *    created by Descartes of Borg 940214
 */

string convert_name(string str) {
    str = replace_string(str, " ", "");
    str = replace_string(str, "'", "");
    return lower_case(replace_string(str, "-", ""));
}
