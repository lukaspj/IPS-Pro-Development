using System;
using System.Collections.Generic;
using WinterLeaf.Classes;
using WinterLeaf.Enums;
using WinterLeaf.Containers;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    // This file contains ShapeBase methods used by all the derived classes

    //-----------------------------------------------------------------------------
    // ShapeBase object
    //-----------------------------------------------------------------------------

    // A raycast helper function to keep from having to duplicate code everytime
    // that a raycast is needed.
    //  %this = the object doing the cast, usually a player
    //  %range = range to search
    //  %mask = what to look for


    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "doRaycast", "(%this, %range, %mask)",  3, 1300, false)]
        public void ShapeBaseDoRayCast(string shapebase, string range, string mask)
            {
            throw new Exception("Not Implemented.");

            //I didn't implement this because rarely would you call it from the script, and it would be
            //better implemented only in csharp.
            }
        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "damage", "(%this, %sourceObject, %position, %damage, %damageType)",  5, 1300, false)]
        public void ShapeBaseDamage(string shapebase, string sourceobject, string position, string damage, string damagetype)
            {
            // All damage applied by one object to another should go through this method.
            // This function is provided to allow objects some chance of overriding or
            // processing damage values and types.  As opposed to having weapons call
            // ShapeBase::applyDamage directly. Damage is redirected to the datablock,
            // this is standard procedure for many built in callbacks.
            if (console.isObject(shapebase))
                console.Call(console.getDatablock(shapebase).AsString(), "damage", new string[] { shapebase, sourceobject, position, damage, damagetype });
            }
        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "setDamageDt", "(%this, %damageAmount, %damageType)",  3, 1300, false)]
        public void ShapeBaseSetDamageDT(string shapebase, string damageAmount, string damageType)
            {
            // This function is used to apply damage over time.  The damage is applied
            // at a fixed rate (50 ms).  Damage could be applied over time using the
            // built in ShapBase C++ repair functions (using a neg. repair), but this
            // has the advantage of going through the normal script channels.

            if (ShapeBase.getDamageState(shapebase) != "Dead")
                {
                
                ShapeBaseDamage(shapebase, "0", "0 0 0", damageAmount, damageType);
                console.SetVar(string.Format("{0}.damageSchedule", shapebase), SimObject.schedule(shapebase, "50", "setDamageDt", damageAmount, damageType));
                }
            else
                {
                console.SetVar(string.Format("{0}.damageSchedule", shapebase), "");
                }
            }
        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "clearDamageDt", "(%this)",  1, 1300, false)]
        public void ShapeBaseClearDamageDt(string shapebase)
            {
            //I could think of soo much better ways of doing this... even if my grammar blows.
            if (console.GetVarString(shapebase + ".damageSchedule") == "") return;
            //con.Eval("cancel(" + con.GetVarString(thisobj + ".damageSchedule") + ");");
            console.Call("cancel", new string[] { console.GetVarString(shapebase + ".damageSchedule") });
            console.SetVar(shapebase + ".damageSchedule", "");
            }
        //-----------------------------------------------------------------------------
        // ShapeBase datablock
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "ShapeBaseData", "damage", "(%this, %obj, %position, %source, %amount, %damageType)",  6, 1300, false)]
        public void ShapeBaseDataDamage(string thisobj, string obj, string position, string source, string amount, string damageType)
            {
            // Ignore damage by default. This empty method is here to
            // avoid console warnings.
            }
        }
    }
