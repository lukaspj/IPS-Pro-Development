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
        /// Shortcut for typi1ng dbgSetParameters with the default values torsion uses.
        [Torque_Decorations.TorqueCallBack("", "recordingsDlg", "onWake", "", 0, 37000, false)]
        public void RecordingsDlgonWake()
            {
            GuiTextListCtrl.clear("RecordingsDlgList");
            
            int i = 0;
            string filespec = console.GetVarString("$currentMod") + "/recordings/*.rec";
            console.print(filespec);

            for (string file = Util.findFirstFile(filespec, false); file != ""; file = Util.findNextFile(filespec))
                
                GuiTextListCtrl.addRow("RecordingsDlgList", i++, Util.fileBase(file), 0);

            GuiTextListCtrl.sort("RecordingsDlgList", 0, false);
            
            GuiTextListCtrl.setSelectedRow("RecordingsDlgList", 0);
            
            GuiTextListCtrl.scrollVisible("RecordingsDlgList", 0);
            
            }
        [Torque_Decorations.TorqueCallBack("", "", "StartSelectedDemo", "", 0, 37000, false)]
        public void StartSelectedDemo()
            {
            int sel = GuiTextListCtrl.getSelectedId("RecordingsDlgList"); //console.Call("RecordingsDlgList", "getSelectedId");
            string rowText = GuiTextListCtrl.getRowTextById("RecordingsDlgList", sel);// console.Call("RecordingsDlgList", "getRowTextById", new string[] { sel });

            string file = console.GetVarString("$currentMod") + "/recordings/" + Util.getField(rowText, 0) + ".rec";
            new Torque_Class_Helper("GameConnection", "ServerConnection").Create(m_ts);



            console.Call("RootGroup", "add", new string[] { "ServerConnection" });
            // Start up important client-side stuff, such as the group
            // for particle emitters.  This doesn't get launched during a demo
            // as we short circuit the whole mission loading sequence.
            
            clientStartMission();
            if (console.Call("ServerConnection", "playDemo", new string[] { file }).AsBool())
                {
                GuiCanvas.setContent("Canvas", "PlayGui");
                GuiCanvas.popDialog("Canvas", "RecordingsDlg");
                
                console.Call("ServerConnection", "prepDemoPlayback");

                }
            else
                {
                console.Call("MessageBoxOK", new string[] { "Playback Failed", "Demo playback failed for file '" + file + "'." });
                if (console.isObject("ServerConnection"))
                    
                    GameConnection.delete("ServerConnection", "");
                }

            }
        [Torque_Decorations.TorqueCallBack("", "", "startDemoRecord", "", 0, 37000, false)]
        public void StartDemoRecord()
            {
            GameConnection.stopRecording("ServerConnection");
            
            
            if (GameConnection.isDemoPlaying("ServerConnection"))//(console.Call("ServerConnection", "isDemoPlaying").AsBool())
                return;

            string file = "";
            int i;
            for (i = 0; i < 1000; i++)
                {
                string num = i.AsString();
                if (i < 10)
                    num = "0" + num;
                if (i < 100)
                    num = "0" + num;

                file = console.GetVarString("$currentMod") + "/recordings/demo" + num + ".rec";
                if (!Util.isFile(file))
                    break;
                }
            if (i == 1000)
                return;

            console.SetVar("$DemoFileName", file);
            
            ChatHudAddLine("ChatHud", console.ColorEncode(@"\c4Recording to file [\c2" + file + @"\cr]."));

            console.Call("ServerConnection", "prepDemoRecord");

            
            GameConnection.startRecording("ServerConnection",file);

            if (GameConnection.isDemoRecording("ServerConnection")) return;//"console.Call("ServerConnection", "isDemoRecording").AsBool()) return;
            
            
            console.Call("deleteFile", new string[] { file });

            
            ChatHudAddLine("ChatHud", console.ColorEncode(@"\c3 *** Failed to record to file [\c2" + file + @"\cr]."));

            console.SetVar("DemoFileName", "");
            }
        [Torque_Decorations.TorqueCallBack("", "", "stopDemoRecord", "", 0, 37000, false)]
        public void StopDemoRecord()
            {
            if (!GameConnection.isDemoRecording("ServerConnection")) return;
            
            
            ChatHudAddLine("ChatHud", console.ColorEncode(@"\c4Recording file [\c2" + console.GetVarString("$DemoFileName") + @"\cr] finished."));
            
            GameConnection.stopRecording("ServerConnection");
            }

        [Torque_Decorations.TorqueCallBack("", "", "demoPlaybackComplete", "", 0, 37000, false)]
        public void DemoPlaybackComplete()
            {
            
            disconnect();
            
            clientEndMission();
            if (console.GetVarBool("$UseUnifiedShell"))
                {
                if (console.isObject("UnifiedMainMenuGui"))
                    GuiCanvas.setContent("Canvas", "UnifiedMainMenuGui");
                else if (console.isObject("MainMenuGui"))
                    GuiCanvas.setContent("Canvas", "MainMenuGui");
                }
            else if (console.isObject("MainMenuGui"))
                GuiCanvas.setContent("Canvas", "MainMenuGui");

            
            GuiCanvas.pushDialog("Canvas", "RecordingsDlg");
            }
        }
    }
