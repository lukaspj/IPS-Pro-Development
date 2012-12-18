﻿using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
using WinterLeaf;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        public const string Game__DefaultPlayerClass = "Player";

        public const string Game__DefaultPlayerDataBlock = "DefaultPlayerData";

        public const string Game__DefaultPlayerSpawnGroups = "PlayerSpawnPoints PlayerDropPoints";

        public const string Game__DefaultCameraClass = "Camera";

        public const string Game__DefaultCameraDataBlock = "Observer";

        public const string Game__DefaultCameraSpawnGroups = "CameraSpawnPoints PlayerSpawnPoints PlayerDropPoints";

        public bool Game_Running = false;


        public bool isScriptFileThenRun(string filename)
            {
            if (console.Call("isScriptFile", new string[] { filename }).AsBool())
                {
                Util.exec(filename, false, false);
                return true;
                }
            return false;

            }

        [Torque_Decorations.TorqueCallBack("", "", "onServerCreated", "", 0, 51, false)]
        public void onServerCreated()
            {
            console.SetVar("$Server::GameType", console.GetVarString("$appName"));
            console.SetVar("$Server::MissionType", "Deathmatch");
            console.SetVar("$Game::StartTime", 0);

            console.Call("physicsInitWorld", new string[] { "server" });

            isScriptFileThenRun("art/shapes/particles/managedParticleData.cs");
            isScriptFileThenRun("art/decals/managedDecalData.cs");
            isScriptFileThenRun("art/datablocks/managedDatablocks.cs");
            isScriptFileThenRun("art/forest/managedItemData.cs");
            isScriptFileThenRun("art/datablocks/datablockExec.cs");
            isScriptFileThenRun("creations/exec.cs");
            console.SetVar("$Game::StartTime", console.GetVarString("$Sim::Time"));
            dnt.DoScriptInjection(ScriptType.Server, 100, 3100);
            }

        [Torque_Decorations.TorqueCallBack("", "", "onServerDestroyed", "", 0, 51, false)]
        public void onServerDestroyed()
            {
            console.Call("physicsDestroyWorld", new[] { "server" });
            }

        [Torque_Decorations.TorqueCallBack("", "", "onMissionLoaded", "", 0, 51, false)]
        public void onMissionLoaded()
            {
            Util._physicsStopSimulation("server");

            Game_Running = false;

            startGame();
            }


        [Torque_Decorations.TorqueCallBack("", "", "startGame", "", 0, 51, false)]
        public void startGame()
            {

            if (Game_Running)
                {
                console.error("startGame: End the game first!");
                return;
                }


            foreach (UInt32 clientid in ClientGroup)
                {
                console.commandToClient(clientid.AsString(), "GameStart");
                console.SetVar(string.Format("{0}.score", clientid), 0);
                console.SetVar(string.Format("{0}.kills", clientid), 0);
                console.SetVar(string.Format("{0}.deaths", clientid), 0);
                }

            Game_Running = true;
            }


        // ============================================================================  
        //  Client Management  
        // ============================================================================  

        // ----------------------------------------------------------------------------  
        // GameConnection Methods  
        // ----------------------------------------------------------------------------  
        // GameConnection manages the communication between the server's world and the  
        // client's simulation. These functions are responsible for maintaining the  
        // client's camera and player objects.  

        // These methods are extensions to the GameConnection class. Extending  
        // GameConnection makes it easier to deal with some of this functionality,  
        // but these could also be implemented as stand-alone functions.  
        //-----------------------------------------------------------------------------  


        //-----------------------------------------------------------------------------  
        // This is the main entry point for spawning a control object for the client.  
        // The control object is the actual game object that the client is responsible  
        // for controlling in the client and server simulations. We also spawn a  
        // convenient camera object for use as an alternate control object. We do not  
        // have to spawn this camera object in order to function in the simulation. 
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "spawnCamera", "(%this, %spawnPoint)", 2, 51, false)]
        public void GameConnectionSpawnCamera(string thisobj, string spawnPoint)
            {

            string camera = console.GetVarString(thisobj + ".camera");

            // Set the control object to the default camera
            if (!console.isObject(camera))
                {
                camera = Util._spawnObject(Game__DefaultCameraClass, Game__DefaultCameraDataBlock).AsString();
                console.SetVar(thisobj + ".camera", camera);
                }

            // If we have a camera then set up some properties
            if (!console.isObject(camera))
                return;

            SimSet.pushToBack("MissionCleanup", camera);

            NetObject.scopeToClient(camera, thisobj);

            GameConnection.setControlObject(thisobj, camera);

            if (!console.Call("isDefined", new[] { spawnPoint }).AsBool())
                return;

            // Attempt to treat %spawnPoint as an object

            if (Util.getWordCount(spawnPoint) == 1 && console.isObject(spawnPoint))
                SceneObject.setTransform(camera, SceneObject.getTransform(spawnPoint));
            else
                SceneObject.setTransform(camera, new TransformF(spawnPoint));
            }


        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onClientEnterGame", "%client", 1, 51, false)]
        public void GameConnectionOnClientEnterGame(string client)
            {
            // Sync the client's clocks to the server's
            string simtime = console.GetVarString("$Sim::Time");

            string starttime = string.Format("0{0}", console.GetVarString(" $Game::StartTime").Trim());

            double result = double.Parse(simtime) - double.Parse(starttime);

            console.commandToClient(client, "SyncClock", new[] { result.AsString() });

            // Find a spawn point for the camera
            // This function currently relies on some helper functions defined in
            // core/scripts/server/spawn.cs. For custom spawn behaviors one can either
            // override the properties on the SpawnSphere's or directly override the
            // functions themselves.
            string cameraSpawnPoint = PickCameraSpawnPoint(Game__DefaultCameraSpawnGroups);//console.GetVarString("$Game::DefaultCameraSpawnGroups"));
            //%cameraSpawnPoint = pickCameraSpawnPoint($Game::DefaultCameraSpawnGroups);
            // Spawn a camera for this client using the found %spawnPoint

            GameConnectionSpawnCamera(client, cameraSpawnPoint);


            // Setup game parameters, the onConnect method currently starts
            // everyone with a 0 score.
            console.SetVar(string.Format("{0}.score", client), 0);
            console.SetVar(string.Format("{0}.kills", client), 0);
            console.SetVar(string.Format("{0}.deaths", client), 0);

            // weaponHUD

            GameConnectionRefreshWeaponHud(client, "0", "", "", "", "");

            // Prepare the player object.

            GameConnectiOnPreparePlayer(client);

            MessageClient(client, "MsgClientJoin", "Current Players: " + ClientGroup.Count);

            // Inform the client we've joined up
            MessageClient(
                client,
                "MsgClientJoin",
                @"\c2Welcome to the Torque demo app %1.",
                console.GetVarString(client + ".playerName"),
                client,
                console.GetVarString(client + ".sendGuid"),
                console.GetVarString(client + ".team"),
                console.GetVarString(client + ".score"),
                console.GetVarString(client + ".kills"),
                console.GetVarString(client + ".deaths"),
                console.GetVarString(client + ".isAiControlled"),
                console.GetVarString(client + ".isAdmin"),
                console.GetVarString(client + ".isSuperAdmin")
                );

            //// Inform all the other clients of the new guy

            MessageAllExcept(
                client,
                "-1",
                "MsgClientJoin",
                @"\c1%1 joined the game.",
                                console.GetVarString(client + ".playerName"),
                client,
                console.GetVarString(client + ".sendGuid"),
                console.GetVarString(client + ".team"),
                console.GetVarString(client + ".score"),
                console.GetVarString(client + ".kills"),
                console.GetVarString(client + ".deaths"),
                console.GetVarString(client + ".isAiControlled"),
                console.GetVarString(client + ".isAdmin"),
                console.GetVarString(client + ".isSuperAdmin")
                );

            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "preparePlayer", "%client", 1, 51, false)]
        public void GameConnectiOnPreparePlayer(string client)
            {
            string playerSpawnPoint = PickPlayerSpawnPoint(Game__DefaultPlayerSpawnGroups);//console.GetVarString("$Game::DefaultPlayerSpawnGroups"));

            if (SpawnPlayer(client, playerSpawnPoint, false))

                GameConnectionLoadOut(client, console.GetVarString(client + ".player"));
            //console.SetVar(client + ".isrespawning", false);
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onClientLeaveGame", "%client", 1, 51, false)]
        public void GameConnectionOnClientLeaveGame(string client)
            {
            if (console.isObject(console.GetVarString(string.Format("{0}.camera", client))))
                console.Call(string.Format("{0}.camera", client), "delete");

            if (console.isObject(console.GetVarString(string.Format("{0}.player", client))))
                console.Call(string.Format("{0}.player", client), "delete");
            }


        // ----------------------------------------------------------------------------  
        //  Functions that implement game-play  
        // ----------------------------------------------------------------------------  

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "loadOut", "%client,%player", 2, 51, false)]
        public void GameConnectionLoadOut(string client, string player)
            {
            ShapeBaseClearWeaponCycle(player);

            ShapeBaseShapeBaseSetInventory(player, "Ryder", "1");
            ShapeBaseShapeBaseSetInventory(player, "RyderClip", ShapeBaseShapeBaseMaxInventory(player, "RyderClip"));
            ShapeBaseShapeBaseSetInventory(player, "RyderAmmo", ShapeBaseShapeBaseMaxInventory(player, "RyderAmmo"));

            ShapeBaseAddToWeaponCycle(player, "Ryder");


            ShapeBaseShapeBaseSetInventory(player, "Lurker", "1");
            ShapeBaseShapeBaseSetInventory(player, "LurkerClip", ShapeBaseShapeBaseMaxInventory(player, "LurkerClip"));
            ShapeBaseShapeBaseSetInventory(player, "LurkerAmmo", ShapeBaseShapeBaseMaxInventory(player, "LurkerAmmo"));

            ShapeBaseAddToWeaponCycle(player, "Lurker");

            ShapeBaseShapeBaseSetInventory(player, "LurkerGrenadeLauncher", "1");
            ShapeBaseShapeBaseSetInventory(player, "LurkerGrenadeAmmo", ShapeBaseShapeBaseMaxInventory(player, "LurkerGrenadeAmmo"));

            ShapeBaseAddToWeaponCycle(player, "LurkerGrenadeLauncher");

            ShapeBaseShapeBaseSetInventory(player, "ProxMine", ShapeBaseShapeBaseMaxInventory(player, "ProxMine"));

            ShapeBaseAddToWeaponCycle(player, "ProxMine");



            //ShapeBaseShapeBaseSetInventory(player, "DeployableTurret", ShapeBaseShapeBaseMaxInventory(player, "DeployableTurret"));
            //con.Eval(player + ".addToWeaponCycle(DeployableTurret);", false);


            string junk = console.GetVarString(string.Format("{0}.mainWeapon.image", console.getDatablock(player).ToString("0")));
            ShapeBase.mountImage(player, junk == "" ? junk : "LurkerWeaponImage", 0, true, "");
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onLeaveMissionArea", "%client", 1, 51, false)]
        public void GameConnectionOnLeaveMissionArea(string client)
            {
            MessageClient(client, "MsgClientJoin", console.ColorEncode(@"\c2Now leaving the mission area!"));
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onEnterMissionArea", "%client", 1, 51, false)]
        public void GameConnectionOnEnterMissionArea(string client)
            {
            MessageClient(client, "MsgClientJoin", console.ColorEncode(@"\c2Now entering the mission area!"));
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onDeath", "%client, %sourceObject, %sourceClient, %damageType, %damLoc", 5, 51, false)]
        public void GameConnectionOnDeath(string client, string sourceobject, string sourceclient, string damagetype, string damloc)
            {
            if (client != "")
                {
                if (console.GetVarString(string.Format("{0}.ownedTurrets", client)) == "")
                    {
                    console.SetVar(client + ".ownedTurrets", new Torque_Class_Helper("SimSet").Create(m_ts));
                    }

                string simSetId = console.GetVarString(string.Format("{0}.ownedTurrets", client));

                int countofitems = SimSet.getCount(simSetId);
                for (uint i = 0; i < countofitems; i++)
                    {
                    string turret = SimSet.getObject(simSetId, i).AsString();
                    SimObject.schedule(turret, "10", "delete");
                    }
                }


            // clear the weaponHUD

            GameConnectionRefreshWeaponHud(client, "0", "", "", "", "");

            string player = console.GetVarString(client + ".player");


            // Clear out the name on the corpse
            ShapeBase.setShapeName(player, "");

            // Update the numerical Health HUD

            PlayerUpdateHealth(player);



            // Switch the client over to the death cam and unhook the player object.
            string camera = console.GetVarString(client + ".camera");

            if (console.isObject(camera) && console.isObject(player))
                {
                CameraSetMode(camera, "Corpse", player, "", "");


                GameConnection.setControlObject(client, camera);
                }
            else
                {
                console.print("------------>Failed to Switch the client over to the death cam.");
                }

            console.SetVar(client + ".player", "0");

            // Display damage appropriate kill message
            string sendMsgFunction = "sendMsgClientKilled_" + damagetype;
            if (!console.isFunction(sendMsgFunction))
                {
                sendMsgFunction = "sendMsgClientKilled_Default";
                }

            console.Call(sendMsgFunction, new string[] { "MsgClientKilled", client, sourceclient, damloc });
            // Dole out points and check for win
            if ((damagetype == "Suicide") || (sourceclient == client))
                {
                IncDeaths(client, "1", "1");
                IncScore(client, "-1", "0");
                }
            else
                {
                IncDeaths(client, "1", "0");
                IncScore(sourceclient, "1", "1");
                IncKills(sourceclient, "1", "0");
                }

            }


        [Torque_Decorations.TorqueCallBack("", "", "incKills", "(%client, %kill, %dontMessageAll)", 3, 51, false)]
        public void IncKills(string client, string kill, string dontmessageall)
            {
            //if (!console.isObject(client)) return;
            //console.SetVar(client + ".kills", console.GetVarInt(client + ".kills") + int.Parse(kill));
            //if (dontmessageall == "0")
            //    MessageAll("MsgClientScoreChanged", "", console.GetVarString(client + ".score"),
            //               console.GetVarString(client + ".kills"), console.GetVarString(client  + "deaths"), client);
            }

        [Torque_Decorations.TorqueCallBack("", "", "incDeaths", "(%client, %kill, %dontMessageAll)", 3, 51, false)]
        public void IncDeaths(string client, string death, string dontmessageall)
            {
            //if (!console.isObject(client)) return;
            //console.SetVar(string.Format("{0}.deaths", client), console.GetVarInt(string.Format("{0}.deaths", client)) + int.Parse(death));
            //if (dontmessageall == "0")
            //    MessageAll("MsgClientScoreChanged", "", console.GetVarString(client + ".score"),
            //               console.GetVarString(client + ".kills"), console.GetVarString(client + ".deaths"), client);
            }

        [Torque_Decorations.TorqueCallBack("", "", "incScore", "(%client, %kill, %dontMessageAll)", 3, 51, false)]
        public void IncScore(string client, string score, string dontmessageall)
            {
            //if (!console.isObject(client)) return;
            //console.SetVar(client + ".score", console.GetVarInt(client + ".score") + int.Parse(score));
            //if (dontmessageall == "0")
            //    MessageAll("MsgClientScoreChanged", "", console.GetVarString(client + ".score"),
            //               console.GetVarString(client + "kills"), console.GetVarString(client + ".deaths"), client);
            }

        [Torque_Decorations.TorqueCallBack("", "", "getScore", "(%%client)", 1, 51, false)]
        public int GetScore(string client)
            {
            return console.GetVarInt(string.Format("{0}.score", client));
            }

        [Torque_Decorations.TorqueCallBack("", "", "getKills", "(%%client)", 1, 51, false)]
        public int GetKills(string client)
            {
            return console.GetVarInt(string.Format("{0}.kills", client));
            }

        [Torque_Decorations.TorqueCallBack("", "", "getDeaths", "(%%client)", 1, 51, false)]
        public int GetDeaths(string client)
            {
            return console.GetVarInt(string.Format("{0}.deaths", client));
            }






        [Torque_Decorations.TorqueCallBack("", "GameConnection", "setAmmoAmountHud", "(%client, %amount, %amountInClips )", 3, 51, false)]
        public void GameConnectionSetAmmoAmountHud(string client, string amount, string amountinClips)
            {
            if (console.isObject(client))
                {
                console.commandToClient(client, "SetAmmoAmountHud", new string[] { amount, amountinClips });
                }
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "RefreshWeaponHud", "(%client, %amount, %preview, %ret, %zoomRet, %amountInClips)", 6, 51, false)]
        public void GameConnectionRefreshWeaponHud(string client, string amount, string preview, string ret, string zoomRet, string amountInClips)
            {
            console.commandToClient(client, "RefreshWeaponHud", new string[] { amount, preview, ret, zoomRet, amountInClips });
            }

        [Torque_Decorations.TorqueCallBack("", "", "sendMsgClientKilled_Impact", "(%msgType, %client, %sourceClient, %damLoc )", 4, 51, false)]
        public void SendMsgClientKilledImpact(string msgtype, string client, string sourceclient, string damloc)
            {
            if (console.isObject(client))
                {
                MessageAll(msgtype, "%1 fell to his death!", console.GetVarString(string.Format("{0}.playerName", client)));
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "sendMsgClientKilled_Suicide", "(%msgType, %client, %sourceClient, %damLoc )", 4, 51, false)]
        public void SendMsgClientKilledSuicide(string msgtype, string client, string sourceclient, string damloc)
            {
            if (console.isObject(client))
                {
                MessageAll(msgtype, "%1 takes his own life!", console.GetVarString(string.Format("{0}.playerName", client)));
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "sendMsgClientKilled_Default", "(%msgType, %client, %sourceClient, %damLoc )", 4, 51, false)]
        public void SendMsgClientKilledDefault(string msgtype, string client, string sourceclient, string damloc)
            {
            if (console.isObject(client))
                {
                if (sourceclient == client)
                    SendMsgClientKilledSuicide(msgtype, client, sourceclient, damloc);

                else if (console.GetVarString(string.Format("{0}.team", sourceclient)) != "" &&
                         console.GetVarString(string.Format("{0}.team", sourceclient)) != console.GetVarString(string.Format("{0}.team", client)))
                    MessageAll(msgtype, "%1 killed by %2 - friendly fire!", console.GetVarString(string.Format("{0}.playerName", client)),
                               console.GetVarString(string.Format("{0}.playerName", sourceclient)));
                else
                    {
                    if (console.isObject(sourceclient))
                        MessageAll(msgtype, "%1 gets nailed by %2!", console.GetVarString(string.Format("{0}.playerName", client)),
                                   console.GetVarString(string.Format("{0}.playerName", sourceclient)));
                    else
                        MessageAll(msgtype, "%1 gets nailed by %2!", console.GetVarString(string.Format("{0}.playerName", client)), "a Bot!");
                    }
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "EditorIsActive", "()", 0, 51, false)]
        public bool EditorIsActive()
            {
            return (console.isObject("EditorGui") && (GuiCanvas.getContent("Canvas").AsString() == console.Call("EditorGui", "getId")));
            }

        [Torque_Decorations.TorqueCallBack("", "", "GuiEditorIsActive", "()", 0, 51, false)]
        public bool GuiEditorIsActive()
            {
            return (console.isObject("GuiEditorGui") && (GuiCanvas.getContent("Canvas").AsString() == console.Call("GuiEditorGui", "getId")));
            }


        [Torque_Decorations.TorqueCallBack("", "", "pickCameraSpawnPoint", "(%spawnGroups)", 1, 51, false)]
        public string PickCameraSpawnPoint(string spawnGroups)
            {
            string spawnpoint = string.Empty;
            String[] lspawngroups = spawnGroups.Split(' ');
            foreach (string group in lspawngroups)
                {
                if (console.isObject(group))
                    spawnpoint = SimSet.getRandom(group);

                if (console.isObject(spawnpoint))
                    return spawnpoint;
                }
            if (!console.isObject("DefaultCameraSpawnSphere"))
                {
                var spawn = new Torque_Class_Helper("SpawnSphere", "DefaultCameraSpawnSphere");
                spawn.Props.Add("dataBlock", "SpawnSphereMarker");
                spawn.PropsAddString("spawnClass", Game__DefaultCameraClass);
                spawn.PropsAddString("spawnDatablock", Game__DefaultCameraDataBlock);
                UInt32 spawnid = spawn.Create(m_ts);
                SimSet.pushToBack("MissionCleanup", spawnid.AsString());
                }

            return console.GetVarString("DefaultCameraSpawnSphere");
            }

        [Torque_Decorations.TorqueCallBack("", "", "pickPlayerSpawnPoint", "(%spawnGroups)", 1, 51, false)]
        public string PickPlayerSpawnPoint(string spawnGroups)
            {
            string spawnpoint = string.Empty;
            String[] lspawngroups = spawnGroups.Split(' ');
            foreach (string group in lspawngroups)
                {
                if (console.isObject(group))

                    spawnpoint = SimSet.getRandom(group);
                if (console.isObject(spawnpoint))
                    return spawnpoint;
                }
            if (!console.isObject("DefaultPlayerSpawnSphere"))
                {
                var spawn = new Torque_Class_Helper("SpawnSphere", "DefaultPlayerSpawnSphere");
                spawn.Props.Add("dataBlock", "SpawnSphereMarker");
                spawn.PropsAddString("spawnClass", Game__DefaultPlayerClass);
                spawn.PropsAddString("spawnDatablock", Game__DefaultPlayerDataBlock);
                UInt32 spawnid = spawn.Create(m_ts);
                SimSet.pushToBack("MissionCleanup", spawnid.AsString());
                }
            return console.GetVarString("DefaultPlayerSpawnSphere");
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "spawnPlayer", "(%client, %spawnPoint, %noControl)", 3, 51, false)]
        public bool SpawnPlayer(string client, string spawnpoint, bool nocontrol)
            {
            string player = console.GetVarString(client + ".player");

            if (console.isObject(player))
                {
                console.error("Attempting to create a player for a client that already has one!");
                return false;
                }

            if ((spawnpoint.Split(' ').GetUpperBound(0) == 0) && (console.isObject(spawnpoint)))
                {
                // Attempt to treat %spawnPoint as an object
                string spawnclass = Game__DefaultPlayerClass;
                string spawndatablock = Game__DefaultPlayerDataBlock;
                // Overrides by the %spawnPoint
                if (console.GetVarString(string.Format("{0}.spawnClass", spawnpoint)).Trim() != "")
                    {
                    spawnclass = console.GetVarString(string.Format("{0}.spawnClass", spawnpoint));
                    spawndatablock = console.GetVarString(string.Format("{0}.spawnDataBlock", spawnpoint));
                    }
                else if (console.GetVarString(string.Format("{0}.spawnDatablock", spawnpoint)).Trim() != "")
                    {
                    // This may seem redundant given the above but it allows
                    // the SpawnSphere to override the datablock without
                    // overriding the default player class
                    spawndatablock = console.GetVarString(string.Format("{0}.spawnDatablock", spawnpoint));
                    }
                string spawnproperties = console.GetVarString(string.Format("{0}.spawnProperties", spawnpoint));
                string spawnScript = console.GetVarString(string.Format("{0}.spawnScript", spawnpoint));

                // Spawn with the engine's Sim::spawnObject() function
                player = console.SpawnObject(spawnclass, spawndatablock, "", spawnproperties, spawnScript).ToString(CultureInfo.InvariantCulture);
                // If we have an object do some initial setup
                if (console.isObject(player))
                    {
                    // Pick a location within the spawn sphere.
                    string spawnlocation = PointInSpawnSphere(player, spawnpoint);
                    SceneObject.setTransform(player, new TransformF(spawnlocation));
                    }
                else
                    {
                    // If we weren't able to create the player object then warn the user
                    // When the player clicks OK in one of these message boxes, we will fall through
                    // to the "if (!isObject(%player))" check below.
                    if (console.GetVarString(spawndatablock).Trim() != "")
                        {
                        console.Call("MessageBoxOK",
                                     new string[]
                                         {
                                             "Spawn Player Failed",
                                             "Unable to create a player with class " + spawnclass + " and datablock "+ spawndatablock + ".\n\nStarting as an Observer instead.", ""
                                         });
                        }
                    else
                        {
                        console.Call("MessageBoxOK",
                                     new string[]
                                         {
                                             "Spawn Player Failed", "Unable to create a player with class " + spawnclass + ".\n\nStarting as an Observer instead.",
                                             ""
                                         });
                        }
                    }
                }
            else
                {
                // Create a default player
                player = console.SpawnObject(Game__DefaultPlayerClass, Game__DefaultPlayerDataBlock, "", "", "").AsString();

                if (SimObject.SimObject_isMemberOfClass(player, "Player"))
                    console.warn("Trying to spawn a class that does not derive from player!!!!!");
                // Treat %spawnPoint as a transform
                try
                {
                    SceneObject.setTransform(player, new TransformF(spawnpoint));
                }
                catch (NullReferenceException e)
                {
                    console.print("No spawnsphere");
                }
                }

            // Update the default camera to start with the player
            if (!console.isObject(player))
                {
                console.SetVar(client + ".spawnCamera", spawnpoint);
                return false;
                }
            SimSet.pushToBack("MissionCleanup", player);
            // Update the default camera to start with the player
            //go to where the console log is

            // Store the client object on the player object for
            // future reference
            console.SetVar(string.Format("{0}.client", player), client);
            // If the player's client has some owned turrets, make sure we let them
            // know that we're a friend too.
            if (console.GetVarString(string.Format("{0}.ownedTurrets", client)) == "1")
                {
                for (uint i = 0; i < SimSet.getCount(client + ".ownedTurrets"); i++)
                    {
                    string turret = SimSet.getObject(client + ".ownedTurrets", i).AsString();
                    console.Call(turret, "addToIgnoreList", new[] { player });
                    }
                }
            ShapeBase.setShapeName(player, console.GetVarString(string.Format("{0}.playerName", client)));


            ShapeBase.setEnergyLevel(player, console.GetVarFloat(console.getDatablock(player) + "maxEnergy"));


            if (console.GetVarString(string.Format("{0}.skin", client)).Trim() != "")
                {
                // Determine which character skins are not already in use
                string availableSkins = console.GetVarString(string.Format("{0}.availableSkins", console.getDatablock(player)));
                foreach (uint other in ClientGroup.Where(other => other.AsString() != client))
                    {
                    availableSkins = availableSkins.Replace(console.GetVarString(other + ".skin"), " ");

                    availableSkins = availableSkins.Replace("  ", " ");
                    }
                List<string> availskin = availableSkins.Split('\t').ToList();
                if (availskin.Count > 0)
                    {
                    int r = new Random().Next(0, availskin.Count - 1);
                    console.SetVar(client + ".skin", availskin[r]);
                    }
                }
            ShapeBase.setSkinName("player", console.GetVarString(string.Format("{0}.skin", client)));

            console.SetVar(string.Format("{0}.player", client), player);
            string control;
            if (console.GetVarString("$startWorldEditor") == "1")
                {
                control = console.GetVarString(string.Format("{0}.camera", client));
                //con.Eval("EditorGui.syncCameraGui();", false);
                console.Call("EditorGui", "syncCameraGui");
                }
            else
                control = player;

            //            if (nocontrol != "1")
            if (!nocontrol)
                {
                GameConnection.setControlObject(client, control);
                }


            console.error(DateTime.Now + " --- PLAYER JOIN::Name '" + Util.StripMLControlChars(ShapeBase.getShapeName(player)) + "'::ID '" + player + "'");
            return true;
            }


        [Torque_Decorations.TorqueCallBack("", "", "pickPointInSpawnSphere", "(%objectToSpawn, %spawnSphere)", 2, 51, false)]
        public string PointInSpawnSphere(string objectToSpawn, string spawnSphere)
            {
            bool spawnLocationFound = false;
            int attemptsToSpawn = 0;

            var spherLocationP3F = new TransformF();


            while (!spawnLocationFound && attemptsToSpawn < 5)
                {
                string sphereLocation = SceneObject.getTransform(spawnSphere).AsString(); // con.getTransform(spawnSphere);
                spherLocationP3F = new TransformF(sphereLocation);
                var r = new Random();

                var angleY = (float)tMath.mDegToRad((r.NextDouble() * 100) * tMath.M_2PI_F);
                var angleXZ = (float)tMath.mDegToRad((r.NextDouble() * 100) * tMath.M_2PI_F);

                int radius = console.GetVarInt(string.Format("{0}.radius", spawnSphere));
                spherLocationP3F.MPosition.x += (float)(Math.Cos(angleY) * Math.Sin(angleXZ) * (r.Next(radius * -1, radius)));
                spherLocationP3F.MPosition.y += (float)(Math.Cos(angleXZ) * (r.Next(radius * -1, radius)));
                spawnLocationFound = true;

                // Now have to check that another object doesn't already exist at this spot.
                // Use the bounding box of the object to check if where we are about to spawn in is
                // clear.

                TransformF boundingboxsize = new TransformF(console.GetVarString(string.Format("{0}.boundingBox", (console.getDatablock(objectToSpawn)))));
                float searchRadius = boundingboxsize.MPosition.x;
                float boxSizeY = boundingboxsize.MPosition.y;
                if (boxSizeY > searchRadius)
                    {
                    searchRadius = boxSizeY;
                    }
                List<UInt32> objectsfound = console.ContainerRadiusSearch(spherLocationP3F.MPosition, searchRadius, (UInt32)SceneObjectTypesAsUint.PlayerObjectType, false);
                if (objectsfound.Count > 0)
                    spawnLocationFound = false;

                attemptsToSpawn++;
                }
            if (!spawnLocationFound)
                {
                spherLocationP3F = SceneObject.getTransform(spawnSphere); // new TransformF(con.getTransform(spawnSphere));
                console.warn("WARNING: Could not spawn player after 5 times");
                }
            return spherLocationP3F.ToString();
            }


        }
    }
