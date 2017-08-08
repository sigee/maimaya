#include <std.h>
#include "./virtual.h";

#define LOG(s, w) write_file(VIRTUAL_DIR + s + ".log", w, 0)

inherit DAEMON;

int Px, Py, Mapx, Mapy;
object Room;
string *Map;
mapping Terrains, Exits;

void load_terrains();
void load_exits();
void set_map();
void config_room();
void config_exits();

void create() {
  Map = read_database(VIRTUAL_DIR + "map.db");
  if (!Map) {
    LOG("server", "No map.db in " + VIRTUAL_DIR + "\n");
    return;
  }
  load_terrains();
  load_exits();
}

object compile_object(string fname) {
  if (sscanf(fname, OUTLANDS_DIR + "%d_%d.outlands", Px, Py) != 2) return 0;
  Mapx = Px - 1;
  Mapy = Py - 1;
  if (!Map) return 0;
  if (!Terrains[Map[Mapy][Mapx..Mapx]]) {
    LOG("server", "Unknown sign on map:" + Terrains[Map[Mapy][Mapx..Mapx]] + "\n");
    return 0;
  }
  if (!Terrains[Map[Mapy][Mapx..Mapx]]["file"]) return 0;
  Room = new(VIRTUAL_DIR + Terrains[Map[Mapy][Mapx..Mapx]]["file"]);
  config_room();
  return Room;
}

/*
Terrains (["0" : (["name":"tenger", "map":"%^BLUE%^~~%^RESET%^ ", "file":"tenger.c",]), ])
*/
void load_terrains() {
  string *db, *line;

  if (file_size(VIRTUAL_DIR + "terrains.db") < 0) {
    LOG("server", "No terrains.db file.\n");
    return;
  }
  db = read_database(VIRTUAL_DIR + "terrains.db");
  if (!db) {
    return;
  }
  Terrains = allocate_mapping(sizeof(db));
  for (int i = 0; i < sizeof(db); i++) {
    line = explode(db[i], ":");
    if (sizeof(line) < 3) {
      LOG("server", "Corrupted line " + i + " in terrains.db\n");
      continue;
    }
    Terrains[line[0]] = ([ "name": line[1] , "map": line[2] ]);
    if (sizeof(line) > 3) {
      Terrains[line[0]]["file"]=line[3];
    }
  }
}

string query_terrain(int x, int y) {
  x--;
  y--;
  if (!Map) return "00 ";
// Outlands visszacsatolasa
  if (y < 0) y += sizeof(Map);
  if (y > sizeof(Map) - 1) y -= sizeof(Map);
  if (x < 0) x += sizeof(Map[y]);
  if (x > sizeof(Map[y]) - 1) x -= sizeof(Map[y]);
// Visszacsatolas eddig
  if (y < 0 || x < 0) return "00 ";
  if (sizeof(Map) <= y) return "00 ";
  if (sizeof(Map[y]) <= x) return "00 ";
  return Terrains[Map[y][x..x]]["map"];
}

void set_map() {
  string *im, dirs;
  int y;

#define qt(a, b) query_terrain(Px + a, Py + b)
#define qf(a, b) "%^FLASH%^" + qt(a, b)
#define SZ2 "          "
#define qe(ir, ha, hanem) (Room->query_exit(ir) ? ha : hanem)
  string eENY = qe("eszaknyugat", "ENY","   "),
  eE = qe("eszak", "E", " "),
  eEK = qe("eszakkelet", "EK ", "   "),
  eNY = qe("nyugat", "NY ", "   "),
  eK = qe("kelet", " K ", "   "),
  eDNY = qe("delnyugat", "DNY", "   "),
  eD = qe("del", "D", " "),
  eDK = qe("delkelet", "DK ", "   ");
  string felso = " " + qe("eszaknyugat", "\\", " ") + qe("eszak", "|", " ") + qe("eszakkelet", "/", " ") + " ";
  string kozepso = qe("nyugat", "--", "  ") + "O" + qe("kelet", "--", "  ");
  string also = " " + qe("delnyugat", "/", " ") + qe("del", "|", " ") + qe("delkelet", "\\", " ") + " ";
  Room->set_long(Room->query_long() + "\n"
    + "   "+qt(-1,-2)+qt( 0,-2)+qt( 1,-2)+"   "+SZ2+ eENY + "  " + eE + "  " + eEK + "\n"
    + qt(-2,-1)+qt(-1,-1)+qt( 0,-1)+qt( 1,-1)+qt( 2,-1)+SZ2+ "   " +  felso + "\n"
    + qt(-2, 0)+qt(-1, 0)+qf( 0, 0)+qt( 1, 0)+qt( 2, 0)+SZ2+ eNY  +  kozepso + eK  + "\n"
    + qt(-2, 1)+qt(-1, 1)+qt( 0, 1)+qt( 1, 1)+qt( 2, 1)+SZ2+ "   " +  also + "\n"
    + "   "+qt(-1, 2)+qt( 0, 2)+qt( 1, 2)+"   "+SZ2+ eDNY + "  " + eD + "  " + eDK + "\n");
}

void make_exit(string dir, int x, int y){
  string m;
  int tmpx, tmpy;
  tmpx = Mapx + x;
  tmpy = Mapy + y;
// Outlands visszacsatolasa
  if (tmpy < 0 ) tmpy = sizeof(Map)-1;
  if (tmpy > sizeof(Map) -1 ) tmpy = 0;
  if (tmpx < 0) tmpx = sizeof(Map[tmpy])-1;
  if (tmpx > sizeof(Map[tmpy]) -1 ) tmpx = 0;
// Visszacsatolas eddig
  if (tmpy < 0 || tmpx < 0) return;
  if (tmpy > sizeof(Map)-1 ) return;
  if (tmpx > sizeof(Map[tmpy]) -1) return;
// To override virtual rooms
  if (file_size(VIRTUAL_DIR+"rooms/"+(Px+x)+"_"+(Py+y)+".c") > -1)
   Room->add_exit(dir,VIRTUAL_DIR+"rooms/"+(Px+x)+"_"+(Py+y)+".c");
  m = Map[tmpy][tmpx..tmpx];
  if (!Terrains[m]) return;
  if (Terrains[m]["file"] && Room && !Room->query_exit(dir) ) {
    Room->add_exit(dir, OUTLANDS_DIR + (tmpx + 1) + "_" + (tmpy + 1) + ".outlands");
  }
}

void config_room() {
  make_exit("eszak", 0, -1);
  make_exit("eszakkelet", 1, -1);
  make_exit("kelet", 1, 0);
  make_exit("delkelet", 1, 1);
  make_exit("del", 0, 1);
  make_exit("delnyugat", -1, 1);
  make_exit("nyugat", -1, 0);
  make_exit("eszaknyugat", -1, -1);

  config_exits();

  set_map();
}

void load_exits() {
  string *exs, *tmp, id;

  exs = read_database(VIRTUAL_DIR + "exits.db");
  if (!exs) return;
  Exits = allocate_mapping(sizeof(exs));
  for (int i = 0; i < sizeof(exs); i++) {
    if (exs[i][0] == '<') continue;
    tmp = explode(exs[i], ":");
    if (sizeof(tmp) < 3) {
      LOG("server", "Corrupted line:" + exs[i] + " in exits.db\n");
    }
    else{
      if (!Exits[tmp[0]]) {
        Exits[tmp[0]] = ({tmp[1] + ":" + tmp[2]});
      }
      else {
        Exits[tmp[0]] += ({tmp[1] + ":" + tmp[2]});
      }
    }
  }
}

void config_exits() {
  string id, *tmp;

  id = "" + Px + "_" + Py;
  if (!Exits || !Exits[id]) return;
  for (int i = 0; i < sizeof(Exits[id]); i++) {
    tmp = explode(Exits[id][i], ":");
    Room->add_exit(tmp[0], tmp[1]);
  }
}
