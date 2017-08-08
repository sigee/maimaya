/*    /adm/SimulEfun/absolute_path.c
 *    from Nightmare IV
 *    returns the full path of a string based on a relative path
 *    created by Huthar@Portals as resolv_path.c
 *    modifications by Pallando@Nightmare 930526
 *    changed to absolute_path() by Descartes of Borg 940501
 *    features added and speed doubled by Pallando, 940531
 */

string absolute_path(string curr, string newstr) {
    int i, len;
    string *tmp;
    string name, rest;

    if(curr && (curr == "cwd") && this_player())
      curr = (string)this_player()->get_path();
    if(!newstr || newstr == "" || newstr == ".") return curr;
    if( (newstr == "here") && this_player() )
    {
        return file_name(environment(this_player())) + ".c";
    }
    len = strlen( newstr );
    switch( newstr[0..0] )
    {
        case "~":
	    if( newstr == "~" || newstr == "~/" )
		newstr = user_path( (string)this_player()-> query_name() )[0..-2];
            else if( newstr[1..1] == "/" )
		newstr = user_path( (string)this_player()-> query_name() ) +
		  newstr[2..len];
            else if( sscanf( newstr, "~%s/%s", name, rest ) == 2 )
		newstr = user_path( name ) + rest;
	    else
		newstr = user_path( newstr[1..len] )[0..-2];
	    break;
        case "^":
	    newstr = "/domains/" + newstr[1..len];
	    break;
	case "/":
	    break;
	default:
	    newstr = curr + "/" + newstr;
    }

    if( -1 == strsrch( newstr, ".." ) ) return newstr;

    if(newstr[strlen(newstr) - 1] != '/')
        newstr += "/";
    tmp = explode(newstr,"/");
    if (!tmp) tmp = ({"/"});
    for(i = 0; i < sizeof(tmp); i++)
        if(tmp[i] == "..") {
            if(sizeof(tmp) > 2) {
                tmp = tmp[0..(i-2)] + tmp[(i+1)..(sizeof(tmp)-1)];
                i -= 2;
            } else {
                tmp = tmp[2 ..(sizeof(tmp)-1)];
                i = 0;
            }
        }
     newstr = "/" + implode(tmp,"/");
     if(newstr == "//") newstr = "/";
     return newstr;
}
