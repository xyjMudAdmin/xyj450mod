// 神话世界·西游记·版本４．５０
/* <SecCrypt CPL V3R05> */
//
// 逃犯 (fugitive bounty) quest logic added for xyj450. Stat block left exactly
// as the xyj450 build had it (Decision A: port quest logic, not xyj2006's
// inflated curve). Added: inquiry "逃犯" -> ask_bad, and accept_object turn-in.

inherit NPC;

#include <ansi.h>
#include "greeting.h"

string ask_bad(object me);
int accept_object(object who, object ob);

void create()
{
        set_name("薛仁贵", ({ "xue rengui", "xue", "rengui" }) );
        set("gender", "男性");
        set("age", 35);
        set("long","薛仁贵是长安城九门提督，负责皇宫内外安全。\n");
        set("combat_exp", 350000);
   set("title", "九门提督");
   set("nickname", "勇冠三军");
        set("attitude", "friendly");
   set("per", 25);
        set("str", 35);
        set("max_kee", 850);
        set("max_gin", 600);
   set("max_mana", 500);
   set("mana", 500);
        set("force", 600);
        set("max_force", 600);
        set("force_factor", 35);
        set_skill("spear", 80);
        set_skill("unarmed", 80);
   set_skill("changquan", 70);
        set_skill("parry", 80);
        set_skill("dodge", 80);
   set_skill("bawang-qiang", 70);
   set_skill("yanxing-steps", 70);
   map_skill("spear", "bawang-qiang");
   map_skill("parry", "bawang-qiang");
   map_skill("dodge", "yanxing-steps");
   map_skill("unarmed", "changquan");
        set("inquiry", ([
            "逃犯" : (: ask_bad :),
        ]));
        setup();

        carry_object("/d/obj/weapon/spear/tiespear")->wield();
        carry_object("/d/obj/armor/tongjia")->wear();
}

void init ()
{
   greeting2(this_player());
}

int accept_fight(object me)
{
        command("say 我为殿前将军，岂能与你村野匹夫较艺！");
        return 0;
}

string ask_bad(object me)
{
    me = this_player();
    if ( me->query("office_number") > 300 )
        return ("不敢有劳大驾。");
    if( me->query("combat_exp") < 100000 )
        return ("我怎么能放心你去呢？");
    return "/d/obj/bad"->query_yao(me);
}

int accept_object(object who, object ob)
{
    int i;
    string bname = who->name() + "的" + who->query("bad/name") + "的头颅";

    if ( (string)ob->query("name") != bname )
        return notify_fail("薛仁贵怒道：你竟敢糊弄我！\n");

    if ( !ob->query("killer") || ob->query("killer") != who->query("id") )
        return notify_fail("薛仁贵大怒：不是你杀的也敢拿来，小心我扁你！\n");

    i = who->query("combat_exp");
    who->add("office_number", 1);
    if ( i < 1500 )
        who->add("combat_exp", who->query("combat_exp")/2000 );
    else
        who->add("combat_exp", 1501);
    who->add("potential", 50 + random(100) );
    command("smile");
    command("touch " + who->query("id"));
    tell_object(who, "薛仁贵点点头：不错，应该奖励。\n");
    who->delete("bad");
    return 1;
}
