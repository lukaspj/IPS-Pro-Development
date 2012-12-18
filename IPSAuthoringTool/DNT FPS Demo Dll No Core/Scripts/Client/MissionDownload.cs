﻿using System.Collections.Generic;
using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    //-----------------------------------------------------------------------------
    // Torque
    // Copyright GarageGames, LLC 2011
    //-----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    // Game start / end events sent from the server
    //----------------------------------------------------------------------------
    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //----------------------------------------------------------------------------
        // Mission Loading & Mission Info
        // The mission loading server handshaking is handled by the
        // core/scripts/client/missingLoading.cs.  This portion handles the interface
        // with the game GUI.
        //----------------------------------------------------------------------------

        //----------------------------------------------------------------------------
        // Loading Phases:
        // Phase 1: Download Datablocks
        // Phase 2: Download Ghost Objects
        // Phase 3: Scene Lighting

        //----------------------------------------------------------------------------
        // Phase 1
        //----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "MissionDownload_init", "", 0, 8000, true)]
        public string MissionDownload_init()
            {

            console.Eval("addMessageCallback( 'MsgLoadInfo', handleLoadInfoMessage );");
            console.Eval("addMessageCallback( 'MsgLoadDescripition', handleLoadDescriptionMessage );");
            console.Eval("addMessageCallback( 'MsgLoadInfoDone', handleLoadInfoDoneMessage );");
            console.Eval("addMessageCallback( 'MsgLoadFailed', handleLoadFailedMessage );");
            return string.Empty;
            }


        [Torque_Decorations.TorqueCallBack("", "", "onMissionDownloadPhase1", "(%missionName, %musicTrack)", 2, 8000, false)]
        public string onMissionDownloadPhase1(string missionName, string musicTrack)
            {
            console.print("------------>onMissionDownloadPhase1('" + missionName + "', '" + musicTrack + "')");
            // Load the post effect presets for this mission.
            string path = "levels/" + Util.fileBase(missionName) + console.GetVarString("$PostFXManager::fileExtension");

            if (console.Call("isScriptFile", new string[] { path }).AsBool())
                //console.Call_Classname("postFXManager", "loadPresetHandler", new string[] { path });
                PostFXManagerloadPresetHandler(path);
            else
                //    console.Call_Classname("PostFXManager", "settingsApplyDefaultPreset");
                PostFXManagersettingsApplyDefaultPreset("PostFXManager");

            // Close and clear the message hud (in case it's open)
            if (console.isObject("MessageHud"))
                console.Call("MessageHud", "close");

            // Reset the loading progress controls:

            if (!console.isObject("LoadingProgress"))
                return string.Empty;

            GuiControl.setValue("LoadingProgress", "0");



            GuiControl.setValue("LoadingProgressTxt", "LOADING DATABLOCKS");

            GuiCanvas.repaint("Canvas", 0);
            return string.Empty;
            }
        [Torque_Decorations.TorqueCallBack("", "", "onPhase1Progress", "(%progress)", 1, 8000, false)]
        public string onPhase1Progress(string progress)
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;

            GuiControl.setValue("LoadingProgress", progress);
            GuiCanvas.repaint("Canvas", 33);
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "onPhase1Complete", "()", 0, 8000, false)]
        public string onPhase1Complete()
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;

            GuiControl.setValue("LoadingProgress", "1");
            GuiCanvas.repaint("Canvas", 0);
            return string.Empty;
            }


        //----------------------------------------------------------------------------
        // Phase 2
        //----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "onMissionDownloadPhase2", "()", 0, 8000, false)]
        public string onMissionDownloadPhase2()
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;

            GuiControl.setValue("LoadingProgress", "0");
            GuiControl.setValue("LoadingProgressTxt", "LOADING OBJECTS");
            GuiCanvas.repaint("Canvas", 0);
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "onPhase2Progress", "(%progress)", 1, 8000, false)]
        public string onPhase2Progress(string progress)
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;
            GuiControl.setValue("LoadingProgress", progress);
            GuiCanvas.repaint("Canvas", 33);
            return string.Empty;
            }
        [Torque_Decorations.TorqueCallBack("", "", "onPhase2Complete", "()", 0, 8000, false)]
        public string onPhase2Complete()
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;
            GuiControl.setValue("LoadingProgress", "1");
            GuiCanvas.repaint("Canvas", 0);
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "onFileChunkReceived", "(%fileName, %ofs, %size)", 3, 8000, false)]
        public string onFileChunkReceived(string filename, string ofs, string size)
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;

            GuiControl.setValue("LoadingProgress", (ofs.AsDouble() / size.AsDouble()).ToString("0.0"));
            GuiControl.setValue("LoadingProgressTxt", "Downloading " + filename + ".... ");
            return string.Empty;
            }

        //----------------------------------------------------------------------------
        // Phase 3
        //----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "onMissionDownloadPhase3", "()", 0, 8000, false)]
        public string onMissionDownloadPhase3()
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;
            GuiControl.setValue("LoadingProgress", "0");
            GuiControl.setValue("LoadingProgressTxt", "LIGHTING MISSION");
            GuiCanvas.repaint("Canvas", 0);
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "onPhase3Progress", "(%progress)", 1, 8000, false)]
        public string onPhase3Progress(string progress)
            {
            if (!console.isObject("LoadingProgress"))
                return string.Empty;

            GuiControl.setValue("LoadingProgress", progress);
            GuiCanvas.repaint("Canvas", 33);
            return string.Empty;
            }
        [Torque_Decorations.TorqueCallBack("", "", "onPhase3Complete", "()", 0, 8000, false)]
        public string onPhase3Complete()
            {

            console.SetVar("$lightingMission", false);

            if (!console.isObject("LoadingProgress"))
                return string.Empty;
            GuiControl.setValue("LoadingProgressTxt", "STARTING MISSION");
            GuiControl.setValue("LoadingProgress", "1");
            GuiCanvas.repaint("Canvas", 0);
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "onMissionDownloadComplete", "()", 0, 8000, false)]
        public string onMissionDownloadComplete()
            {
            init_Default_Bind();
            // Client will shortly be dropped into the game, so this is
            // good place for any last minute gui cleanup.
            return string.Empty;
            }

        public List<string> MLoadinfo = new List<string>();
        [Torque_Decorations.TorqueCallBack("", "", "handleLoadInfoMessage", "( %msgType, %msgString, %mapName ) ", 3, 8000, false)]
        public string handleLoadInfoMessage(string msgType, string msgString, string mapname)
            {
            console.error("Load info Recieved map " + mapname);
            if (GuiCanvas.getContent("Canvas") != console.Call("LoadingGui", "getId").AsInt())
                LoadLoadingGui("LOADING MISSION FILE " + mapname);
                

            if (mapname.Trim() != "")
                MLoadinfo.Clear();
            MLoadinfo.Add("LOADING MISSION FILE " + mapname);
            for (int line = 0; line < console.GetVarInt("LoadingGui.qLineCount"); line++)
                {
                console.SetVar("LoadingGui.qLine[" + line.AsString() + "]", "");
                console.SetVar("LoadingGui.qLineCount", line + 1);
                }
            return string.Empty;
            }



        [Torque_Decorations.TorqueCallBack("", "", "handleLoadDescriptionMessage", "( %msgType, %msgString, %line ) ", 3, 8000, false)]
        public void handleLoadDescriptionMessage(string msgType, string msgString, string line)
            {
            console.error("Line recieved " + line);
            MLoadinfo.Add(line);
            int l = console.GetVarInt("LoadingGui.qLineCount");
            console.SetVar("LoadingGui.qLine[" + l.AsString() + "]", line);
            l++;
            console.SetVar("LoadingGui.qLineCount", l);
            }

        [Torque_Decorations.TorqueCallBack("", "", "handleLoadInfoDoneMessage", "( %msgType, %msgString ) ", 2, 8000, false)]
        public string handleLoadInfoDoneMessage(string msgType, string msgString)
            {
            // This will get called after the last description line is sent.
            //onServerMessage
            foreach (string line in MLoadinfo)
                {
                OnServerMessage(line);
                
                }
            return string.Empty;
            }
        [Torque_Decorations.TorqueCallBack("", "", "handleLoadFailedMessage", "( %msgType, %msgString ) ", 2, 8000, false)]
        public string handleLoadFailedMessage(string msgType, string msgString)
            {
            console.Call("MessageBoxOk", new string[] { "Mission Load Failed", msgString + "\r\nPress OK to return to the Main Menu", "disconnect();" });
            return string.Empty;
            }





        //-------------------------------------

        //----------------------------------------------------------------------------
        // Mission Loading
        // Server download handshaking.  This produces a number of onPhaseX
        // calls so the game scripts can update the game's GUI.
        //
        // Loading Phases:
        // Phase 1: Download Datablocks
        // Phase 2: Download Ghost Objects
        // Phase 3: Scene Lighting
        //----------------------------------------------------------------------------

        //----------------------------------------------------------------------------
        // Phase 1 
        //----------------------------------------------------------------------------


        [Torque_Decorations.TorqueCallBack("", "", "clientCmdMissionStartPhase1", "(%seq, %missionName, %musicTrack)", 3, 52000, false)]
        public void clientCmdMissionStartPhase1(string seq, string missionName, string musicTrack)
            {
            console.print("*** New Mission: " + missionName);
            console.print("*** Phase 1: Download Datablocks & Targets");
            
            onMissionDownloadPhase1(missionName, musicTrack);
            console.print("------------>MissionStartPhase1Ack");
            console.commandToServer("MissionStartPhase1Ack", new string[] { seq });
            }
        [Torque_Decorations.TorqueCallBack("", "", "onDataBlockObjectReceived", "(%index, %total)", 2, 52000, false)]
        public void onDataBlockObjectReceived(string index, string total)
            {
            onPhase1Progress((index.AsDouble()/total.AsDouble()).AsString());
            
            }

        //----------------------------------------------------------------------------
        // Phase 2
        //----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "clientCmdMissionStartPhase2", "(%seq,%missionName)", 2, 52000, false)]
        public void clientCmdMissionStartPhase2(string seq, string missionName)
            {
            onPhase1Complete();
            
            console.print("*** Phase 2: Download Ghost Objects");
            onMissionDownloadPhase2();

//            console.Call("onMissionDownloadPhase2", new string[] { missionName });
            console.commandToServer("MissionStartPhase2Ack", new string[] { seq, console.GetVarString("$pref::Player:PlayerDB") });
            }
        [Torque_Decorations.TorqueCallBack("", "", "onGhostAlwaysStarted", "(%ghostCount)", 1, 52000, false)]
        public void onGhostAlwaysStarted(string ghostCount)
            {
            console.SetVar("$ghostCount", ghostCount);
            console.SetVar("$ghostsRecvd", 0);
            }

        [Torque_Decorations.TorqueCallBack("", "", "onGhostAlwaysObjectReceived", "()", 0, 52000, false)]
        public void onGhostAlwaysObjectReceived()
            {
            console.SetVar("$ghostsRecvd", console.GetVarInt("$ghostsRecvd") + 1);
            onPhase2Progress((console.GetVarFloat("$ghostsRecvd")/console.GetVarFloat("$ghostCount")).AsString());
            
            }

        //----------------------------------------------------------------------------
        // Phase 3
        //----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "clientCmdMissionStartPhase3", "(%seq,%missionName)", 2, 52000, false)]
        public void clientCmdMissionStartPhase3(string seq, string missionName)
            {
            onPhase2Complete();
            
            Util.StartClientReplication();
            Util.StartFoliageReplication();

            // Load the static mission decals.

            console.Call("decalManagerLoad", new string[] { missionName + ".decals" });

            console.print("*** Phase 3: Mission Lighting");

            console.SetVar("$MSeq", seq);
            console.SetVar("$Client::MissionFile", missionName);
            // Need to light the mission before we are ready.
            // The sceneLightingComplete function will complete the handshake 
            // once the scene lighting is done.
            if (!Util.lightScene("sceneLightingComplete", "")) return;
            console.print("Lighting mission....");
            Util._schedule("1", "0", "updateLightingProgress");

            onMissionDownloadPhase3();

            
            console.SetVar("$lightingMission", true);
            }

        [Torque_Decorations.TorqueCallBack("", "", "updateLightingProgress", "()", 0, 52000, false)]
        public void updateLightingProgress()
            {
            onPhase3Progress(console.GetVarString("$SceneLighting::lightingProgress"));
            
            if (console.GetVarBool("$lightingMission"))
                console.SetVar("$lightingProgressThread", Util._schedule("1", "0", "updateLightingProgress"));
            }
        [Torque_Decorations.TorqueCallBack("", "", "sceneLightingComplete", "()", 0, 52000, false)]
        public void sceneLightingComplete()
            {
            console.print("Mission lighting done");
            onPhase3Complete();
            
            // The is also the end of the mission load cycle.
            onMissionDownloadComplete();
            
            console.commandToServer("MissionStartPhase3Ack", new string[] { console.GetVarString("$MSeq") });

            }

        //----------------------------------------------------------------------------
        // Helper functions
        //----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "connect", "(%server)", 1, 52000, false)]
        public void connect(string server)
            {
            console.SetVar("$pref::Net::PacketRateToServer", 32);
            console.SetVar("$pref::Net::PacketSize", 200);
            console.SetVar("$platform::timeManagerProcessInterval", 1);

            
            string conn = new Torque_Class_Helper("GameConnection", "ServerConnection").Create(m_ts).AsString();
            SimSet.pushToBack("RootGroup", "ServerConnection");
            //SimSet.add("RootGroup", "ServerConnection");
            GameConnection.setConnectArgs(conn, console.GetVarString("$pref::Player::Name"));
            GameConnection.setJoinPassword(conn, console.GetVarString("$Client::Password"));
            NetConnection.connect(conn, server);
            }

        }
    }
