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
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // PlayGui is the main TSControl through which the game is viewed.
        // The PlayGui also contains the hud controls.
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "PlayGui", "onWake", "(this)",  1, 4900, false)]
        public void PlayGuiOnWake(string thisobj)
            {
            // Turn off any shell sounds...
            // sfxStop( ... );

            console.SetVar("$enableDirectInput", 1);
            console.Call("activateDirectInput");
            // Message hud dialog
            if (console.isObject("MainChatHud"))
                {
                GuiCanvas.pushDialog("Canvas", "MainChatHud");
                GuiMessageVectorCtrl.attach("chatHud", "HudMessageVector");
                }
            // just update the action map here
            ActionMap.push("moveMap");
            // hack city - these controls are floating around and need to be clamped
            if (console.isFunction("refreshCenterTextCtrl"))
                Util._schedule("0", "0", "refreshCenterTextCtrl");
            if (console.isFunction("refreshBottomTextCtrl"))
                Util._schedule("0", "0", "refreshBottomTextCtrl");
            }

        [Torque_Decorations.TorqueCallBack("", "PlayGui", "onSleep", "(this)",  1, 4900, false)]
        public void PlayGuiOnSleep(string thisobj)
            {
            if (console.isObject("MainChatHud"))
                GuiCanvas.popDialog("Canvas", "MainChatHud");
            ActionMap.pop("moveMap");
            }
        [Torque_Decorations.TorqueCallBack("", "PlayGui", "clearHud", "(this)",  1, 4900, false)]
        public void PlayGuiclearHud(string thisobj)
            {
            GuiCanvas.popDialog("Canvas", "MainChatHud");
            while (SimSet.getCount(thisobj) > 0)
                console.Call(SimSet.getObject(thisobj, 0), "delete");
            }

        [Torque_Decorations.TorqueCallBack("", "", "refreshBottomTextCtrl", "(this)",  0, 4900, false)]
        public void refreshBottomTextCtrl()
            {
            console.SetVar("BottomPrintText.position","0 0");
            }
        [Torque_Decorations.TorqueCallBack("", "", "refreshCenterTextCtrl", "(this)",  0, 4900, false)]
        public void refreshCenterTextCtrl()
            {
            console.SetVar("CenterPrintText.position", "0 0");
            }

        }
    }
