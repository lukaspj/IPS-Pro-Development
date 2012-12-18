﻿using System;
using System.Globalization;
using System.Timers;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using System.Collections.Generic;
using System.Threading;
using System.ComponentModel;
using System.Collections.Concurrent;
using WinterLeaf.Enums;
using System.Linq;



namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {

    //-----------------------------------------------------------------------------
    // AIPlayer callbacks
    // The AIPlayer class implements the following callbacks:
    //
    //    PlayerData::onStop(%this,%obj)
    //    PlayerData::onMove(%this,%obj)
    //    PlayerData::onReachDestination(%this,%obj)
    //    PlayerData::onMoveStuck(%this,%obj)
    //    PlayerData::onTargetEnterLOS(%this,%obj)
    //    PlayerData::onTargetExitLOS(%this,%obj)
    //    PlayerData::onAdd(%this,%obj)
    //
    // Since the AIPlayer doesn't implement it's own datablock, these callbacks
    // all take place in the PlayerData namespace.
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // Demo Pathed AIPlayer.
    //-----------------------------------------------------------------------------
    public partial class Main : TorqueScriptTemplate
        {
        private Random r = new Random();

        public UInt32 AiPlayerAiPlayerSpawn(string name, TransformF spawnpoint)
            {
            //warn("----------------------->AIPlayer::spawn(%name,%spawnPoint) (CSHARP)");
            var npcScriptObject = new Torque_Class_Helper("AIPlayer", "");
            npcScriptObject.Props.Add("dataBlock", "DemoPlayer");
            npcScriptObject.Props.Add("path", "");
            UInt32 npcID = npcScriptObject.Create(m_ts);
            if (npcID != 0)
                {
                SimSet.pushToBack("MissionCleanup", npcID.AsString());
                ShapeBase.setShapeName(npcID.ToString("0"), name);
                SceneObject.setTransform(npcID.ToString("0"), spawnpoint);
                console.SetVar(npcID, 1);
                }
            return npcID;
            }

        #region AIPlayer Datablock


        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "onDisabled", "(%this, %obj, %state)", 3, 2500, false)]
        public void DemoPlayerOnDisabled(string datablock, string player, string state)
            {
            console.Call("cancelAll", new string[] { player });

            string aimanager = console.GetVarString(player + ".aiManager");
            if (!console.isObject(aimanager))
                console.error("Bad aiManager");

            ShapeBase.setImageTrigger(player, 0, false);

            string item = console.GetVarString(ShapeBase.getMountedImage(player, WeaponSlot).AsString() + ".item");

            if (r.Next(1, 100) > 80)
                if (console.isObject(item))
                    {
                    int amount = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.image.ammo", item)));

                    if (amount.AsBool())
                        ShapeBaseShapeBaseThrow(player, console.GetVarString(string.Format("{0}.image.clip", item)), "1");
                    }

            ShapeBaseTossPatch(player);
            PlayerPlayDeathCry(player);
            PlayerPlayDeathAnimation(player);

            int ctov = 2000;

            //m_ts.LogError("AIManager " + aimanager + " - Respawning due to death.");
            //Util._schedule("1", "0", "spawnAI", aimanager);
            spawnAI(aimanager);

            SimObject.schedule(player, (ctov - 1000).AsString(), "startFade", "1000", "0", "true");

            //Util._schedule(ctov.AsString(), "0", "delete", player);
            SimObject.schedule(player, ctov.AsString(), "deleteme");
            aiscreated--;
            }
        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "deleteme", "(%this)", 2, 2400, false)]
        public void deleteme(string ai, string t)
            {
            console.Call("cancelAll", new string[] { ai });
            console.Call(ai, "delete");

            }

        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "damage", "(%this, %obj, %sourceObject, %position, %damage, %damageType)", 6, 2400, false)]
        public void DemoPlayerDamage(string datablock, string aiPlayer, string sourceobject, string position, string damage, string damageType)
            {
            if (!console.isObject(aiPlayer) || Player.getState(aiPlayer) == "Dead" || damage.AsFloat() == 0)
                return;

            ShapeBase.applyDamage(aiPlayer, damage.AsFloat());



            Point3F ejectvel = ShapeBase.getVelocity(aiPlayer);

            //ejectvel += new Point3F(0, 0, 5);
            if (ejectvel.z <= 0)
                {
                ejectvel += new Point3F(0, 0, 5);

                ejectvel = ejectvel.vectorScale(console.GetVarFloat(datablock + ".mass"));
                ShapeBase.applyImpulse(aiPlayer, SceneObject.getPosition(aiPlayer), ejectvel);
                }



            Point3F currentpos = SceneObject.getPosition(aiPlayer);
            currentpos.x += r.Next(-5, 5);
            currentpos.y += r.Next(-5, 5);

            if (ShapeBase.getDamageLevel(aiPlayer) > 50)
                {
                currentpos.x += r.Next(-50, 50);
                currentpos.y += r.Next(-50, 50);
                }

            AIPlayer.setMoveDestination(aiPlayer, currentpos, false);


            }


        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "onReachDestination", "(%this,%obj)", 2, 2500, false)]
        public void DemoPlayerOnReachDestination(string datablock, string aiPlayer)
            {
            if (!console.GetVarBool(aiPlayer + ".HoldAndFire"))
                {
                if (console.GetVarString(string.Format("{0}.path", aiPlayer)) == "") return;
                if (console.GetVarString(string.Format("{0}.currentNode", aiPlayer)) == console.GetVarString(string.Format("{0}.targetNode", aiPlayer)))

                    DemoPlayerOnEndOfPath(datablock, aiPlayer, console.GetVarString(string.Format("{0}.path", aiPlayer)));
                else
                    AiPlayerMoveToNextNode(aiPlayer);
                }

            }

        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "onMoveStuck", "(%this,%obj)", 2, 2500, false)]
        public void DemoPlayerOnMoveStuck(string datablock, string aiPlayer)
            {
            string path = console.GetVarString(aiPlayer + ".path");
            uint targetnode = console.GetVarUint(string.Format("{0}.targetNode", datablock));
            string node = SimSet.getObject(path, targetnode).AsString();
            TransformF t = SceneObject.getTransform(node);
            t.MPosition.z += r.Next(10, 100);
            t.MPosition.x += r.Next(10, 30);
            t.MPosition.y += r.Next(10, 30);
            SceneObject.setTransform(aiPlayer, t);
            }

        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "onTargetExitLOS", "(%this,%obj)", 2, 2500, false)]
        public void DemoPlayerOnTargetExitLos(string datablock, string aiPlayer)
            {
            ShapeBase.setImageTrigger(aiPlayer, 0, false);
            }

        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "onTargetEnterLOS", "(%this,%obj)", 2, 2500, false)]
        public void DemoPlayerOnTargetEnterLos(string datablock, string aiPlayer)
            {
            ShapeBase.setImageTrigger(aiPlayer, 0, true);
            
            }

        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "onEndOfPath", "(%this,%obj,path)", 3, 2500, false)]
        public void DemoPlayerOnEndOfPath(string datablock, string aiPlayer, string path)
            {
            AiPlayerNextTask(aiPlayer);

            }

        [Torque_Decorations.TorqueCallBack("", "DemoPlayer", "onEndSequence", "(%this,%obj,path)", 3, 2500, false)]
        public void DemoPlayerOnEndSequence(string datablock, string aiPlayer, int slot)
            {
            //console.print("Sequence Done!");
            ShapeBase.stopThread(aiPlayer, slot);

            AiPlayerNextTask(aiPlayer);

            }




        #endregion

        #region AIPlayer TorqueClass Object

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "spawn", "(%name,%spawnPoint)", 2, 2500, false)]
        public uint AiPlayerSpawn(string name, string spawnPoint)
            {
            var tch = new Torque_Class_Helper("AiPlayer");
            tch.Props.Add("dataBlock", "DemoPlayer");
            tch.Props.Add("path", "");
            uint aIplayer = tch.Create(m_ts);
            SimSet.pushToBack("MissionCleanup", aIplayer.AsString());
            ShapeBase.setShapeName(aIplayer.AsString(), name);
            SceneObject.setTransform(aIplayer.AsString(), new TransformF(spawnPoint));
            return aIplayer;
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "spawnOnPath", "(%name,%path)", 2, 2500, false)]
        public uint AiPlayerSpawnOnPath(string ainame, string path)
            {
            if (!console.isObject(path))
                return 0;

            string node = SimSet.getObject(path, (uint)r.Next(0, SimSet.getCount(path) - 1));
            string transform = SceneObject.getTransform(node).AsString();

            uint player = AiPlayerSpawn(ainame, transform);

            return player;
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "followPath", "(%this,%path,%node)", 3, 2500, false)]
        public void AiPlayerFollowPath(string aiPlayer, string path, string node)
            {
            ShapeBase.stopThread(aiPlayer, 0);

            if (!console.isObject(path))
                {
                console.SetVar(string.Format("{0}.path", aiPlayer), "");
                return;
                }

            if (int.Parse(node) > SimSet.getCount(path) - 1)
                console.SetVar(string.Format("{0}.targetNode", aiPlayer), SimSet.getCount(path) - 1);
            else
                console.SetVar(string.Format("{0}.targetNode", aiPlayer), node);


            if (console.GetVarString(string.Format("{0}.path", aiPlayer)) == path)
                AiPlayerMoveToNode(aiPlayer, console.GetVarInt(aiPlayer + ".currentNode"));

            else
                {
                console.SetVar(string.Format("{0}.path", aiPlayer), path);

                AiPlayerMoveToNode(aiPlayer, 0);
                }
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "moveToNextNode", "(%this)", 1, 2500, false)]
        public void AiPlayerMoveToNextNode(string aiPlayer)
            {
            int targetnode = console.GetVarInt(string.Format("{0}.targetNode", aiPlayer));
            int currentnode = console.GetVarInt(string.Format("{0}.currentNode", aiPlayer));
            string path = console.GetVarString(string.Format("{0}.path", aiPlayer));
            int pathcount = SimSet.getCount(path);
            if ((targetnode < 0) || (currentnode < targetnode))

                AiPlayerMoveToNode(aiPlayer, currentnode < pathcount - 1 ? (currentnode + 1) : 0);
            else

                AiPlayerMoveToNode(aiPlayer, currentnode == 0 ? (pathcount - 1) : (currentnode - 1));
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "moveToNode", "(%this,%index)", 2, 2500, false)]
        public void AiPlayerMoveToNode(string aiPlayer, int index)
            {


            console.SetVar(string.Format("{0}.currentNode", aiPlayer), index);
            string path = console.GetVarString(string.Format("{0}.path", aiPlayer));

            string node = SimSet.getObject(path, (uint)index).AsString();

            bool r = false;

            r = index == console.GetVarInt(string.Format("{0}.targetNode", aiPlayer));

            /*NOTE:  this function creates a memory leak, without it the memory leak is gone,
             * I am assuming the leak is pretty far in, haven't been able to find it yet.
             * I don't recommend using this function.
            */
            if (AIPlayer.getMoveDestination(aiPlayer) != new Point3F(SceneObject.getTransform(node).AsString()))

                AIPlayer.setMoveDestination(aiPlayer, new Point3F(SceneObject.getTransform(node).AsString()), false);
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "pushTask", "(%this,%method)", 2, 2500, false)]
        public void AiPlayerPushTask(string aiPlayer, string method)
            {
            if (console.GetVarString(string.Format("{0}.taskIndex", aiPlayer)) == "")
                {
                console.SetVar(string.Format("{0}.taskIndex", aiPlayer), 0);
                console.SetVar(string.Format("{0}.taskCurrent", aiPlayer), -1);
                }
            console.SetVar(string.Format("{0}.task[{0}.taskIndex]", aiPlayer), method);
            console.Eval(string.Format("{0}.taskIndex++;", aiPlayer));
            if (console.GetVarInt(string.Format("{0}.taskCurrent", aiPlayer)) != -1) return;
            int pt = console.GetVarInt(string.Format("{0}.taskIndex", aiPlayer));


            AiPlayerExecuteTask(aiPlayer, pt - 1);
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "clearTasks", "(%this)", 1, 2500, false)]
        public void AiPlayerClearTasks(string aiPlayer)
            {
            console.SetVar(string.Format("{0}.taskIndex", aiPlayer), 0);
            console.SetVar(string.Format("{0}.taskCurrent", aiPlayer), -1);
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "nextTask", "(%this)", 1, 2500, false)]
        public void AiPlayerNextTask(string aiPlayer)
            {
            if (console.GetVarInt(string.Format("{0}.taskCurrent", aiPlayer)) == -1) return;
            if (console.GetVarInt(string.Format("{0}.taskCurrent", aiPlayer)) < console.GetVarInt(string.Format("{0}.taskIndex", aiPlayer)) - 1)
                {
                int pt = console.GetVarInt(aiPlayer + ".taskCurrent");
                console.SetVar(aiPlayer + ".taskCurrent", pt + 1);

                AiPlayerExecuteTask(aiPlayer, pt + 1);
                }
            else
                console.SetVar(string.Format("{0}.taskCurrent", aiPlayer), -1);
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "executeTask", "(%this,%index)", 2, 2500, false)]
        public void AiPlayerExecuteTask(string aiPlayer, int index)
            {
            console.SetVar(string.Format("{0}.taskCurrent", aiPlayer), index);
            console.error("This probally doens't work and will need debugging");
            console.Eval(string.Format("{0}.{1};", aiPlayer, console.GetVarString(aiPlayer + ".task[" + index + "]")));
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "singleShot", "(%this)", 1, 2500, false)]
        public void AiPlayerSingleShot(string aiPlayer)
            {
            ShapeBase.setImageTrigger(aiPlayer, 0, true);
            ShapeBase.setImageTrigger(aiPlayer, 0, false);
            console.SetVar(string.Format("{0}.trigger", aiPlayer),
                           SimObject.schedule(aiPlayer, console.GetVarString(string.Format("{0}.shootingDelay,singleShot", aiPlayer))));
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "wait", "(%this, %time)", 2, 2500, false)]
        public void AiPlayerWait(string aiPlayer, string time)
            {
            SimObject.schedule(aiPlayer, (time.AsLong() * 1000).AsString(), "nextTask");
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "done", "(%this, %time)", 2, 2500, false)]
        public void AiPlayerDone(string aiPlayer, string time)
            {
            SimObject.schedule(aiPlayer, "0", "delete");
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "fire", "(%this, %bool)", 2, 2500, false)]
        public void AiPlayerFire(string aiPlayer, bool sbool)
            {
            //  if (console.GetVarBool(sbool))
            if (sbool)
                {
                console.Call("cancel", new string[] { console.GetVarString(aiPlayer + ".trigger") });
                //console.Call(aiPlayer, "singleShot");
                AiPlayerSingleShot(aiPlayer);
                }
            else
                console.Call("cancel", new string[] { console.GetVarString(aiPlayer + ".trigger") });
            console.Call(aiPlayer, "nextTask");
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "aimAt", "(%this, %object)", 2, 2500, false)]
        public void AiPlayerAimAt(string aiPlayer, string target)
            {
            console.print(string.Format("aiming at {0}", target));
            console.Call(aiPlayer, "setAimObject", new[] { target });
            console.error(string.Format("{0}.nextTask();", aiPlayer));
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "animate", "(%this,%seq)", 2, 2500, false)]
        public void AiPlayerAnimate(string aiPlayer, string seq)
            {
            Player.setActionThread(aiPlayer, seq, false, true);

            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "getNearestPlayerTarget", "(%this)", 1, 2500, false)]
        public int AIPlayergetNearestPlayerTarget(string aiPlayer)
            {
            if (!console.isObject(aiPlayer))
                return -1;
            Point3F botpos = SceneObject.getPosition(aiPlayer);
            Dictionary<uint, float> playersaround = console.initContainerRadiusSearch(new Point3F(botpos), 30, (uint)SceneObjectTypesAsUint.PlayerObjectType);
            //I would convert this to a linq query, but since they are ordered by dist, and mostlikely I will
            //want the first one or second one, I left it as a loop.
            foreach (KeyValuePair<uint, float> keyValuePair in playersaround)
                {
                if (!console.isObject(keyValuePair.Key.AsString())) continue;
                if (Player.getState(keyValuePair.Key.AsString()) == "Dead") continue;
                if (console.GetVarInt(keyValuePair.Key.AsString() + ".aiteam") == console.GetVarInt(aiPlayer + ".aiteam")) continue;
                return (int)keyValuePair.Key;
                }
            return -1;
            }

        public float AIPlayergetTargetDistance(string aiPlayer, string target)
            {

            return Util.VectorDist(SceneObject.getPosition(target), SceneObject.getWorldBoxCenter(aiPlayer));
            }

        public bool AIPlayerplayerLOS(string aiPlayer, string target)
            {
            uint mask = (uint)(WinterLeaf.Enums.SceneObjectTypesAsUint.StaticObjectType | WinterLeaf.Enums.SceneObjectTypesAsUint.TerrainObjectType);
            string collision = Util.containerRayCast(ShapeBase.getEyePoint(aiPlayer), ShapeBase.getEyePoint(target), mask, aiPlayer, true);
            if (collision == "0")
                return true;

            string hit = collision.Split(' ')[0];
            return hit == target;
            }

        [Torque_Decorations.TorqueCallBack("", "AIPlayer", "think", "(%this,%t)", 2, 2500, false)]
        public void AIPlayerThink(string aiPlayer, string ai)
            {
            //console.Call("cancelAll", new string[] { aiPlayer });

            if (Player.getState(aiPlayer) == "Dead" || !console.isObject(aiPlayer))
                return;

            int nextdelay = AIPlayerCheckForEnemy(aiPlayer) ? 500 : 250;
            m_thoughtqueue.Add(new AIInterval(DateTime.Now.AddMilliseconds(nextdelay), aiPlayer));

            //int sched = SimObject.schedule(aiPlayer, nextdelay.AsString(), "think");
            //console.SetVar(aiPlayer + ".thought", sched);
            }




        public bool AIPlayerCheckForEnemy(string aiPlayer)
            {
            //   ShapeBase.setImageTrigger(aiPlayer, 0, false);
            int nearestplayer = AIPlayergetNearestPlayerTarget(aiPlayer);
            if (nearestplayer != -1)
                {
                float dist = AIPlayergetTargetDistance(aiPlayer, nearestplayer.AsString());
                if (dist < 30)
                    {
                    TransformF t = SceneObject.getTransform(nearestplayer.AsString());
                    AIPlayer.setAimObject(aiPlayer, nearestplayer.AsString());
                    string currentweapon = console.GetVarString(ShapeBase.getMountedImage(aiPlayer, 0) + ".name");

                    if (dist > 25)
                        {
                        if (currentweapon != "LurkerGrenadeLauncherImage")
                            if (ShapeBaseShapeBaseHasInventory(aiPlayer, "LurkerGrenadeAmmo"))
                                ShapeBase.mountImage(aiPlayer, "LurkerGrenadeLauncherImage", 0, true, "");
                            else
                                if (currentweapon != "LurkerWeaponImage")
                                    ShapeBase.mountImage(aiPlayer, "LurkerWeaponImage", 0, true, "");
                        }
                    else
                        {
                        if (currentweapon != "LurkerWeaponImage")
                            ShapeBase.mountImage(aiPlayer, "LurkerWeaponImage", 0, true, "");
                        }

                    console.SetVar(aiPlayer + ".HoldAndFire", true);
                    if (AIPlayerplayerLOS(aiPlayer, nearestplayer.AsString()))
                        {
                        ShapeBase.setImageTrigger(aiPlayer, 0, true);
                        ShapeBase.setImageTrigger(aiPlayer, 0, false);
                        }
                    else
                        {
                        if (currentweapon == "LurkerGrenadeLauncherImage")
                            {
                            ShapeBase.setImageTrigger(aiPlayer, 0, true);
                            ShapeBase.setImageTrigger(aiPlayer, 0, false);
                            }
                        }

                    if (dist < 10)
                        AIPlayer.stop(aiPlayer);
                    else
                        AIPlayer.setMoveDestination(aiPlayer, new Point3F(t.AsString()), false);
                    return false;
                    }
                return false;
                }

            console.SetVar(aiPlayer + ".HoldAndFire", false);
            AIPlayer.clearAim(aiPlayer);
            int currentnode = console.GetVarInt(string.Format("{0}.currentNode", aiPlayer));
            AiPlayerMoveToNode(aiPlayer, currentnode);
            return true;
            }

        #endregion

        [Torque_Decorations.TorqueCallBack("", "", "DumpMC", "", 0, 2500, false)]
        public void DumpMC()
            {
            console.error("Items in Missioncleaup are " + SimSet.getCount("MissionCleanup"));

            }


        private int lastcount = 0;

        [Torque_Decorations.TorqueCallBack("", "", "createAI", "(%numofai", 1, 2500, false)]
        public void createAI(int count)
            {
            if (lastcount > 0)
                {
                console.error("Mobs already spawned");
                return;
                }

            lastcount = count;
            int team = 0;
            for (int i = 0; i < count; i++)
                {
                team++;
                if (team == 3)
                    team = 1;
                string MobRoot = new Torque_Class_Helper("ScriptObject", "Mob" + i.AsString()).Create(m_ts).AsString();
                console.SetVar(MobRoot + ".player", "");
                console.SetVar(MobRoot + ".aiteam", team);
                SimSet.pushToBack("MissionCleanup", MobRoot);
                Util._schedule((i * 100).AsString(), "0", "spawnAI", MobRoot);
                }
            using (var bwr_AIThought = new BackgroundWorker())
                {

                bwr_AIThought.DoWork += new DoWorkEventHandler(bwr_AIThought_DoWork);

                bwr_AIThought.RunWorkerAsync();

                }
            }

        void bwr_AIThought_DoWork(object sender, DoWorkEventArgs e)
            {

            while (lastcount > 0)
                {
                List<AIInterval> t = m_thoughtqueue.toArray().Where(item => item.Intervaltime < DateTime.Now).ToList();
                int counter = 0;
                bool nosleep = false;
                lock (m_ts.tick)

                    foreach (var item in t)
                        {
                        counter++;
                        if (counter > 5)
                            {
                            nosleep = true;
                            break;
                            }
                        m_thoughtqueue.Remove(item);
                        if (console.isObject(item.Player_id))
                            AIPlayerThink(item.Player_id, item.Player_id);//This is happening outside of the T3D main loop.
                        }
                if (!nosleep)
                    Thread.Sleep(100);
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "deleteAI", "", 0, 2500, false)]
        public void deleteAI()
            {
            for (int i = 0; i < lastcount; i++)
                {
                string player = console.GetVarString("Mob" + i.AsString() + ".player");
                console.Call(player, "delete");
                console.Call("Mob" + i.AsString(), "delete");
                }
            lastcount = 0;
            }

        [Torque_Decorations.TorqueCallBack("", "", "spawnAI", "(%this)", 1, 2500, false)]
        public void spawnAI(string aiManager)
            {
            if (!console.isObject(aiManager))
                {
                console.error("Bad aiManager!");
                return;
                }


            string _player = AiPlayerSpawnOnPath("Team" + console.GetVarString(aiManager + ".aiteam"), "MissionGroup/Paths/Path1").AsString();
            if (_player == "0")
                {
                console.error("UNABLE TO SPAWN MONSTER!@!!!!!a");
                Util._schedule((1000).AsString(), "0", "spawnAI", aiManager);
                return;
                }
            if (!console.isObject(_player))
                {
                console.error("UNABLE TO SPAWN MONSTER!@!!!!!");
                Util._schedule((1000).AsString(), "0", "spawnAI", aiManager);
                return;
                }
            //  m_ts.LogError("AIManager " + aiManager + " - Creating.");

            console.Call(_player, "followPath", new[] { "MissionGroup/Paths/Path1", "-1" });
            console.Call(_player, "setMoveSpeed", new[] { ".8" });
            console.SetVar(aiManager + ".player", _player);
            console.SetVar(_player + ".aiManager", aiManager);

            console.SetVar(_player + ".aiteam", console.GetVarString(aiManager + ".aiteam"));

            ShapeBaseShapeBaseSetInventory(_player, "Lurker", "1");
            ShapeBaseShapeBaseSetInventory(_player, "LurkerClip", "1000");
            ShapeBaseShapeBaseSetInventory(_player, "LurkerAmmo", "1000");

            ShapeBaseShapeBaseSetInventory(_player, "LurkerGrenadeLauncher", "1");
            ShapeBaseShapeBaseSetInventory(_player, "LurkerGrenadeAmmo", "10");

            ShapeBaseShapeBaseSetInventory(_player, "ProxMine", "1");


            ShapeBase.mountImage(_player, "LurkerWeaponImage", 0, true, "");



            TransformF t = SceneObject.getTransform(_player);
            t.MPosition.z += r.Next(0, 200);


            SceneObject.setTransform(_player, t);

            m_thoughtqueue.Add(new AIInterval(DateTime.Now.AddMilliseconds(500), _player));

            //int sched = SimObject.schedule(_player, "500", "think");
            //console.SetVar(_player + ".thought", sched);

            aiscreated++;
            }

        private int aiscreated = 0;

        private ConcurrentList<AIInterval> m_thoughtqueue = new ConcurrentList<AIInterval>();

        private class AIInterval
            {
            private DateTime m_intervaltime;

            public DateTime Intervaltime
                {
                get { return m_intervaltime; }
                set { m_intervaltime = value; }
                }
            private string m_player_id;

            public string Player_id
                {
                get { return m_player_id; }
                set { m_player_id = value; }
                }
            public AIInterval(DateTime dt, string p)
                {
                m_intervaltime = dt;
                m_player_id = p;
                }

            }



        private class ConcurrentList<T>
            {
            private object mlock = new object();
            private List<T> m_Objects;
            public ConcurrentList()
                {
                m_Objects = new List<T>();
                }
            public void Add(T obj)
                {
                lock (mlock)
                    m_Objects.Add(obj);
                }

            public T[] toArray()
                {
                T[] temp;
                lock (mlock)
                    temp = m_Objects.ToArray();
                return temp;
                }

            public void Remove(T toremove)
                {
                lock (mlock)
                    m_Objects.Remove(toremove);
                }
            }

        }





    }