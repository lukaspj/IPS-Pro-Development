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
        [Torque_Decorations.TorqueCallBack("", "", "imposterMetricsCallback", "", 0, 45000, false)]
        public string imposterMetricsCallback()
            {
            return "  | IMPOSTER |" +
          "   Rendered: " + console.GetVarString("$ImposterStats::rendered") +
          "   Batches: " + console.GetVarString("$ImposterStats::batches") +
          "   DrawCalls: " + console.GetVarString("$ImposterStats::drawCalls") +
          "   Polys: " + console.GetVarString("$ImposterStats::polyCount") +
          "   RtChanges: " + console.GetVarString("$ImposterStats::rtChanges");
            }
        }
    }
