/*    /daemon/terminal.c
 *    from Nightmare IV
 *    daemon storing terminal information
 *    created by Pinkfish@Discworld
 *    rewritten for Nightmare by Descartes of Borg 930903
 */

#include <std.h>

inherit DAEMON;

#define ANSI(p) sprintf("%c["+(p)+"m", 27)
#define ESC(p) sprintf("%c"+(p), 27)

static mapping term_info;

void create() {
    daemon::create();
    set_no_clean(1);
    term_info = ([ "unknown":
      ([ "RESET": "", "BOLD": "", "FLASH":"", "BLACK":"", "RED":"", "BLUE":"",
      "CYAN":"", "MAGENTA":"", "ORANGE":"", "YELLOW":"", "GREEN":"",
      "WHITE":"", "BLACK":"", "B_RED":"", "B_ORANGE":"", "B_YELLOW":"",
      "B_BLACK":"", "B_CYAN":"","B_WHITE":"", "B_GREEN":"",
      "B_MAGENTA":"", "STATUS":"", "WINDOW":"", "INITTERM": "","ENDTERM":""]),
      "ansi": ([ "RESET":ANSI("0;37;40"), "BOLD":ANSI(1), "FLASH":ANSI(5),
      "BLACK":ANSI(30), "RED":ANSI(31), "GREEN":ANSI(32), "ORANGE":ANSI(33),
      "YELLOW":ANSI(1)+ANSI(33), "BLUE": ANSI(34), "CYAN":ANSI(36),
      "MAGENTA":ANSI(35), "BLACK":ANSI(30), "WHITE": ANSI(37),
      "B_RED":ANSI(41), "B_GREEN":ANSI(42), "B_ORANGE":ANSI(43),
      "B_YELLOW":ANSI(1)+ANSI(43), "B_BLUE":ANSI(44),
      "B_CYAN":ANSI(46), "B_BLACK":ANSI(40), "B_WHITE": ANSI(47),
      "CLEARLINE":ESC("[L")+ESC("[G"), "B_MAGENTA":ANSI(45), "STATUS":"", "WINDOW":"", 
      "INITTERM":ESC("[H")+ESC("[2J"), "ENDTERM":"" ]),
      "freedom": ([ "RESET": ESC("G0"), "BOLD":ESC("G@"), "FLASH":ESC("G2"),
      "BLACK":"", "RED":"", "GREEN":"", "ORANGE":"", "YELLOW":"", "BLUE":"",
      "CYAN":"", "MAGENTA":"", "BLACK":"", "WHITE":"", "B_RED":ESC("GD"),
      "B_GREEN": ESC("GD"), "B_ORANGE":ESC("G4"), "B_YELLOW":ESC("G4"),
      "B_BLUE":ESC("G4"), "B_CYAN":ESC("GD"), "B_BLACK": ESC("GD"),
      "B_WHITE":ESC("G4"), "B_MAGENTA":("G4"), "STATUS":"", "WINDOW":"",
      "CLEARLINE":"\r", "INITTERM":"", "ENDTERM":"" ]),
      "ansi-status": ([ "RESET": ANSI("0;37;40"), "BOLD":ANSI(1),
      "FLASH":ANSI(5), "BLACK":ANSI(30), "RED":ANSI(31),
      "GREEN":ANSI(32), "ORANGE":ANSI(33), "YELLOW":ANSI(33), "BLUE":ANSI(34),
      "CYAN": ANSI(36), "MAGENTA": ANSI(35), "BLACK":ANSI(30),
      "WHITE":ANSI(37), "B_RED":ANSI(41), "B_GREEN":ANSI(42),
      "B_ORANGE":ANSI(43), "B_YELLOW": ANSI(1)+ANSI(43), "B_BLUE":ANSI(44),
      "B_CYAN": ANSI(46), "B_BLACK":ANSI(40), "B_WHITE":ANSI(47),
      "B_MAGENTA":ANSI(45), "STATUS":ESC("[23;24r")+ESC(8), 
      "WINDOW":ESC(7)+ESC("[0;22r")+ESC("[22H\n"),
      "INITTERM":ESC("[H")+ESC("[J")+ESC("[23;24r")+ESC("23H\n"),
      "CLEARLINE":"\r", "ENDTERM":ESC("[0r")+ESC("[H")+ESC("[J") ]),
      "xterm": ([ "RESET": ESC(">")+ESC("[1;3;4;5;61")+ESC("[?7h")+ESC("[m"),
      "BOLD": ESC("[7m"), "FLASH":ESC("[5m$<2>"), "BLACK":ANSI(30),
      "RED":ANSI(31), "GREEN":ANSI(32), "ORANGE":ANSI(33), "YELLOW":ANSI(33),
      "BLUE":ANSI(34), "CYAN":ANSI(36), "MAGENTA":ANSI(35),"BLACK":ANSI(34),
      "WHITE":ANSI(38), "B_RED":ANSI(41), "B_GREEN":ANSI(42), 
      "B_ORANGE":ANSI(43), "B_YELLOW":ANSI(43), "B_BLUE":ANSI(44), 
      "B_CYAN":ANSI(46), "B_BLACK":ANSI(40), "B_WHITE": ANSI(47),
      "B_MAGENTA": ANSI(45), "STATUS":"", "WINDOW":"", "INITTERM":"",
      "CLEARLINE":"\r", "ENDTERM":"" ]),
    ]);
}

mapping query_term_info(string type) {
    return (term_info[type] ? term_info[type] : term_info["unknown"]);
}

string *query_terms() { return keys(term_info); }

int query_term_support(string str) {
    return (term_info[str] ? 1 : 0);
}

string no_colours(string str) {
    string *bits;
    int i;

    if(!str) return "";
    i = sizeof(bits = explode(str, "%^"));
    while(i--)
      if(term_info["unknown"][bits[i]]) bits[i] = "";
    return implode(bits, "");
}
