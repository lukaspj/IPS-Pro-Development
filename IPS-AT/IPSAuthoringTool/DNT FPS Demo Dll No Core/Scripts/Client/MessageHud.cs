﻿using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //----------------------------------------------------------------------------
        // Enter Chat Message Hud
        //----------------------------------------------------------------------------

        //------------------------------------------------------------------------------


        [Torque_Decorations.TorqueCallBack("", "MessageHud", "open", "(this)",  1, 5000, false)]
        public string MessageHudOpen(string thisobj)
            {
            const int offset = 6;


            if (GuiControl.isVisible(thisobj))
                return string.Empty;

            string text = console.GetVarBool(thisobj + ".isTeamMsg") ? "TEAM:" : "GLOBAL:";

            GuiControl.setValue("MessageHud_Text", text);

            string windowPos = "0 " + console.GetVarString("outerChatHud.position").Split(' ')[1] +
                               (console.GetVarString("outerChatHud.extent").Split(' ')[1].AsInt() + 1).AsString();
            string windowExt = string.Format("{0} {1}", console.GetVarString("OuterChatHud.extent").Split(' ')[0], console.GetVarString("MessageHud_Frame.extent").Split(' ')[1]);

            int textExtent = console.GetVarString("MessageHud_Text.extent").Split(' ')[0].AsInt() + 14;
            int ctrlExtent = console.GetVarString("MessageHud_Frame.extent").Split(' ')[0].AsInt();

            GuiCanvas.pushDialog("Canvas",thisobj);

            console.SetVar("messageHud_Frame.position", windowPos);
            console.SetVar("messageHud_Frame.extent", windowExt);

            console.SetVar("MessageHud_Edit.position", Util.setWord(console.GetVarString("MessageHud_Edit.position"), 0, (textExtent + offset).AsString()));
            console.SetVar("MessageHud_Edit.extent",
                       Util.setWord(console.GetVarString("MessageHud_Edit.extent"), 0, ((ctrlExtent - textExtent - (2*offset))).AsString()));

            GuiControl.setVisible(thisobj,true);

            
            console.Call("deactivateKeyboard");

            GuiControl.makeFirstResponder("MessageHud_Edit","true");

            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "MessageHud", "close", "(this)",  1, 5000, false)]
        public string MessageHudClose(string thisobj)
            {
            if (!GuiControl.isVisible(thisobj))
                return string.Empty;

            GuiCanvas.popDialog("Canvas", thisobj);
            GuiControl.setVisible(thisobj,false);

            if (console.GetVarBool("$enableDirectInput"))
                console.Call("activateKeyboard");

            GuiControl.setValue("MessageHud_Edit","");
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "MessageHud", "toggleState", "(this)",  1, 5000, false)]
        public string MessageHudToggleState(string thisobj)
            {
            console.Call(thisobj,GuiControl.isVisible(thisobj) ? "close" : "open");
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "MessageHud_Edit", "onEscape", "(this)",  1, 5000, false)]
        public string MessageHudEditOnEscape(string thisobj)
            {
            console.Call("MessageHud", "close");
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "MessageHud_Edit", "eval", "(this)",  1, 5000, false)]
        public string MessageHudEditEval(string thisobj)
            {
            string text = Util.collapseEscape(console.Call(thisobj, "getValue").Trim());
            if (text != "")
                {
                console.commandToServer(console.GetVarString("MessageHud.isTeamMsg").AsBool() ? "teamMessageSent" : "messageSent", new string[] { text });
                }
            console.Call("MessageHud", "close");
            return string.Empty;
            }

        //----------------------------------------------------------------------------
        // MessageHud key handlers
        [Torque_Decorations.TorqueCallBack("", "", "toggleMessageHud", "(make)",  1, 5000, false)]
        public string ToggleMessageHud(string make)
            {
            if (make.AsBool())
                {
                console.SetVar("MessageHud.isTeamMsg", false);
                console.Call("MessageHud", "toggleState");
                }
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "teamMessageHud", "(make)",  1, 5000, false)]
        public string TeamMessageHud(string make)
            {
            if (make.AsBool())
                {
                console.SetVar("MessageHud.isTeamMsg", true);
                console.Call("MessageHud", "toggleState");
                }
            return string.Empty;
            }

        }
    }
