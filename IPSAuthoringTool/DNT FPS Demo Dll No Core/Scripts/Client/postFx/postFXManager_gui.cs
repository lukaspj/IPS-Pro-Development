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
        [Torque_Decorations.TorqueCallBack("", "", "postFXManager_Init", "", 0, 102000, true)]
        public void postFXManager_Init()
            {
            if (!console.isObject("PostFXManager"))
                Util.exec("art/postfx/postFXManager.gui", false, false);
            }

        }
    }
