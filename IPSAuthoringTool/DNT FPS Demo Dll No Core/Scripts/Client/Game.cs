using WinterLeaf.Classes;
using WinterLeaf.Containers;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
{
    //-----------------------------------------------------------------------------
    // Torque
    // Copyright GarageGames, LLC 2011
    //-----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    // Game start / end events sent from the server
    //----------------------------------------------------------------------------
    public partial class Main : TorqueScriptTemplate
    {


        [Torque_Decorations.TorqueCallBack("", "", "clientCmdGameStart", "(%seq)", 1, 7000, false)]
        public void ClientCmdGameStart(string seq)
        {
            //Camera.setOrbitPoint(GameConnection.getCameraObject("LocalClientConnection"), new TransformF(0, 0, 0), 5, 16, 12, new Point3F(0, 0, 0), false);
        }

        [Torque_Decorations.TorqueCallBack("", "", "EditorIsActive", "()", 0, 0, false)]
        public bool EditorIsActive()
        {
            return (console.isObject("EditorGui") &&
                GuiCanvas.getContent("Canvas") == console.Call("EditorGui", "getId").AsInt());
        }

        [Torque_Decorations.TorqueCallBack("", "", "GuiEditorIsActive", "()", 0, 0, false)]
        public bool GuiEditorIsActive()
        {
            return (console.isObject("GuiEditorGui") &&
                GuiCanvas.getContent("Canvas") == console.Call("GuiEditorGui", "getId").AsInt());
        }

        [Torque_Decorations.TorqueCallBack("", "", "clientCmdGameEnd", "(%seq)", 1, 7000, false)]
        public void ClientCmdGameEnd(string endgamepause)
        {
            sfxStopAll("");

            if (!EditorIsActive() && !GuiEditorIsActive())
            {
                // Copy the current scores from the player list into the
                // end game gui (bit of a hack for now).
                console.Call("EndGameGuiList", "clear");
                for (int i = 0; i < console.Call("PlayerListGuiList", "rowCount").AsInt(); i++)
                {
                    string text = console.Call("PlayerListGuiList", "getRowText", new string[] { i.AsString() });
                    string id = console.Call("PlayerListGuiList", "getRowID", new string[] { i.AsString() });
                    GuiTextListCtrl.addRow("EndGameGuiList", id.AsInt(), text, -1);
                }
                GuiTextListCtrl.sortNumerical("EndGameGuiList", 1, false);
                GuiCanvas.setContent("Canvas", "EndGameGui");

                if (endgamepause.AsInt() > 0)
                    Util._schedule((endgamepause.AsInt() * 1000).AsString(), "0", "ShowLoading");

            }
        }
        [Torque_Decorations.TorqueCallBack("", "", "ShowLoading", "", 0, 7000, false)]
        public void ShowLoading()
        {
            GuiCanvas.setContent("Canvas", "MainMenuGui");
        }
    }
}
