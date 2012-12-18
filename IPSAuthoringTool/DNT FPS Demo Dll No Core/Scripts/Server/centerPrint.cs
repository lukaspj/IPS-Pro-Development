using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
using WinterLeaf;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "centerPrintAll", "(%message, %time, %lines)", 3, 20000, false)]
        public void CenterPrintAll(string message, string time, string lines)
            {
            if (lines == "" || lines.AsInt() > 3 || lines.AsInt() < 1)
                lines = "1";

            foreach (uint client in ClientGroup.Where(client => !GameConnection.isAIControlled(client.AsString())))
                console.commandToClient(client.AsString(), "centerPrint", new[] { message, time, lines });
            }

        [Torque_Decorations.TorqueCallBack("", "", "bottomPrintAll", "(%message, %time, %lines)", 3, 20000, false)]
        public void BottomPrintAll(string message, string time, string lines)
            {
            if (lines == "" || lines.AsInt() > 3 || lines.AsInt() < 1)
                lines = "1";
            foreach (uint client in ClientGroup.Where(client => !GameConnection.isAIControlled(client.AsString())))
                console.commandToClient(client.AsString(), "bottomPrint", new[] { message, time, lines });
            }

        [Torque_Decorations.TorqueCallBack("", "", "centerPrint", "(%client,%message, %time, %lines)", 4, 20000, false)]
        public void CenterPrint(string client, string message, string time, string lines)
            {
            if (lines == "" || lines.AsInt() > 3 || lines.AsInt() < 1)
                lines = "1";

            console.commandToClient(client, "centerPrint", new[] { message, time, lines });
            }

        [Torque_Decorations.TorqueCallBack("", "", "bottomPrint", "(%client,%message, %time, %lines)", 4, 20000, false)]
        public void BottomPrint(string client, string message, string time, string lines)
            {
            if (lines == "" || lines.AsInt() > 3 || lines.AsInt() < 1)
                lines = "1";

            console.commandToClient(client, "bottomPrint", new[] { message, time, lines });
            }

        [Torque_Decorations.TorqueCallBack("", "", "clearCenterPrint", "(%client)", 1, 20000, false)]
        public void ClearCenterPrint(string client)
            {
            console.commandToClient(client, "ClearCenterPrint");
            }

        [Torque_Decorations.TorqueCallBack("", "", "clearBottomPrint", "(%client)", 1, 20000, false)]
        public void ClearBottomPrint(string client)
            {
            console.commandToClient(client, "clearBottomPrint");
            }

        [Torque_Decorations.TorqueCallBack("", "", "clearCenterPrint", "()", 0, 20000, false)]
        public void ClearCenterPrintAll()
            {
            foreach (uint client in ClientGroup.Where(client => !GameConnection.isAIControlled(client.AsString())))
                console.commandToClient(client.AsString(), "ClearCenterPrint");
            }

        [Torque_Decorations.TorqueCallBack("", "", "clearBottomPrint", "()", 0, 20000, false)]
        public void ClearBottomPrintAll()
            {
            foreach (uint client in ClientGroup.Where(client => !GameConnection.isAIControlled(client.AsString())))
                console.commandToClient(client.AsString(), "clearBottomPrint");
            }
        }
    }
