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
        // Writes out all script functions to a file.
        [Torque_Decorations.TorqueCallBack("", "", "writeOutFunctions", "()", 0, 32000, false)]
        public void WriteOutFunctions()
            {
            console.Eval(@"new ConsoleLogger(logger, ""scriptFunctions.txt"", false);");
            Util.dumpConsoleFunctions(true, true);
            console.Call("logger", "delete");
            }
        // Writes out all script classes to a file.
        [Torque_Decorations.TorqueCallBack("", "", "writeOutClasses", "()", 0, 32000, false)]
        public void WriteOutClasses()
            {
            console.Eval(@"new ConsoleLogger(logger, ""scriptClasses.txt"", false);");
            Util.dumpConsoleClasses(true, true);
            console.Call("logger", "delete");
            }
        }
    }
