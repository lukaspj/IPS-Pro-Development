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
        [Torque_Decorations.TorqueCallBack("", "", "centerprint_Init", "", 0, 38000, true)]
        public void centerprint_Init()
            {
            console.SetVar("$centerPrintActive", 0);
            console.SetVar("$bottomPrintActive", 0);
            console.SetVar("$CenterPrintSizes[1]", 20);
            console.SetVar("$CenterPrintSizes[2]", 36);
            console.SetVar("$CenterPrintSizes[3]", 56);
            }
        [Torque_Decorations.TorqueCallBack("", "", "clientCmdCenterPrint", "%message, %time, %size", 3, 38000, false)]
        public void clientCmdCenterPrint(string message, string time, string size)
            {
            if (console.GetVarBool("$centerPrintActive"))
                {
                if (console.Call("centerPrintDlg", "removePrint") != "")
                    console.Call("cancel", new string[] { "centerPrintDlg.removePrint" });
                }
            else
                {
                console.SetVar("CenterPrintDlg.isVisible", 1);
                console.SetVar("$centerPrintActive", 1);
                }
            console.Call("CenterPrintText", "setText", new string[] { "<just:center>" + message });
            console.Call("CenterPrintDlg", "Extent",
                     new string[] { console.GetVarString("CenterPrintDlg.extent").Split(' ')[0] + " " + console.GetVarString("$CenterPrintSizes[" + size + "]") });

            if (time.AsInt() > 0)
                console.SetVar("centerPrintDlg.removePrint", Util._schedule((time.AsInt() * 1000).AsString(), "0", "clientCmdClearCenterPrint"));
            }
        [Torque_Decorations.TorqueCallBack("", "", "clientCmdBottomPrint", "%message, %time, %size", 3, 38000, false)]
        public void clientCmdBottomPrint(string message, string time, string size)
            {

            if (console.GetVarBool("$bottomPrintActive"))
                {
                if (console.Call("bottomPrintDlg", "removePrint") != "")
                    console.Call("Cancel", new string[] { console.GetVarString("bottomPrintDlg.removePrint") });
                }
            else
                {
                GuiControl.setVisible("bottomPrintDlg", true);
                console.SetVar("$bottomPrintActive", 1);
                }
            GuiMLTextCtrl.setText("bottomPrintText", "<just:center>" + message);
            console.Call("bottomPrintDlg", "extent", new string[] { console.GetVarString("bottomPrintDlg.extent").Split(' ')[0] + " " + console.GetVarString("$CenterPrintSizes[" + size + "]") });

            if (time.AsInt() > 0)
                console.SetVar("bottomPrintDlg.removePrint", Util._schedule((time.AsInt() * 1000).AsString(), "0", "clientCmdClearbottomPrint"));
            }
        [Torque_Decorations.TorqueCallBack("", "BottomPrintText", "onResize", "%this, %width, %height", 3, 38000, false)]
        public void BottomPrintTextonResize(string thisobj, string width, string height)
            {
            console.SetVar(thisobj + ".position", "0 0");
            }
        [Torque_Decorations.TorqueCallBack("", "CenterPrintText", "onResize", "%this, %width, %height", 3, 38000, false)]
        public void CenterPrintTextOnResize(string thisobj, string width, string height)
            {
            console.SetVar(thisobj + ".position", "0 0");
            }
        [Torque_Decorations.TorqueCallBack("", "", "clientCmdClearCenterPrint", "", 0, 38000, false)]
        public void clientCmdClearCenterPrint()
            {
            console.SetVar("$centerPrintActive", 0);
            console.SetVar("CenterPrintDlg.visible", 0);
            console.SetVar("CenterPrintDlg.removePrint", "");
            }
        [Torque_Decorations.TorqueCallBack("", "", "clientCmdClearBottomPrint", "", 0, 38000, false)]
        public void clientCmdClearBottomPrint()
            {
            console.SetVar("$bottomPrintActive", 0);
            console.SetVar("BottomPrintDlg.visible", 0);
            console.SetVar("BottomPrintDlg.removePrint", "");
            }
        }
    }
