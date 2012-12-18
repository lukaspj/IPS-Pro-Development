using System;
using System.Collections.Generic;
using WinterLeaf.Classes;
using WinterLeaf.Enums;
using WinterLeaf.Containers;
using WinterLeaf;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        private bool mDisplayOnMaster = false;
        [Torque_Decorations.TorqueCallBack("", "", "initBaseServer", "()", 0, -10000, false)]
        public void InitBaseServer()
            {
            dnt.DoScriptInjection(ScriptType.Server, 10000, 20000);
            }
        [Torque_Decorations.TorqueCallBack("", "", "portInit", "(%port)", 1, -10000, false)]
        public void PortInit(string port)
            {
            // Attempt to find an open port to initialize the server with
            int failedCount = 0;
            while (failedCount < 10 && !Util._setNetPort(port))
                {
                console.error("Port init failed on port " + port + " trying next port.");
                port = (port.AsInt() + 1).AsString();
                failedCount++;
                }
            }
        [Torque_Decorations.TorqueCallBack("", "", "createAndConnectToLocalServer", "(%serverType, %level", 2, -10000, false)]
        public bool CreateAndConnectToLocalServer(string serverType, string level)
            {
            if (!CreateServer(serverType, level))
            
                return false;

            Torque_Class_Helper tch = new Torque_Class_Helper("GameConnection", "ServerConnection");
            string conn = tch.Create(m_ts).AsString();

            GameConnection.setConnectArgs(conn, console.GetVarString("$pref::Player::Name"));

            GameConnection.setJoinPassword(conn, console.GetVarString("$Client::Password"));
            
            string result = NetConnection.connectLocal(conn);

            if (result != "")
                {
                console.Call(conn, "delete");
                DestroyServer();
                return false;
                }
            return true;
            }
        /// Create a server with either a "SinglePlayer" or "MultiPlayer" type
        /// Specify the level to load on the server
        [Torque_Decorations.TorqueCallBack("", "", "createServer", "(%serverType, %level)", 2, -10000, false)]
        public bool CreateServer(string serverType, string level)
            {
            // Increase the server session number.  This is used to make sure we're
            // working with the server session we think we are.
            console.SetVar("$Server::Session", console.GetVarInt("$Server::Session") + 1);
            if (level == "")
                {
                console.error("createServer(): level name unspecified");
                return false;
                }
            // Make sure our level name is relative so that it can send
            // across the network correctly
            level = Util.makeRelativePath(level, Util.getWorkingDirectory());
            DestroyServer();
            

            console.SetVar("$missionSequence", 0);
            //console.SetVar("$Server::PlayerCount", 0);
            Server__PlayerCount = 0;
            console.SetVar("$Server::ServerType", serverType);
            
            Server__LoadFailMsg = "";
            console.SetVar("$Physics::isSinglePlayer", true);

            // Setup for multi-player, the network must have been
            // initialized before now.

            if (serverType == "MultiPlayer")
                {
                console.SetVar("$Physics::isSinglePlayer", false);
                console.print("Starting multiplayer mode");
                // Make sure the network port is set to the correct pref.
                PortInit(console.GetVarString("$Pref::Server::Port"));
                
                console.Call("allowConnections", new string[] { "true" });

                if (mDisplayOnMaster)
                    Util._schedule("0", "0", "startHeartbeat");
                }
            // Create the ServerGroup that will persist for the lifetime of the server.
            new Torque_Class_Helper("SimGroup", "ServerGroup").Create(m_ts);

            Util.exec(@"core/art/datablocks/datablockExec.cs", false, false);
            // Let the game initialize some things now that the
            // the server has been created
            
            
            onServerCreated();

            
            LoadMission(level, true);
            return true;
            }
        [Torque_Decorations.TorqueCallBack("", "", "destroyServer", "", 0, -10000, false)]
        public void DestroyServer()
            {
            console.SetVar("$Server::ServerType", "");
            console.Call("allowConnections", new string[] { "false" });
            console.Call("stopHeartBeat");
            console.SetVar("$missionRunning",false);
            missionRunning = false;

            // End any running levels
            EndMission();
            
            
            onServerDestroyed();

            // Delete all the server objects
            if (console.isObject("ServerGroup"))
                console.Call("ServerGroup", "delete");

            // Delete all the connections:
            foreach (uint client in ClientGroup)
                {
                console.Call(client.AsString(), "delete");
                }
            console.SetVar("$Server::GuidList", "");
            // Delete all the data blocks...
            Util.deleteDataBlocks();
            // Save any server settings
            console.print("Exporting server prefs...");
            Util.export(" $Pref::Server::*", @"core/prefs.cs", false);
            // Increase the server session number.  This is used to make sure we're
            // working with the server session we think we are.
            console.SetVar("$Server::Session", console.GetVarInt("$Server::Session") + 1);
            }
        [Torque_Decorations.TorqueCallBack("", "", "resetServerDefaults", "", 0, -10000, false)]
        public void ResetServerDefaults()
            {
            console.print("Resetting server defaults...");
            Util.exec(@"core/default.cs", false, false);
            Util.exec(@"core/prefs.cs", false, false);
            // Reload the current level
            
            LoadMission(console.GetVarString("$Server::MissionFile"),false);
            }
        [Torque_Decorations.TorqueCallBack("", "", "addToServerGuidList", "(%guid)", 1, -10000, false)]
        public void AddToServerGuidList(string guid)
            {
            int count = Util.getFieldCount(console.GetVarString("$Server::GuidList"));
            for (int i = 0; i < count; i++)
                {
                if (Util.getField(console.GetVarString("$Server::GuidList"), i) == guid)
                    return;
                }

            console.SetVar("$Server::GuidList", console.GetVarString("$Server::GuidList") == "" ? guid : console.GetVarString("$Server::GuidList") + "\t" + guid);
            }

        [Torque_Decorations.TorqueCallBack("", "", "removeFromServerGuidList", "(%guid)", 1, -10000, false)]
        public void RemoveFromServerGuidList(string guid)
            {
            int count = Util.getFieldCount(console.GetVarString("$Server::GuidList"));
            for (int i = 0; i < count; i++)
                {
                if (Util.getField(console.GetVarString("$Server::GuidList"), i) != guid) continue;
                console.SetVar("$Server::GuidList", Util.removeField(console.GetVarString("$Server::GuidList"), i));
                return;
                }
            }
        /// When the server is queried for information, the value of this function is
        /// returned as the status field of the query packet.  This information is
        /// accessible as the ServerInfo::State variable.
        [Torque_Decorations.TorqueCallBack("", "", "onServerInfoQuery", "()", 0, -10000, false)]
        public string OnServerInfoQuery()
            {
            return "Doing Ok";
            }
        }
    }
