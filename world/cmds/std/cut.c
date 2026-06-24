// cut.c -- behead a corpse to obtain its head (头颅)
// Ported from xyj2006 cmds/std/cut.c for xyj450.
//
// Deviations from the xyj2006 original (both documented, both revertible):
//   1. The upstream code ran destruct(obj) AFTER make_wutoucorpse had already
//      converted that same object into the headless corpse and left it in the
//      room -- i.e. it destroyed the headless body it had just created,
//      contradicting its own "blood still flowing from the headless corpse"
//      message. Here the headless corpse is left to persist (and decays on its
//      own), and a re-cut guard (the headless-corpse name) prevents beheading it
//      twice. To restore verbatim upstream behavior, delete the re-cut guard
//      and add `destruct(obj);` immediately before `return 1;`.
//   2. Upstream required the argument to be the literal word "corpse". That
//      restriction is dropped here: any arg that resolves to a real, whole
//      corpse works (present()+is_corpse() already validate it).

inherit F_CLEAN_UP;

#include <ansi.h>
#include <dbase.h>

int main(object me, string arg)
{
        object obj, toulu, corpse, wield1;
        object *inv;
        string killname;
        int i;

        if( environment(me)->query("no_fight") )
                return notify_fail("这里不准露出兵器。\n");

        if( !arg )
                return notify_fail("你想要谁的头颅？\n");

        obj = present(arg, environment(me));

        if( !obj )
                return notify_fail("这里没有这个死人。\n");

        if( !obj->is_corpse() )
                return notify_fail("你必须先杀了这个人。\n");

        if( obj->query("name")=="无头男尸"
         || obj->query("name")=="无头女尸"
         || obj->query("name")=="性别不明的尸体" )
                return notify_fail("那不是完整的尸体。\n");

        if( obj->query("name")=="腐烂的尸体"
         || obj->query("name")=="腐烂的男尸"
         || obj->query("name")=="腐烂的女尸" )
                return notify_fail("那已是腐烂的尸体了。\n");

        if( obj == me )
                return notify_fail("你想自刎？\n");

        if( !(wield1 = me->query_temp("weapon")) )
                return notify_fail("你应该先有武器。\n");

        if( (string)wield1->query("skill_type") != "sword"
         && (string)wield1->query("skill_type") != "blade" )
                return notify_fail("你应该找把剑，或是刀什么的。\n");

        if( !sscanf(obj->query("name"), "%s的尸体%*s", killname) )
                killname = obj->query("name");

        message_vision("\n$N嚓地一下，把" + killname + "的头砍了下来。\n"
                + HIR + "鲜血溅了$P一身。\n" + NOR, me, obj);

        me->add("bellicosity", 5);
        obj->set("killer", me->query("id"));

        inv = all_inventory(me);
        for(i=0; i<sizeof(inv); i++) {
                if( inv[i]->query("equipped")
                 && inv[i]->query("armor_type")=="cloth"
                 && !(int)inv[i]->query("blood_onit") ) {
                        inv[i]->set("blood_onit", 1);
                        inv[i]->set("name", HIR "粘满鲜血的" NOR + inv[i]->query("name"));
                }
        }

        if( objectp(corpse = CHAR_D->make_wutoucorpse(obj)) )
                message_vision("只看见鲜血还在不断地从" + corpse->query("name")
                        + "的断脖流了出来。\n", me);
        else
                write("不能创建无头尸体，请告诉巫师。\n");

        if( objectp(toulu = CHAR_D->make_toulu(obj)) ) {
                if( obj->query("killer") )
                        toulu->set("killer", obj->query("killer"));
                toulu->move(me);
        } else
                write("不能创建头颅，请告诉巫师。\n");

        return 1;
}

int help(object me)
{
        write(@HELP
指令格式 : cut <尸体>

砍下某个死人的头颅。需先用剑或刀杀死目标。
HELP
        );
        return 1;
}
