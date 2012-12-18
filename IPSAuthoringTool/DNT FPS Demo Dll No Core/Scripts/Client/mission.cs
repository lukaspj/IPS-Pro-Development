using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "init_missioncs", "()", 0, 51000, true)]
        public void init_missioncs()
            {
            // Whether the local client is currently running a mission.
            console.SetVar("$Client::missionRunning", false);
            // Sequence number for currently running mission.
            console.SetVar("$Client::missionSeq", -1);
            }

        // Called when mission is started.
        [Torque_Decorations.TorqueCallBack("", "", "clientStartMission", "()", 0, 51000, false)]
        public void clientStartMission()
            {
            // The client recieves a mission start right before
            // being dropped into the game.
            Util._physicsStartSimulation("client");
            // Start game audio effects channels.

            SFXSource.play("AudioChannelEffects",-1);


            // Create client mission cleanup group.
            if(!Util._isObject("ClientMissionCleanup"))
                new Torque_Class_Helper("SimGroup", "ClientMissionCleanup").Create(m_ts);
            console.SetVar("$Client::missionRunning", true);
            Camera.setOrbitPoint(GameConnection.getCameraObject("LocalClientConnection"), new TransformF(0, 0, 0), 1, 16, 12, new Point3F(), false);
            }

        // Called when mission is ended (either through disconnect or
        // mission end client command).
        [Torque_Decorations.TorqueCallBack("", "", "clientEndMission", "()", 0, 51000, false)]
        public void clientEndMission()
            {
            // Stop physics simulation on client.
            Util._physicsStopSimulation("client");
            // Stop game audio effects channels.
            
            SFXSource.stop("AudioChannelEffects",-1);

            console.Call("decalManagerClear");
            // Delete client mission cleanup group. 

            if (console.isObject("ClientMissionCleanup"))
                console.Call("ClientMissionCleanup", "delete");

            console.Call("clearClientPaths");
            console.SetVar("$Client::missionRunning", false);

            }

        //----------------------------------------------------------------------------
        // Mission start / end events sent from the server
        //----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "clientCmdMissionStart", "(%seq)", 1, 51000, false)]
        public void clientCmdMissionStart(string seq)
            {
            clientStartMission();
            
            console.SetVar("$Client::missionSeq", seq);
            }

        [Torque_Decorations.TorqueCallBack("", "", "clientCmdMissionEnd", "(%seq)", 1, 51000, false)]
        public void clientCmdMissionEnd(string seq)
            {
            if (!console.GetVarBool("$Client::missionRunning") || console.GetVarString("$Client::missionSeq") != seq) return;
            clientEndMission();
            
            console.SetVar("$Client::missionSeq", -1);
            }
        /// Expands the name of a mission into the full 
        /// mission path and extension.

        [Torque_Decorations.TorqueCallBack("", "", "expandMissionFileName", "(%missionFile)", 1, 51000, false)]
        public string expandMissionFileName(string missionFile)
            {
            // Expand any escapes in it.
            
            missionFile = Util._expandFilename(missionFile);
            string newMission = "";
            if (!Util.isFile(missionFile))
                {
                if (!missionFile.Trim().EndsWith(".mis"))
                    newMission = missionFile.Trim() + ".mis";

                if (!Util.isFile(newMission))
                    {
                    newMission = Util._expandFilename("levels/" + newMission);
                    
                    if (!Util.isFile(newMission))
                        {
                        console.warn("The mission file '" + missionFile + "' was not found.");
                        return "";
                        }

                    }
                missionFile = newMission;
                }
            return missionFile;
            }

        /// Load a single player level on the local server.
        [Torque_Decorations.TorqueCallBack("", "", "loadLevel", "(%missionNameOrFile)", 1, 51000, false)]
        public string loadLevel(string missionNameOrFile)
            {
            // Expand the mission name... this allows you to enter
            // just the name and not the full path and extension.
            string missionFile = expandMissionFileName(missionNameOrFile);
            if (missionFile == "")
                return "false";
            // Show the loading screen immediately.
            if (console.isObject("LoadingGui"))
                {
                GuiCanvas.setContent("Canvas", "LoadingGui");
                
                console.Call("LoadingProgress", "setValue", new string[] { "1" });
                console.Call("LoadingProgressTxt", "setValue", new string[] { "LOADING MISSION FILE" });
                GuiCanvas.repaint("Canvas", 0);
                }
            // Prepare and launch the server.
            return console.Call("createAndConnectToLocalServer", new string[] { "SinglePlayer", missionFile });
            }
        }
    }
