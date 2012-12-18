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

        [Torque_Decorations.TorqueCallBack("", "", "initializeCore", "", 0, 20000, false)]
        public void initializeCore()
            {

            // Not Reentrant
            if (console.GetVarBool("$coreInitialized"))
                return;
            // Core keybindings.
            ActionMap.bind("GlobalActionMap", "keyboard", "tilde", "toggleConsole");
            ActionMap.bind("GlobalActionMap", "keyboard", "ctrl p", "doScreenShot");
            ActionMap.bindCmd("GlobalActionMap", "keyboard", "alt enter", "Canvas.attemptFullscreenToggle();", "");
            ActionMap.bindCmd("GlobalActionMap", "keyboard", "alt k", "cls();", "");



            /*
             * core                 20000
             * audio                21000
             * canvas               22000
             * cursor               23000
             * profiles             24000
             * GuiCursors           25000
             * audioEnviroments     26000
             * audioDescriptions    27000
             * audioStates          28000
             * audioAmbiences       29000   
             */
            dnt.DoScriptInjection(ScriptType.Client, 20000, 29999);

            // Seed the random number generator.
            Util.setRandomSeed(DateTime.Now.Millisecond);

            // Set up networking.
            Util._setNetPort("0", true.AsString());
            // Initialize the canvas.
            initializeCanvas();

            // Start processing file change events.   
            Util.startFileChangeNotifications();

            Util.exec("core/art/gui/remapDlg.gui", false, false);
            Util.exec("core/art/gui/console.gui", false, false);
            Util.exec("core/art/gui/consoleVarDlg.gui", false, false);
            Util.exec("core/art/gui/netGraphGui.gui", false, false);

            /* help                 30000
             * screenshot           31000
             * scriptDoc            32000
             * helperfuncs          33000
             * commands             34000
             * devHelpers           35000
             * metrics              36000
             * recordings           37000
             * centerPrint          38000
             */

            dnt.DoScriptInjection(ScriptType.Client, 30000, 38999);

            loadCoreMaterials();

            /*
             * commonMaterialData   40000
             * shaders              41000
             * materials            42000
             * terrainBlock         43000
             * water                44000
             * imposter             45000
             * scatterSky           46000
             * clouds               47000
             * postfx               48000
             */

            dnt.DoScriptInjection(ScriptType.Client, 40000, 48999);

            initPostEffects();

            // Initialize the post effect manager.

            /* postFX/postFXManager_Gui_cs                  102000
             * postFX/postFXManager_gui_cs_cs               103000
             * postfx/postfxmanagerguid_settings            104000
             * postFX/postFXManager.persistance             105000
             */
            dnt.DoScriptInjection(ScriptType.Client, 102000, 105999);

            PostFXManagersettingsApplyDefaultPreset("PostFXManager");
            

            GuiCanvas.setCursor("Canvas", "DefaultCursor");

            console.Call("loadKeybindings");
            
            

            console.SetVar("$coreInitialized", true);

            }
        //---------------------------------------------------------------------------------------------
        // shutdownCore
        // Shuts down core game functionality.
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "shutdownCore", "", 0, 20000, false)]
        public void shutdownCore()
            {
            Util.stopFileChangeNotifications();
            sfxShutdown();
            }

        //---------------------------------------------------------------------------------------------
        // dumpKeybindings
        // Saves of all keybindings.
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "dumpKeybindings", "", 0, 20000, false)]
        public void dumpKeybindings()
            {
            for (int i = 0; i < console.GetVarInt("$keybindCount"); i++)
                {
                string bindobj = console.GetVarString("$keybindMap[%i]");
                if (!console.isObject(bindobj)) continue;
                string prefspath = console.Call("getPrefsPath", new string[] { "bind.cs" });
                console.Call(bindobj, "save", new string[] { prefspath, i == 0 ? "false" : "true" });
                console.Call(bindobj, "delete");
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "handleEscape", "", 0, 20000, false)]
        public void handleEscape()
            {
            if (console.isObject("EditorGui"))
                {
                if (GuiCanvas.getContent("Canvas") == console.Call("EditorGui", "getID").AsInt())
                    {
                    console.Call("EditorGui", "handleEscape");
                    return;
                    }
                else if (console.Call("EditorIsDirty").AsBool())
                    {
                    console.Call("MessageBoxYesNoCancel",
                             new string[] { "Level Modified", "Level has been modified in the Editor. Save?", "EditorDoExitMission(1);", "EditorDoExitMission();", "" });
                    return;
                    }
                }
            if (console.isObject("GuiEditor"))
                {
                if (console.Call("GuiEditor", "isAwake").AsBool())
                    {
                    console.Call("GuiEditCanvas", "quit");
                    return;
                    }
                }
            if (GuiControl.isAwake("PlayGui"))
                console.Call("escapeFromGame");
            }
        //-----------------------------------------------------------------------------
        // loadMaterials - load all materials.cs files
        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "loadCoreMaterials", "", 0, 20000, false)]
        public void loadCoreMaterials()
            {
            for (string file = Util.findFirstFile("core/materials.cs.dso", true); file != ""; file = Util.findNextFile("core/materials.cs.dso"))
                {
                string csFileName = file.Substring(0, file.Length - 4);
                if (!Util.isFile(csFileName))
                    Util.exec(csFileName, false, false);
                }

            // Load all source material files.

            for (string file = Util.findFirstFile("core/materials.cs", true); file != ""; file = Util.findNextFile("core/materials.cs"))
                {
                Util.exec(file, false, false);
                }

            }
        [Torque_Decorations.TorqueCallBack("", "", "reloadCoreMaterials", "", 0, 20000, false)]
        public void reloadCoreMaterials()
            {
            Util.reloadTextures();

            loadCoreMaterials();

            console.Call("reInitMaterials");
            }
        //-----------------------------------------------------------------------------
        // loadMaterials - load all materials.cs files
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "loadMaterials", "", 0, 20000, false)]
        public void loadMaterials()
            {
            for (string file = Util.findFirstFile("*/materials.cs.dso", true); file != ""; file = Util.findNextFile("*/materials.cs.dso"))
                {
                string csFileName = file.Substring(0, file.Length - 4);
                if (!Util.isFile(csFileName))
                    Util.exec(csFileName, false, false);
                }
            for (string file = Util.findFirstFile("*/materials.cs", true); file != ""; file = Util.findNextFile("*/materials.cs"))
                {
                Util.exec(file, false, false);
                }
            // Load all materials created by the material editor if
            // the folder exists

            if (Util.IsDirectory("materialEditor"))
                {
                for (string file = Util.findFirstFile("materialEditor/*.cs.dso", true); file != ""; file = Util.findNextFile("materialEditor/*.cs.dso"))
                    {
                    string csFileName = file.Substring(0, file.Length - 4);
                    if (!Util.isFile(csFileName))
                        Util.exec(csFileName, false, false);
                    }
                for (string file = Util.findFirstFile("materialEditor/*.cs", true); file != ""; file = Util.findNextFile("materialEditor/*.cs"))
                    {
                    Util.exec(file, false, false);
                    }

                }
            }


        [Torque_Decorations.TorqueCallBack("", "", "reloadMaterials", "", 0, 20000, false)]
        public void reloadMaterials()
            {
            Util.reloadTextures();
            
            loadCoreMaterials();
            console.Call("reInitMaterials");
            }
        }
    }
