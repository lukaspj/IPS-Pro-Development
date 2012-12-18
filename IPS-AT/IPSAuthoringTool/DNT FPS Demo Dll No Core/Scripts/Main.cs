﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
namespace DNT_FPS_Demo_Game_Dll.Scripts
    {
    public partial class Main : TorqueScriptTemplate
        {
        private dnTorque dnt;
        public Main(ref dnTorque c)
            : base(ref c)
            {
            dnt = c;
            }

        [Torque_Decorations.TorqueCallBack("", "", "init_Main", "", 0, 1000, true)]
        public void init_Main()
            {
            // Constants for referencing video resolution preferences
            console.SetVar("$WORD::RES_X", "0");
            console.SetVar("$WORD::RES_Y", 1);
            console.SetVar("$WORD::FULLSCREEN", 2);
            console.SetVar("$WORD::BITDEPTH", 3);
            console.SetVar("$WORD::REFRESH", 4);
            console.SetVar("$WORD::AA", 5);
            }


        [Torque_Decorations.TorqueCallBack("", "", "onStart", "", 0, 1200, false)]
        public void onStart()
            {
            console.error(Util.getPackageList());
            console.Call("dnEval", new string[] { "client.Load_Defaults()" });
            console.Call("dnEval", new string[] { "server.Load_Defaults()" });

            // Here is where we will do the video device stuff, so it overwrites the defaults
            // First set the PCI device variables (yes AGP/PCI-E works too)
            console.SetVar("$isFirstPersonVar", 1);

            // Uncomment to enable AdvancedLighting on the Mac (T3D 2009 Beta 3)
            //$pref::machax::enableAdvancedLighting = true;

            // Uncomment to disable ShaderGen, useful when debugging
            //$ShaderGen::GenNewShaders = false;

            // Uncomment to dump disassembly for any shader that is compiled to disk.
            // These will appear as shadername_dis.txt in the same path as the
            // hlsl or glsl shader.   
            //$gfx::disassembleAllShaders = true;

            // Uncomment useNVPerfHud to allow you to start up correctly
            // when you drop your executable onto NVPerfHud
            //$Video::useNVPerfHud = true;

            // Uncomment these to allow you to force your app into using
            // a specific pixel shader version (0 is for fixed function)
            //$pref::Video::forcePixVersion = true;
            //$pref::Video::forcedPixVersion = 0;

            console.SetVar("$pref::Video::displayDevice", "D3D9");

            #region exec("./scripts/client/core.cs");
            dnt.DoScriptInjection(ScriptType.Client, 20000, 20999);
            console.Call("initializeCore");
            #endregion

            #region exec("./scripts/client/client.cs");
            dnt.DoScriptInjection(ScriptType.Client, 70000, 70999);
            #endregion

            #region exec("./scripts/server/server.cs");
            dnt.DoScriptInjection(ScriptType.Server, -10000, -10000);
            #endregion

            Util.exec(@"core/scripts/gui/guiTreeViewCtrl.cs", false, false);
            Util.exec("core/scripts/gui/messageBoxes/messageBox.ed.cs", false, false);

            Util.exec("art/gui/customProfiles.cs", false, false);

            // Level Chooser GUI
            Util.exec("core/art/gui/chooseLevelDlg.gui", false, false);
            Util.exec("core/scripts/gui/chooseLevelDlg.cs", false, false);

            Util.exec("core/scripts/gui/optionsDlg.cs", false, false);
            Util.exec("core/art/gui/optionsDlg.gui", false, false);
            Util.exec("core/scripts/gui/loadingGui.cs", false, false);
            Util.exec("core/art/gui/loadingGui.gui", false, false);

            console.print(" % - Initialized Core");


            console.print("\n--------- Initializing Directory: scripts ---------");
            console.Call("dnEval", new string[] { "client.Load_ClientInit()" });
            console.Call("dnEval", new string[] { "server.Load_ServerInit()" });
            console.Call("physicsInit");
            console.Call("sfxStartup");
            console.Call("initServer");
            console.Call(console.GetVarBool("$Server::Dedicated") ? "initDedicated" : "initClient");
            }

        [Torque_Decorations.TorqueCallBack("", "", "onExit", "", 0, 1300, false)]
        public void onExit()
            {
            console.Call(console.GetVarBool("$Server::Dedicated") ? "destroyServer" : "disconnect");

            console.Call("physicsDestroy");
            console.print("Exporting client prefs");
            Util.export("$pref::*", "./scripts/client/prefs.cs", false);

            console.print("Exporting server prefs");
            Util.export("$pref::Server::*", "./scripts/server/prefs.cs", false);

            console.Call_Classname("BanList", "Export", new string[] { "./server/banlist.cs" });

            console.Call("shutdownCore");
            }


        [Torque_Decorations.TorqueCallBack("", "", "loadKeybindings", "", 0, 1400, false)]
        public void loadKeybindings()
            {
            console.SetVar("$keybindCount", 0);
            // Load up the active projects keybinds.
            if (console.isFunction("setupKeybinds"))
                console.Call("setupKeybinds");
            }






        public void LoadMainInit()
            {
            m_ts.DoScriptInjection(ScriptType.Main, 1000, 1700);
            dnt.DoScriptInjection(ScriptType.Main, 0, 999);
            }

        public void Init_ParseArgs()
            {
            dnt.DoScriptInjection(ScriptType.Main, 1800, 1830); //Calls ParseArgs

            }




        }
    }
