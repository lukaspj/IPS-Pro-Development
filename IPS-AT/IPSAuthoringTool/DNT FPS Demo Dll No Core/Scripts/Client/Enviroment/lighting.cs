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
        [Torque_Decorations.TorqueCallBack("", "", "initLightingSystems", "", 0, 55000, false)]
        public void initLightingSystems()
            {


            /* client/lighting/advanced/shaders.cs          60000
             * client/lighting/advanced/lightViz.cs         61000
             * client/lighting/advanced/shadowViz.cs        62000
             * client/lighting/advanced/shadowViz.gui       63000
             * client/lighting/advanced/init.cs             64000
             * client/lighting/basic/init.cs                65000
             * client/lighting/basic/shadowFilter           66000
             * client/lighting/shadowMaps/init              67000
             */
            dnt.DoScriptInjection(ScriptType.Client, 60000, 67999);




            // Try the perfered one first.
            bool success = Util.setLightManager(console.GetVarString("$pref::lightManager"));
            if (!success)
                {
                // The perfered one fell thru... so go thru the default
                // light managers until we find one that works.
                int lmCount = Util.getFieldCount(console.GetVarString("$lightManager::defaults"));
                for (int i = 0; i < lmCount; i++)
                    {
                    string lmName = Util.getField(console.GetVarString("$lightManager::defaults"), i);
                    success = Util.setLightManager(lmName);
                    if (success)
                        break;
                    }
                }
            // Did we completely fail to initialize a light manager?
            if (!success)
                {
                // If we completely failed to initialize a light 
                // manager then the 3d scene cannot be rendered.
                Util.quitWithErrorMessage("Failed to set a light manager!");
                }
            console.print("\n");
            }
        [Torque_Decorations.TorqueCallBack("", "", "onLightManagerActivate", "%lmName", 1, 55100, false)]
        public void onLightManagerActivate(string lmName)
            {
            console.SetVar("$pref::lightManager", lmName);
            console.print("Using " + lmName);

            string activateNewFn = "onActivate" + lmName.Split(' ')[0] + "LM";
            if (console.isFunction(activateNewFn))
                console.Call(activateNewFn);
            }
        [Torque_Decorations.TorqueCallBack("", "", "onLightManagerDeactivate", "%lmName", 1, 55200, false)]
        public void onLightManagerDeactivate(string lmName)
            {
            string deactivateOldfn = "onDeactivate" + lmName.Split(' ')[0] + "LM";
            if (console.isFunction(deactivateOldfn))
                console.Call(deactivateOldfn);
            }

        }
    }
