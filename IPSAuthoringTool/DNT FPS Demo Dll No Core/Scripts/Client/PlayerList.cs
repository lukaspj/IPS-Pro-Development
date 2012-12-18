using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // Hook into the client update messages to maintain our player list and scores.
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "playerlist_defaults_load", "()",  0, 3000, true)]
        public string PlayerlistDefaultsLoad()
            {
            console.Eval("addMessageCallback('MsgClientJoin', handleClientJoin);");
            console.Eval("addMessageCallback('MsgClientDrop', handleClientDrop);");
            console.Eval("addMessageCallback('MsgClientScoreChanged', handleClientScoreChanged);");
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "handleClientJoin",
            "(%msgType, %msgString, %clientName, %clientId, %guid, %score, %kills, %deaths, %isAI, %isAdmin, %isSuperAdmin)",  11, 3000, false)]
        public string HandleClientJoin(string msgType, string msgString, string ClientName, string clientID, string guid, string score, string kills,
                                       string deaths, string isAI, string isAdmin, string isSuperAdmin)
            {
            PlayerListGuiUpdate("PlayerListGui", clientID, Util.detag(ClientName), isSuperAdmin, isAdmin, isAI, score, kills, deaths);
            
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "handleClientDrop", "(%msgType, %msgString, %clientName, %clientId)",  4, 3000, false)]
        public string HandleClientDrop(string msgType, string msgString, string clientName, string clientId)
            {
            PlayerListGuiRemove("PlayerListGui", clientId);
            
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "handleClientScoreChanged", "(%msgType, %msgString, %score, %kills, %deaths, %clientId)",  6,
            3000, false)]
        public void HandleClientScoreChanged(string msgType, string msgString, string score, string kills, string deaths, string clientId)
            {
            PlayerListGuiUpdateScore("PlayerListGui", clientId, score, kills, deaths);
            
            console.print(string.Format(" score:{0} kills:{1} deaths: {2}", score, kills, deaths));
            }

        // ----------------------------------------------------------------------------
        // GUI methods
        // ----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "PlayerListGui", "update",
            "(%this, %clientId, %name, %isSuperAdmin, %isAdmin, %isAI, %score, %kills, %deaths)",  9, 3000, false)]
        public void PlayerListGuiUpdate(string thisobj, string clientId, string name, string isSuperAdmin, string isAdmin, string isAI, string score,
                                          string kills, string deaths)
            {

            // Build the row to display.  The name can have ML control tags, including
            // color and font.  Since we're not using an ML control here, we need to
            // strip them off.

            string tag = isSuperAdmin.AsBool() ? "[Super]" : (isAdmin.AsBool() ? "[Admin]" : (isAI.AsBool() ? "[Bot]" : ""));

            string text = string.Format("{0} {1}\t{2}\t{3}\t{4}", Util.StripMLControlChars(name), tag, score, kills, deaths);
            // Update or add the player to the control
            console.Call("PlayerListGuiList", 
                console.Call("PlayerListGuiList", "getRowNumByID", new[] {clientId}).AsInt() == -1 ? "addRow" : "setRowByID",
                     new[] {clientId, text});

            // Sorts by score
            GuiTextListCtrl.sortNumerical("PlayerListGuiList",1,false);
            GuiTextListCtrl.clearSelection("PlayerListGuiList");
            }

        [Torque_Decorations.TorqueCallBack("", "PlayerListGui", "updateScore", "(%this, %clientId, %score, %kills, %deaths)",  5, 3000, false)]
        public string PlayerListGuiUpdateScore(string thisobj, string clientId, string score, string kills, string deaths)
            {
            string text = console.Call("PlayerListGuiList", "getRowTextByID", new string[] {clientId});
            //Since I'm lazy and I don't feel like writing a csharp function...

            text = Util.setField(text, 1, score);
            text = Util.setField(text, 2, kills);
            text = Util.setField(text, 3, deaths);

            GuiTextListCtrl.setRowById("PlayerListGuiList", clientId.AsInt(),text);

            
            // Sorts by score
            GuiTextListCtrl.sortNumerical("PlayerListGuiList",1,false);
            GuiTextListCtrl.clearSelection("PlayerListGuiList");
            
            return string.Empty;
            }
        [Torque_Decorations.TorqueCallBack("", "PlayerListGui", "remove", "(%this, %clientId)",  2, 3000, false)]
        public string PlayerListGuiRemove(string thisobj,string clientID)
            {
            GuiTextListCtrl.removeRowById("PlayerListGuiList",clientID.AsInt());
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "PlayerListGui", "toggle", "(%this)",  1, 3000, false)]
        public string PlayerListGuiToggle(string thisobj)
            {
            if (GuiControl.isAwake(thisobj))
            GuiCanvas.popDialog("Canvas",thisobj);
            
            else
                GuiCanvas.pushDialog("Canvas",thisobj);
            
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "PlayerListGui", "clear", "(%this)",  1, 3000, false)]
        public string PlayerListGuiClear(string thisobj)
            {
            GuiTextListCtrl.clear("PlayerListGuiList");
            return string.Empty;
            }
        [Torque_Decorations.TorqueCallBack("", "PlayerListGui", "zeroScores", "(%this)",  1, 3000, false)]
        public string PlayerListGuiZeroScores(string thisobj)
            {
            for (int i = 0;i<console.Call("PlayerListGuiList","rowCount").AsInt();i++)
                {
                string text = GuiTextListCtrl.getRowText("PlayerListGuiList", i);
                text = Util.setField(text, 1, "0");
                text = Util.setField(text, 2, "0");
                text = Util.setField(text, 3, "0");
                GuiTextListCtrl.setRowById("PlayerListGuiList", GuiTextListCtrl.getRowId("PlayerListGuiList",i),text);
                }
            GuiTextListCtrl.clearSelection("PlayerListGuiList");
            return string.Empty;
            }
        }
    }