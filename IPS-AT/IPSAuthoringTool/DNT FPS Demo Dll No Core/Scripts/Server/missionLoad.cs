using System;
using System.Linq;
using WinterLeaf.Classes;



namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        public bool missionRunning = false;

        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // Server mission loading
        //-----------------------------------------------------------------------------

        // On every mission load except the first, there is a pause after
        // the initial mission info is downloaded to the client.


        public int MissionLoadPause = 5000;

        [Torque_Decorations.TorqueCallBack("", "", "initializemissionload", "()", 0, 13000, true)]
        public void Initializemissionload()
            {
            console.SetVar("$MissionLoadPause", 5000);
            }

        [Torque_Decorations.TorqueCallBack("", "", "loadMission", "( %missionName, %isFirstMission )", 2, 13000, false)]
        public void LoadMission(string missionName, bool isFirstMission)
            {
            EndMission();
            console.print("*** LOADING MISSION: " + missionName);
            console.print("*** Stage 1 load");

            // Reset all of these

            if (console.isFunction("clearCenterPrintAll"))
                ClearCenterPrintAll();
            if (console.isObject("clearBottomPrintAll"))
                ClearBottomPrintAll();
            // increment the mission sequence (used for ghost sequencing)

            console.SetVar("$missionSequence", console.GetVarInt("$missionSequence") + 1);
            //console.SetVar("$missionRunning", false);
            missionRunning = false;
            console.SetVar("$Server::MissionFile", missionName);
            Server__LoadFailMsg = "";

            // Extract mission info from the mission file,
            // including the display name and stuff to send
            // to the client.

            BuildLoadInfo(missionName);

            // Download mission info to the clients
            foreach (uint client in ClientGroup.Where(client => !GameConnection.isAIControlled(client.AsString())))
                SendLoadInfoToClient(client.AsString());

            // Now that we've sent the LevelInfo to the clients
            // clear it so that it won't conflict with the actual
            // LevelInfo loaded in the level
            ClearLoadInfo();

            // if this isn't the first mission, allow some time for the server
            // to transmit information to the clients:
            if (isFirstMission || console.GetVarString("$Server::ServerType") == "SinglePlayer")
                LoadMissionStage2();
            else
                Util._schedule(MissionLoadPause.AsString(), "0", "loadMissionStage2");
            }

        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "loadMissionStage2", "()", 0, 13000, false)]
        public void LoadMissionStage2()
            {
            console.print("*** Stage 2 load CSHARP");
            // Create the mission group off the ServerGroup
            console.SetVar("$instantGroup", "ServerGroup");

            // Make sure the mission exists
            string file = console.GetVarString("$Server::MissionFile");
            if (!Util.isFile(file))
                Server__LoadFailMsg = "Could not find mission " + file;
            else
                {
                // Calculate the mission CRC.  The CRC is used by the clients
                // to caching mission lighting.
                int missioncrc = Util.getFileCRC(file);
                console.SetVar("$missionCRC", missioncrc.AsString());
                //Looked at making this one a dll direct call, doesn't appear
                //it can be easily converted.
                // Exec the mission.  The MissionGroup (loaded components) is added to the ServerGroup
                Util.exec(console.GetVarString("$Server::MissionFile"), false, false);

                if (!console.isObject("MissionGroup"))
                    Server__LoadFailMsg = "No 'MissionGroup' found in mission " + file;

                }
            if (Server__LoadFailMsg != "")
                {
                // Inform clients that are already connected
                foreach (UInt32 client in ClientGroup)
                    MessageClient(client.AsString(), "MsgLoadFailed", Server__LoadFailMsg);
                return;
                }
            // Set mission name.
            if (console.isObject("theLevelInfo"))
                console.SetVar("$Server::MissionName", "theLevelInfo.levelName");

            // Mission cleanup group.  This is where run time components will reside.  The MissionCleanup
            // group will be added to the ServerGroup.

            var tc = new Torque_Class_Helper("SimGroup", "MissionCleanup");
            tc.Create(m_ts);
            // Make the MissionCleanup group the place where all new objects will automatically be added.
            console.SetVar("$instantGroup", "MissionCleanup");
            console.pathOnMissionLoadDone();

            // Mission loading done...
            console.warn("*** Mission loaded");

            missionRunning = true;
            //Even though we track this in the CSharp, the tools use it as a flag.
            console.SetVar("$missionRunning",true);
            // Start all the clients in the mission
            foreach (UInt32 client in ClientGroup)
                {
                GameConnectionLoadMission(client.AsString());
                }

            onMissionLoaded();
            }

        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "endMission", "()", 0, 13000, false)]
        public void EndMission()
            {
            if (console.isObject("MissionGroup"))
                return;

            console.print("*** ENDING MISSION");
            // Inform the game code we're done.

            // Inform the clients
            foreach (uint client in ClientGroup)
                {

                GameConnectionendMission(client.AsString());
                GameConnection.resetGhosting(client.AsString());
                NetConnection.clearPaths(client.AsString());
                }

            if (console.isObject("MissionGroup"))
                console.Call("MissionGroup", "delete");
            if (console.isObject("MissionCleanup"))
                console.Call("MissionCleanup", "delete");

            console.clearServerPaths();
            }

        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "resetMission", "()", 0, 13000, false)]
        public void ResetMission()
            {
            console.print("*** MISSION RESET");
            // Remove any temporary mission objects
            console.Call("MissionCleanup", "delete");
            console.SetVar("$instantGroup", console.GetVarString("ServerGroup"));
            Torque_Class_Helper tcg = new Torque_Class_Helper("SimGroup", "MissionCleanup");
            string sMissionCleanup = tcg.Create(m_ts).AsString();
            console.SetVar("$instantGroup", sMissionCleanup);

            console.clearServerPaths();
            console.error("-------------> Client Group Count = " + ClientGroup.Count);
            }


        }
    }
