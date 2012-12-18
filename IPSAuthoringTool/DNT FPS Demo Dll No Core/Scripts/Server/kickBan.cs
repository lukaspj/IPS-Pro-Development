using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "kick", "(  %client )", 1, 16000, false)]
        public void Kick(string client)
            {
            console.Call("messageAll",
                     new[] { "MsgAdminForce", console.ColorEncode(@"\c2The Admin has kicked %1."), console.GetVarString(string.Format("{0}.playerName", client)) });
            if (!GameConnection.isAIControlled(client))
                console.Call_Classname("BanList", "add",
                           new string[] { console.GetVarString(client + ".guid"), NetConnection.getAddress(client), console.GetVarString("$Pref::Server::KickBanTime") });

            console.Call(client, "delete", new[] { "You have been kicked from this server" });
            }

        [Torque_Decorations.TorqueCallBack("", "", "ban", "(  %client )", 1, 16000, false)]
        public void Ban(string client)
            {
            console.Call("messageAll",
                     new[] { "MsgAdminForce", console.ColorEncode(@"\c2The Admin has banned %1."), console.GetVarString(string.Format("{0}.playerName", client)) });
            if (!GameConnection.isAIControlled(client))
                console.Call_Classname("BanList", "add",
                           new string[] { console.GetVarString(client + ".guid"), NetConnection.getAddress(client), console.GetVarString("$Pref::Server::BanTime") });
            console.Call(client, "delete", new[] { "You have been banned from this server" });
            }
        }
    }
