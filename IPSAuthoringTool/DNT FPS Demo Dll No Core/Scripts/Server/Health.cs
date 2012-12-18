﻿using System;
using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using System.Threading;
using System.ComponentModel;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
{
    public partial class Main : TorqueScriptTemplate
    {
        // Inventory items.  These objects rely on the item & inventory support
        // system defined in item.cs and inventory.cs

        //-----------------------------------------------------------------------------
        // Health Patches cannot be picked up and are not meant to be added to inventory.
        // Health is applied automatically when an objects collides with a patch.
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "HealthPatch", "onCollision", "(%this, %obj, %col)", 3, 1500, false)]
        public void HealthPatchOnCollision(string healthkit_datablock, string healthkit_instance, string player)
        {
            if (ShapeBase.getDamageLevel(player) <= 0.000 || Player.getState(player) == "Dead") return;

            ShapeBase.applyRepair(player, console.GetVarFloat(healthkit_datablock + ".repairAmount"));


            console.Call(healthkit_instance, "schedulePop");

            if (console.GetVarBool(player + ".client"))
            {
                //MessageClient(console.GetVarString(player + ".client"), "MsgHealthPatchUsed", console.ColorEncode(@"\c2Health Patch Applied"));
                //SimObject.schedule(player, "250", "sUpdateHealth", "0");

                //Util._schedule("250", "0", "sUpdateHealth", player,"1");
                //We want to update the health of the player over the next 9 seconds
                //the old system used a Schedule, but we are trying to take
                //as much load as possible off the Torque Server, so this implementation
                //Create a new Backgroundworker thread (which can run in a seperate cpu space)
                //and run our little update process there.
                using (var bwr = new BackgroundWorker())
                {
                    bwr.DoWork += bwr_UpdateHealth;
                    bwr.RunWorkerAsync((object)new HealthKitHelper(player, healthkit_instance));
                }
            }
            //else
            //    player = "";

            AudioServerPlay3D("HealthUseSound", SceneObject.getTransform(player).AsString());
        }

        //[Torque_Decorations.TorqueCallBack("", "", "sUpdateHealth", "(%player,counter)", 2, 1500, false)]
        //public void sUpdateHealth(string player, string counter)
        //    {
        //    if (console.isObject(player))
        //        if (counter.AsInt() < 40)
        //            {
        //            PlayerUpdateHealth(player);
        //            SimObject.schedule(player, "250", "sUpdateHealth", (counter.AsInt() + 1).AsString());
        //            //Util._schedule("250", "0", "sUpdateHealth", player, (counter.AsInt() + 1).AsString());

        //            }
        //    }
        /// <summary>
        /// Class used to pass the player id and healthkit id to a background
        /// thread which updates the player and then deletes the healthkit.
        /// </summary>
        public class HealthKitHelper
        {
            public string player { get; set; }
            public string healthkit { get; set; }
            public HealthKitHelper(string p, string h)
            {
                player = p;
                healthkit = h;
            }
        }
        /// <summary>
        /// This function is used by the above backgroundworker to update the 
        /// player that their health has changed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void bwr_UpdateHealth(object sender, DoWorkEventArgs e)
        {
            string player = ((HealthKitHelper)e.Argument).player;

            string healthkit = ((HealthKitHelper)e.Argument).healthkit;
            if (player == "") return;
            // Would be better to add a onRepair() callback to shapeBase.cpp in order to
            // prevent any excess/unneccesary schedules from this.  But for the time
            // being....

            // This is just a rough timer to update the Health HUD every 250 ms.  From
            // my tests a large health pack will fully heal a player from 10 health in
            // 36 iterations (ie. 9 seconds).  If either the scheduling time, the repair
            // amount, or the repair rate is changed then the healthTimer counter should
            // be changed also.
            for (int i = 0; i < 40; i++)
            {
                //Since this is happening out of process, we need prevent the torque
                //engine from ticking while we do this.  If we don't the memory will
                //get corrupt if two processes enter the Torque DLL at the same time.
                //So we use a Monitor and lock an object which prevents an engine tick
                //from occuring.
                lock (m_ts.tick)
                {
                    //update the player if he is alive.
                    if (console.isObject(player) && Player.getState(player) != "Dead")
                        PlayerUpdateHealth(player);
                    else
                        //No reason to keep updating if they are dead.
                        break;
                }
                //Sleep for 250 milleseconds.
                Thread.Sleep(250);
            }
        }


        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "tossPatch", "(%this)", 1, 1500, false)]
        public string ShapeBaseTossPatch(string thisobj)
        {
            if (!console.isObject(thisobj))
                return string.Empty;

            string item = console.Call_Classname("ItemData", "CreateItem", new string[] { "HealthKitPatch" });
            console.SetVar(item + ".istemp", true);
            //string item = con.Eval("ItemData::CreateItem(HealthKitPatch);", true);
            console.SetVar(string.Format("{0}.sourceObject", item), thisobj);
            console.SetVar(string.Format("{0}.static", item), false);
            SimSet.pushToBack("MissionCleanup", item);

            Random r = new Random();

            Point3F vec = new Point3F(-1 + (float)r.NextDouble() * 2, -1 * (float)r.NextDouble() * 2, (float)r.NextDouble());
            vec = vec.vecotrScale(10);
            Point3F eye = ShapeBase.getEyeVector(thisobj);
            float dot = new Point3F("0 0 1 ").vectorDot(eye);
            if (dot < 0)
                dot = -dot;

            vec = vec + new Point3F("0 0 8").vecotrScale(1 - dot);
            vec = vec + ShapeBase.getVelocity(thisobj);

            TransformF pos = new TransformF(SceneObject.getWorldBox(thisobj).minExtents);
            SceneObject.setTransform(item, pos);
            ShapeBase.applyImpulse(item, pos.MPosition, vec);
            Item.setCollisionTimeout(item, thisobj.AsInt());
            console.Call(item, "schedulePop");
            return item;
        }


    }
}
