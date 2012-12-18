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
        [Torque_Decorations.TorqueCallBack("", "", "onActivateAdvancedLM", "", 0, 67000, false)]
        public void onActivateAdvancedLM()
            {
            if (console.GetVarString("$platform") == "macos")
                return;
            if (console.GetVarString("$platform") == "xenon")
                return;

            console.Call("AL_FormatToken", "enable");
            }
        [Torque_Decorations.TorqueCallBack("", "", "onDeactivateAdvancedLM", "", 0, 67100, false)]
        public void onDeactivateAdvancedLM()
            {
            console.Call("AL_FormatToken", "disable");
            }

        [Torque_Decorations.TorqueCallBack("", "", "setAdvancedLighting", "", 0, 67200, false)]
        public void setAdvancedLighting()
            {
            Util.setLightManager("Advanced Lighting");
            }
        }
    }
