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
        [Torque_Decorations.TorqueCallBack("", "", "postFXManagerPersistancesettingscs", "", 0, 105000, true)]
        public void postFXManagerPersistancesettingscs()
            {
            // Used to name the saved files.
            console.SetVar("$PostFXManager::fileExtension", ".postfxpreset.cs");
            // The filter string for file open/save dialogs.
            console.SetVar("$PostFXManager::fileFilter", "Post Effect Presets|*.postfxpreset.cs");
            // Enable / disable PostFX when loading presets or just apply the settings?
            console.SetVar("$PostFXManager::forceEnableFromPresets", true);

            }
        //Load a preset file from the disk, and apply the settings to the
        //controls. If bApplySettings is true - the actual values in the engine
        //will be changed to reflect the settings from the file.

        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "loadPresetFile", "", 0, 105010, false)]
        public void PostFXManagerloadPresetFile()
            {
            console.Call("getLoadFilename", new string[] { console.GetVarString("$PostFXManager::fileFilter"), "PostFXManager::loadPresetHandler" });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "loadPresetHandler", "%filename", 1, 105020, false)]
        public void PostFXManagerloadPresetHandler(string filename)
            {
            //Check the validity of the file
            if (console.Call("isScriptFile", new string[] { filename }).AsBool())
                {
                filename = Util._expandFilename(filename);
                console.Call("postVerbose", new string[] { "% - PostFX Manager - Executing " + filename });
                Util.exec(filename, false, false);
                console.Call("PostFXManager", "settingsApplyFromPreset");

                }
            }

        //Save a preset file to the specified file. The extension used
        //is specified by $PostFXManager::fileExtension for on the fly
        //name changes to the extension used. 
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "savePresetFile", "%this", 1, 105030, false)]
        public void PostFXManagersavePresetFile(string thisobj)
            {
            string defaultfile = Util.filePath(console.GetVarString("$Client::MissionFile")) + "/" + Util.filePath(console.GetVarString("$Client::MissionFile"));
            console.Call("getSaveFilename", new string[] { console.GetVarString("$PostFXManager::fileFilter"), "PostFXManager::savePresetHandler", defaultfile });
            }

        //Called from the PostFXManager::savePresetFile() function
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "savePresetHandler", "%filename", 1, 105040, false)]
        public void PostFXManagersavePresetHandler(string filename)
            {
            filename = Util.makeRelativePath(filename, Util.getMainDotCsDir());
            if (Util.strstr(filename, ".") == -1)
                filename = filename + console.GetVarString("$PostFXManager::fileExtension");

            //Apply the current settings to the preset
            console.Call("PostFXManager", "settingsApplyAll");

            Util.export("$PostFXManager::Settings::*", filename, false);
            console.Call("postVerbose", new string[] { "% - PostFX Manager - Save complete. Preset saved at : " + filename });
            }

        }
    }
