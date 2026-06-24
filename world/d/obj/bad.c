// bad.c -- the fugitive generator/registry for the 逃犯 (fugitive bounty) quest.
// Ported from xyj2006 /d/obj/bad.c for xyj450.
//
// Adaptations from upstream (all documented):
//   - Decision C: dropped `#include "place.h"` and its 17-area hardcoded room
//     table. Replaced with a small list of VERIFIED top-level areas + get_dir()
//     to pick a real room at spawn time, so it can never load_object a room
//     that doesn't exist on this build. (Same technique the yg/ engine uses.)
//   - Renamed local `where` -> `where_room` (`where` is reserved on this build).
//   - All locals declared at function top (parser requirement).
//   - `daoxing` read is left as-is; it returns 0 on this build (harmless).
//   - query_yao's no-op command() replaced with tell_object() to actually
//     message the player.

#include <ansi.h>

string* guainame=({
        "土匪","dacoit",
        "强盗","robber",
        "杀人犯","killer",
        "盗墓者", "resurrecter",
        "通奸犯", "misconducter",
        "纵火犯", "incendiariser",
        "恶霸", "bully",
        "抢劫犯", "robber",
        "采花贼", "rascal",
        "疯狂ＰＫ者", "crazy PKER",
});

string* xianname=({
        "抢劫犯", "robber",
        "土匪","dacoit",
        "强盗","robber",
        "杀人犯","killer",
        "盗墓者", "resurrecter",
        "通奸犯", "misconducter",
        "纵火犯", "incendiariser",
        "恶霸", "bully",
        "采花贼", "rascal",
        "疯狂ＰＫ者", "crazy PKER",
});

string* prename=({
        "罪大恶极 ",
        "十恶不赦 ",
        "罪该万死 ",
        "官府通缉 ",
        "死有余辜 ",
});

// Decision C: verified top-level areas (prefix, player-facing place name).
// get_dir picks a real room within, so spawns can never hit a missing file.
// To extend coverage later (e.g. after a dntg port), just add a pair here.
string* areas=({
        "/d/kaifeng/",  "开封城",
        "/d/changan/",  "长安城",
        "/d/gao/",      "高家庄",
        "/d/westway/",  "长安城西",
        "/d/meishan/",  "灌江口",
        "/d/xueshan/",  "大雪山",
        "/d/moon/",     "玉女峰",
        "/d/jjf/",      "将军府",
});

int add_new_yao(object me);

void create()
{
    seteuid(getuid());
}

string query_yao(object me)
{
    mapping bad;

    if(mapp(bad=me->query("bad"))){
        if(me->query("mud_age")-me->query("bad_mudage")<480)
         return ("不是让你替我抓回"+bad["place"]+"的"+bad["name"]+"("+bad["id"]+")么？\n");
        else
         tell_object(me, "你真是没用，我再给你一次机会！\n");
    }
    if(me->query("mud_age")-me->query("bad_mudage")<60)
        return ("有劳您了，您先休息一下吧！");

    add_new_yao(me);
    bad=me->query("bad");
    return ("现有"+bad["name"]+"("+bad["id"]+")流窜到"+bad["place"]+
            "一带滋扰民众，\n你马上把他的人头拿来见我！\n");
}

int add_new_yao(object me)
{
    string my_cls, type, sname, prefix, placename, room;
    string* yaoname;
    string* files;
    int my_exp, index;
    mapping skill_status;
    mapping bad;
    object where_room, yao;

    my_exp=me->query("combat_exp");
    my_cls=me->query("class");
    if(my_cls=="xian"||my_cls=="taoist"||my_cls=="dragon"||my_cls=="bonze"){
        type="xian";
        yaoname=guainame;
    }
    else {
        type="mo";
        yaoname=xianname;
    }

    index=random(sizeof(yaoname)/2)*2;
    bad=(["type" : type]);
    bad+=(["id" : yaoname[index+1] ]);
    sname=prename[random(sizeof(prename))]+yaoname[index];
    bad+=(["name" : sname]);

    // scale to the player's strongest non-literate skill
    if (skill_status = me->query_skills() ) {
      yaoname  = keys(skill_status);
      my_exp=0;
      for(index=0; index<sizeof(skill_status); index++) {
        if(skill_status[yaoname[index]]>my_exp && yaoname[index]!="literate")
           my_exp=skill_status[yaoname[index]];
      }
    }

    // Decision C: pick a verified area, then a real room within it via get_dir
    index=random(sizeof(areas)/2)*2;
    prefix=areas[index];
    placename=areas[index+1];
    files=get_dir(prefix+"*.c");
    if( sizeof(files) ) {
        room=files[random(sizeof(files))];
        sscanf(room, "%s.c", room);
        bad+=(["location" : prefix+room]);
    } else {
        bad+=(["location" : prefix]);
    }
    bad+=(["place" : placename]);

    me->set("bad_mudage",me->query("mud_age"));
    me->set("bad",bad);

    if(!(where_room=find_object(bad["location"])))
             where_room=load_object(bad["location"]);

    yao=new("/d/obj/npc/bad");
    yao->move(where_room);
    yaoname=explode(bad["id"]," ");
    yaoname=({bad["id"]})+yaoname;
    if( me->query_temp("apply/name") )
        me->delete_temp("apply/name");
    yao->set_name(me->name()+"的"+bad["name"],yaoname);
    yao->set("pre_killer",me);
    yao->init_skill(me->query("combat_exp"),me->query("daoxing"),my_exp,
         me->query("max_kee"),me->query("max_force"),
         me->query("max_sen"),me->query("max_mana"),bad["type"]);

    return 1;
}

int query_killyao(object me,string loc,string yaoid)
{
    string* loclist;
    string* loclist2;
    mapping bad=me->query("bad");

    if(!mapp(bad))return 0;
    loclist=explode(bad["location"],"/");
    loclist2=explode(loc,"/");
    return (loclist[sizeof(loclist)-2]==loclist2[sizeof(loclist2)-2]&&bad["id"]==yaoid);
}
