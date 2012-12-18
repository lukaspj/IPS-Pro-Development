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
        [Torque_Decorations.TorqueCallBack("", "HelpDlg", "onWake", "(this)", 1, 30000, false)]
        public void HelpDlgonWake(string thisobj)
            {
            console.SetVar("HelpFileList.entryCount", 0);
            console.Call("HelpFileList", "clear");
            for (string file = Util.findFirstFile("*.hfl", true); file != ""; file = Util.findNextFile("*.hfl"))
                {
                console.SetVar("HelpFileList.fileName[" + console.GetVarString("HelpFileList.entryCount") + "]", file);
                console.Call("HelpFileList", "addRow", new string[] { console.GetVarString("HelpFileList.entryCount"), Util.fileBase(file) });
                console.SetVar("HelpFileList.entryCount", console.GetVarInt("HelpFileList.entryCount") + 1);
                }
            console.Call("HelpFileList", "sortNumerical", new string[] { "0" });
            for (int i = 0; i < console.GetVarInt("HelpFileList.entryCount"); i++)
                {
                string rowid = console.Call("HelpFileList", "getRowId", new string[] { i.AsString() });
                string text = console.Call("HelpFileList", "getRowTextById", new string[] { rowid });
                text = (i + 1).AsString() + ". " + Util.restWords(text);
                console.Call("HelpFileList", "setRowByID", new string[] { rowid, text });
                }
            console.Call("HelpFileList", "setSelectedRow", new string[] { "0" });
            }

        [Torque_Decorations.TorqueCallBack("", "HelpFileList", "onSelect", "(this,row)", 2, 30000, false)]
        public void HelpFileListonSelect(string thisobj, string row)
            {
            string fo = new Torque_Class_Helper("FileObject").Create(m_ts).AsString();
            FileObject.openForRead(fo, console.GetVarString(thisobj + ".fileName[" + row + "]"));
            string text = "";
            while (!FileObject.isEOF(fo))
                text += FileObject.readLine(fo) + "\n";

            FileObject.close(fo);
            console.Call("HelpText", "setText", new string[] { text });
            }
        [Torque_Decorations.TorqueCallBack("", "", "getHelp", "(%helpName)", 1, 30000, false)]
        public void getHelp(string helpname)
            {
            console.Call("Canvas", "pushDialog", new string[] { "HelpDlg" });
            if (helpname == "") return;
            int index = GuiTextListCtrl.findTextIndex("HelpFileList", helpname);
            GuiTextListCtrl.setSelectedRow("HelpFileList", index);
            }
        [Torque_Decorations.TorqueCallBack("", "", "contextHelp", "(%helpName)", 0, 30000, false)]
        public void contextHelp()
            {
            for (int i = 0; i < console.Call("Canvas", "getCount").AsInt(); i++)
                {
                if (console.Call(console.Call("Canvas", "getObject", new string[] { i.AsString() }), "getName") == console.GetVarString("HelpDlg"))
                    {
                    console.Call("Canvas", "popDialog", new string[] { console.GetVarString("HelpDlg") });
                    return;
                    }
                }
            string content = console.Call("Canvas", "getContent");
            string helpPage = console.Call(content, "getHelpPage");
            console.Call("getHelp", new string[] { helpPage });
            }

        [Torque_Decorations.TorqueCallBack("", "GuiControl", "getHelpPage", "(%this)", 1, 30000, false)]
        public string GuiControlgetHelpPage(string thisobj)
            {
            return console.GetVarString(thisobj + ".helpPage");
            }
        [Torque_Decorations.TorqueCallBack("", "GuiMLTextCtrl", "onURL", "(%this,url)", 2, 30000, false)]
        public void GuiMLTextCtrlonURL(string thisobj, string url)
            {
            console.Call("gotoWebPage", new string[] { url });
            }
        }
    }
