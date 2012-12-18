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
        /// Shortcut for typing dbgSetParameters with the default values torsion uses.
        [Torque_Decorations.TorqueCallBack("", "", "dbgTorsion", "", 0, 35000, false)]
        public void DbgTorsion()
            {
            console.Call("dbgSetParameters", new string[] { "6060", "password", "false" });
            }
        /// Reset the input state to a default of all-keys-up.
        /// A helpful remedy for when Torque misses a button up event do to your breakpoints
        /// and can't stop shooting / jumping / strafing.
        [Torque_Decorations.TorqueCallBack("", "", "mvReset", "", 0, 35000, false)]
        public void MvReset()
            {
            for (int i = 0; i < 6; i++)
                console.SetVar("mvTriggerCount[" + i.AsString() + "]", 0);

            console.SetVar("$mvUpAction", 0);
            console.SetVar("$mvDownAction", 0);
            console.SetVar("$mvLeftAction", 0);
            console.SetVar("$mvRightAction", 0);
            }
        }
    }
