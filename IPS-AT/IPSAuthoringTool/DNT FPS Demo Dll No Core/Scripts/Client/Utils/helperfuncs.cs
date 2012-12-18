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

        //---------------------------------------------------------------------------------------------
        // formatImageNumber
        // Preceeds a number with zeros to make it 6 digits long.
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "validateDatablockName", "(%name)", 1, 33000, false)]
        public string ValidateDatablockName(string name)
            {
            // remove whitespaces at beginning and end
            name = name.Trim();
            string numbers = "1234567890";
            // remove numbers at the beginning
            while (name.Length > 0)
                {
                // the first character
                char firstchar = name[0];
                // if the character is a number remove it
                if (numbers.Contains(firstchar))
                    {
                    numbers = numbers.Substring(1).Trim();
                    }
                else
                    {
                    break;
                    }
                }
            // replace whitespaces with underscores
            name = name.Replace(" ", "_");
            // remove any other invalid characters

            const string toremove = "-+*/%$&§=()[].?\"#,;!~<>|°^{}";
            name = toremove.Aggregate(name, (current, c) => current.Replace(c + "", ""));
            if (name == "")
                name = "Unnamed";
            return name;
            }


        //--------------------------------------------------------------------------
        // Finds location of %word in %text, starting at %start.  Works just like strPos
        //--------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "wordPos", "(%text, %word, %start)", 3, 33000, false)]
        public int wordPos(string text, string word, string start)
            {
            string[] words = word.Split(' ');
            int counter = 0;
            foreach (string s in words)
                {
                if (s.Trim() == word)
                    {
                    if (counter >= start.AsInt())
                        return counter;
                    }
                counter++;
                }
            return -1;
            }

        [Torque_Decorations.TorqueCallBack("", "", "fieldPos", "(%text, %field, %start)", 3, 33000, false)]
        public int fieldPos(string text, string field, string start)
            {
            if (Util.strpos(text, field, 0) == -1) return -1;

            int count = Util.getFieldCount(text);
            if (start.AsInt() > count) return -1;
            for (int i = start.AsInt(); i < count; i++)
                if (Util.getField(text, i) == field) return i;
            return -1;
            }

        [Torque_Decorations.TorqueCallBack("", "", "loadFileText", "(%file)", 1, 33000, false)]
        public string loadFileText(string file)
            {
            string fo = new Torque_Class_Helper("FileObject").Create(m_ts).AsString();
            FileObject.openForRead(fo, file);
            string text = "";
            while (!FileObject.isEOF(fo))
                {
                text += FileObject.readLine(fo);
                if (!FileObject.isEOF(fo)) text += "\n";
                }
            console.Call(fo, "delete");
            return text;
            }

        [Torque_Decorations.TorqueCallBack("", "", "setValueSafe", "(%dest, %val)", 2, 33000, false)]
        public void setValueSafe(string dest, string val)
            {
            string cmd = console.GetVarString(dest + ".command");
            string alt = console.GetVarString(dest + ".altCommand");
            console.SetVar(dest + ".command", "");
            console.SetVar(dest + ".altCommand", "");

            console.Call(dest, "setValue", new string[] { val });
            console.SetVar(dest + ".command", cmd);
            console.SetVar(dest + ".altCommand", alt);
            }

        [Torque_Decorations.TorqueCallBack("", "", "shareValueSafe", "(%source, %dest)", 2, 33000, false)]
        public void shareValueSafe(string source, string dest)
            {
            setValueSafe(source, dest);
            }

        [Torque_Decorations.TorqueCallBack("", "", "shareValueSafeDelay", "(%source, %dest, %delayMs)", 3, 33000, false)]
        public void shareValueSafeDelay(string source, string dest, string delayMS)
            {
            Util._schedule(delayMS, "0", "shareValueSafe", source, dest);
            }

        //------------------------------------------------------------------------------
        // An Aggregate Control is a plain GuiControl that contains other controls, 
        // which all share a single job or represent a single value.
        //------------------------------------------------------------------------------

        // AggregateControl.setValue( ) propagates the value to any control that has an 
        // internal name.
        [Torque_Decorations.TorqueCallBack("", "AggregateControl", "setValue", "(%this, %val, %child)", 3, 33000, false)]
        public void AggregateControlsetValue(string thisobj, string val, string child)
            {
            for (uint i = 0; i < SimSet.getCount(thisobj); i++)
                {
                string obj = SimSet.getObject(thisobj, i);
                if (obj == child)
                    continue;
                if (console.GetVarString(obj + ".internalName") != "")
                    setValueSafe(obj, val);
                }
            }

        // AggregateControl.getValue() uses the value of the first control that has an
        // internal name, if it has not cached a value via .setValue
        [Torque_Decorations.TorqueCallBack("", "AggregateControl", "getValue", "(%this)", 1, 33000, false)]
        public string AggregateControlgetValue(string thisobj)
            {
            for (uint i = 0; i < SimSet.getCount(thisobj); i++)
                {
                string obj = SimSet.getObject(thisobj, i);
                if (console.GetVarString(obj + ".internalName") != "")
                    return console.Call(obj, "getValue");
                }
            return "";
            }

        // AggregateControl.updateFromChild( ) is called by child controls to propagate
        // a new value, and to trigger the onAction() callback.
        [Torque_Decorations.TorqueCallBack("", "AggregateControl", "updateFromChild", "(%this, %child)", 2, 33000, false)]
        public void AggregateControlupdateFromChild(string thisobj, string child)
            {
            string val = console.Call(child, "getValue");
            if (val.AsFloat() != Math.Ceiling(val.AsFloat()))
                {
                if (val.AsFloat() <= -100)
                    {
                    val = Util.mCeil(val.AsFloat()).AsString();
                    }
                else if (val.AsFloat() <= -10)
                    {
                    val = Util.mFloatLength(val.AsFloat(), 1);
                    }
                else if (val.AsFloat() < 0)
                    {
                    val = Util.mFloatLength(val.AsFloat(), 2);
                    }
                else if (val.AsFloat() >= 1000)
                    {
                    val = Util.mCeil(val.AsFloat()).AsString();
                    }
                else if (val.AsFloat() >= 100)
                    {
                    val = Util.mFloatLength(val.AsFloat(), 1);
                    }
                else if (val.AsFloat() >= 10)
                    {
                    val = Util.mFloatLength(val.AsFloat(), 2);
                    }
                else if (val.AsFloat() > 0)
                    {
                    val = Util.mFloatLength(val.AsFloat(), 3);
                    }
                }
            console.Call(thisobj, "setValue", new string[] { val, child });
            console.Call(thisobj, "onAction");
            }

        [Torque_Decorations.TorqueCallBack("", "AggregateControl", "onAction", "(%this)", 1, 33000, false)]
        public void AggregateControlonAction(string thisobj)
            {

            }

        // call a method on all children that have an internalName and that implement the method.
        [Torque_Decorations.TorqueCallBack("", "AggregateControl", "callMethod", "(%this, %method, %args)", 3, 33000, false)]
        public void AggregateControlcallMethod(string thisobj, string method, string args)
            {
            for (int i = 0; i < console.Call(thisobj, "getCount").AsInt(); i++)
                {
                string obj = console.Call(thisobj, "getObject", new string[] { i.AsString() });
                if (console.GetVarString(obj + ".internalName") != "" && console.Call(obj, "isMethod", new string[] { method }).AsBool())
                    console.Eval(obj + "." + method + "(" + args + ");");
                }
            }

        // A function used in order to easily parse the MissionGroup for classes . I'm pretty 
        // sure at this point the function can be easily modified to search the any group as well.
        [Torque_Decorations.TorqueCallBack("", "", "parseMissionGroup", "(%className, %childGroup)", 2, 33000, false)]
        public string parseMissionGroup(string className, string childGroup)
            {
            string currentGroup = "";
            currentGroup = Util.getWordCount(childGroup) == 0 ? "MissionGroup" : childGroup;

            for (int i = 0; i < console.Call(currentGroup, "getCount").AsInt(); i++)
                {
                if (console.Call(currentGroup, "getObject", new string[] { i.AsString() }) == className)
                    return "1";

                if (console.Call(currentGroup, "getObject", new string[] { i.AsString() }) != "SimGroup") continue;
                if (parseMissionGroup(className, console.Call(console.Call(currentGroup, "getObject", new string[] { i.AsString() }), "getID")).AsBool())
                    return "1";
                }
            return "0";
            }


        [Torque_Decorations.TorqueCallBack("", "", "parseMissionGroupForIds", "(%className, %childGroup)", 2, 33000, false)]
        public string parseMissionGroupForIds(string className, string childGroup)
            {
            string classIds = "";
            string currentGroup = "";
            currentGroup = Util.getWordCount(childGroup) == 0 ? "MissionGroup" : childGroup;

            for (int i = 0; i < console.Call(currentGroup, "getCount").AsInt(); i++)
                {
                if (console.Call(currentGroup, "getObject", new string[] { i.AsString() }) == className)
                    classIds += console.Call(console.Call(currentGroup, "getObject", new string[] { i.AsString() }), "getID") + " ";
                if (console.Call(currentGroup, "getObject", new string[] { i.AsString() }) == "SimGroup")
                    classIds +=
                        parseMissionGroupForIds(className, console.Call(console.Call(currentGroup, "getObject", new string[] { i.AsString() }), "getID")).AsBool() + " ";
                }
            return classIds;
            }

        //------------------------------------------------------------------------------
        // Altered Version of TGB's QuickEditDropDownTextEditCtrl
        //------------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "QuickEditDropDownTextEditCtrl", "onRenameItem", "(%this)", 1, 33000, false)]
        public void QuickEditDropDownTextEditCtrlonRenameItem(string thisobj)
            {
            }
        [Torque_Decorations.TorqueCallBack("", "QuickEditDropDownTextEditCtrl", "updateFromChild", "(%this, %ctrl )", 2, 33000, false)]
        public void QuickEditDropDownTextEditCtrlupdateFromChild(string thisobj, string ctrl)
            {
            switch (console.GetVarString(ctrl + ".internalName"))
                {
                    case "PopUpMenu":
                        SimSet.findObjectByInternalName(thisobj, console.Call(ctrl, "getText"), false);
                        break;
                    case "TextEdit":
                        {
                        string popup = SimSet.findObjectByInternalName(thisobj, "PopupMenu", false);
                        console.Call(popup, "changeTextByID", new string[] { console.Call(popup, "getSelected"), console.Call(ctrl, "getText") });
                        console.Call(thisobj, "onRenameItem");
                        }
                        break;
                }
            }
        }
    }
