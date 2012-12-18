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
        [Torque_Decorations.TorqueCallBack("", "", "clientCmdSyncEditorGui", "()", 0, 34000, false)]
        public void ClientCmdSyncEditorGui()
            {
            if (console.isObject("EditorGui"))
                console.Call("EditorGui", "syncCameraGui");
            }
        }
    }
