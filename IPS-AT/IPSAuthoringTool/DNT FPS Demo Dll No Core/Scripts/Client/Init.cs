﻿using System;
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
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // Variables used by client scripts & code.  The ones marked with (c)
        // are accessed from code.  Variables preceeded by Pref:: are client
        // preferences and stored automatically in the ~/client/prefs.cs file
        // in between sessions.
        //
        //    (c) Client::MissionFile             Mission file name
        //    ( ) Client::Password                Password for server join

        //    (?) Pref::Player::CurrentFOV
        //    (?) Pref::Player::DefaultFov
        //    ( ) Pref::Input::KeyboardTurnSpeed

        //    (c) pref::Master[n]                 List of master servers
        //    (c) pref::Net::RegionMask
        //    (c) pref::Client::ServerFavoriteCount
        //    (c) pref::Client::ServerFavorite[FavoriteCount]
        //    .. Many more prefs... need to finish this off

        // Moves, not finished with this either...
        //    (c) firstPerson
        //    $mv*Action...

        //-----------------------------------------------------------------------------
        // These are variables used to control the shell scripts and
        // can be overriden by mods:
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
         [Torque_Decorations.TorqueCallBack("", "", "initClient", "()",  0,2000, false)]
        public string InitClient()
             {
             console.print(string.Format("\n--------- Initializing {0}: Client Scripts ---------", console.GetVarString("$appName")));

             // Make sure this variable reflects the correct state
             console.SetVar("$Server::Dedicated",false);

             // Game information used to query the master server
             console.SetVar("$Client::GameTypeQuery", console.GetVarString("$appName"));
             console.SetVar("$Client::MissionTypeQuery","Any");

             // override the base profiles if necessary
             Util.exec("core/art/gui/customProfiles.cs", false, false);

             initBaseClient();



             configureCanvas();


             Util.exec("art/gui/defaultGameProfiles.cs", false, false);
             Util.exec("art/gui/PlayGui.gui", false, false);
             Util.exec("art/gui/ChatHud.gui", false, false);
             Util.exec("art/gui/playerList.gui", false, false);
             Util.exec("art/gui/hudlessGui.gui", false, false);
             Util.exec("art/gui/controlsHelpDlg.gui", false, false);

             // Load up the shell GUIs

             if (console.GetVarString("$platform") != "xenon")
                 Util.exec("art/gui/mainMenuGui.gui", false, false);
             Util.exec("art/gui/joinServerDlg.gui", false, false);
             Util.exec("art/gui/endGameGui.gui", false, false);
             Util.exec("art/gui/StartupGui.gui", false, false);


             /*
             * PlayerList           3000
             * ChatHud              4000
             * playgui.cs           4900
             * MessageHud           5000
              */

             dnt.DoScriptInjection(ScriptType.Client, 3000, 5000);

             Util.exec("scripts/gui/startupGui.cs", false, false);
             
             // Load useful Materials
             /*
             *Client               6000
             * Game                 7000
             * MissionDownload      8000
             * ServerConnection     9000
             * Shaders              10000
             * default.bind            11000
              */
             dnt.DoScriptInjection(ScriptType.Client, 6000, 11999);

             if (Util.isFile("scripts/client/config.cs"))
                 Util.exec("scripts/client/config.cs", false, false);

             loadMaterials();


             // Really shouldn't be starting the networking unless we are
             // going to connect to a remote server, or host a multi-player
             // game.
             Util._setNetPort("0", true.AsString());


             // Copy saved script prefs into C++ code.
             console.Call("setDefaultFov", new string[] {console.GetVarString("$pref::Player::defaultFov")});
             console.Call("setZoomSpeed", new string[] {console.GetVarString("$pref::Player::zoomSpeed")});

             /*
              * AudioData            12000
              */
             dnt.DoScriptInjection(ScriptType.Client, 12000, 12000);


             // Start up the main menu... this is separated out into a
             // method for easier mod override.


             if (console.GetVarBool("$startWorldEditor") && console.GetVarBool("$startGUIEditor"))
                 // Editor GUI's will start up in the primary main.cs once
                 // engine is initialized.
                 return string.Empty;

             
             LoadLoadingGui("Load....");

             if (console.GetVarString("$JoinGameAddress")!="")
                 
                 console.Call("connect", new string[] {console.GetVarString("$JoinGameAddress"), "", console.GetVarString("$Pref::Player::Name")});
             else
                 {
                 GuiCanvas.setCursor("Canvas","DefaultCursor");

                 string file = Util.findFirstFile("levels/preview.mis", false);

                 if (file != "")

                     console.Call("createAndConnectToLocalServer", new string[] { "SinglePlayer", file });
                 }
             return string.Empty;
             }
        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "loadMainMenu", "()",  0, 2000, false)]
        public string LoadMainMenu()
            {
            // Startup the client with the Main menu...
            if (console.isObject("MainMenuGui"))
                GuiCanvas.setContent("Canvas","MainMenuGui");
            else if (console.isObject("UnifiedMainMenuGui"))
                GuiCanvas.setContent("Canvas","UnifiedMainMenuGui");

            GuiCanvas.setCursor("Canvas", "DefaultCursor");

            // first check if we have a level file to load
            if (console.GetVarString("$levelToLoad")!="")
                {
                string levelFile = "levels/";

                
                string level2Load = console.GetVarString("$levelToLoad");
                string ext = level2Load.Substring(level2Load.Length - 3, 3);
                if (ext != "mis")
                    levelFile = string.Format("{0}{1}.mis", levelFile, level2Load);
                else
                    levelFile = levelFile + level2Load;

                // Clear out the $levelToLoad so we don't attempt to load the level again
                // later on.

                console.SetVar("$levelToLoad","");

                // let's make sure the file exists

                string file = Util.findFirstFile(levelFile,false);

                if (file != "")
                    
                    console.Call("createAndConnectToLocalServer", new string[] {"SinglePlayer", file});
                }
            return string.Empty;
            }
        [Torque_Decorations.TorqueCallBack("", "", "loadLoadingGui", "(%displayText)",  1, 2000, false)]
        public string LoadLoadingGui(string displayText)
            {
            GuiCanvas.setContent("Canvas","LoadingGui");
            GuiControl.setValue("LoadingProgress", "1");
            GuiControl.setValue("LoadingProgressTxt", displayText != "" ? displayText : "WAITING FOR SERVER... shouldn't take long....");
            GuiCanvas.repaint("Canvase",0);
            return string.Empty;
            }
        }
    }