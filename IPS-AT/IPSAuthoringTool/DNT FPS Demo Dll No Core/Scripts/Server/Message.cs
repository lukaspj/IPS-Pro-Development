using System;
using WinterLeaf.Classes;
using System.Linq;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {


        [Torque_Decorations.TorqueCallBack("", "", "InitializeMessage", "()", 0, 11000, true)]
        public void InitializeMessage()
            {
            console.SetVar("$SPAM_PROTECTION_PERIOD", 1000);
            console.SetVar("$SPAM_MESSAGE_THRESHOLD", 4);
            console.SetVar("$SPAM_PENALTY_PERIOD", 10000);
            console.SetVar("$SPAM_MESSAGE", console.ColorEncode(@"\c3FLOOD PROTECTION:\cr You must wait another %1 seconds."));
            }


        //[Torque_Decorations.TorqueCallBack("", "", "messageClient",
        //    "(%client, %msgType, %msgString, %a1, %a2, %a3, %a4, %a5, %a6, %a7, %a8, %a9, %a10, %a11, %a12, %a13)", 16, 11000, false)]
        //public void MessageClient(string client, string msgType, string msgString, string a1, string a2, string a3, string a4, string a5, string a6, string a7, string a8, string a9, string a10, string a11, string a12, string a13)
        //    {
        //    console.commandToClient(client, "ServerMessage", new[] { msgType, msgString, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13 });
        //    }

        [Torque_Decorations.TorqueCallBack("", "", "messageTeam",
            "(%team, %msgType, %msgString, %a1, %a2, %a3, %a4, %a5, %a6, %a7, %a8, %a9, %a10, %a11, %a12, %a13)", 16, 11000, false)]
        public void MessageTeam(string team, string msgType, string msgString, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "", string a11 = "", string a12 = "", string a13 = "")
            {
            foreach (uint clientid in ClientGroup.Where(clientid => console.GetVarString(clientid.AsString() + ".team") == team))
                MessageClient(clientid.AsString(), msgType, msgString, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
            }

        [Torque_Decorations.TorqueCallBack("", "", "messageTeamExcept",
            "(%client, %msgType, %msgString, %a1, %a2, %a3, %a4, %a5, %a6, %a7, %a8, %a9, %a10, %a11, %a12, %a13)", 16, 11000, false)]
        public void MessageTeamExcept(string client, string msgType, string msgString, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "", string a11 = "", string a12 = "", string a13 = "")
            {
            string team = console.GetVarString(client + ".team");
            foreach (
                uint clientid in ClientGroup.Where(clientid => (console.GetVarString(clientid.AsString() + ".team") == team) && (clientid.AsString() != client)))
                MessageClient(clientid.AsString(), msgType, msgString, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
            }



        [Torque_Decorations.TorqueCallBack("", "", "messageAllExcept",
            "(%client, %team, %msgtype, %msgString, %a1, %a2, %a3, %a4, %a5, %a6, %a7, %a8, %a9, %a10, %a11, %a12, %a13)", 17, 11000, false)]
        public void MessageAllExcept(string client, string team, string msgtype, string msgstring, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "", string a11 = "", string a12 = "", string a13 = "")
            {
            foreach (
                uint recipient in ClientGroup.Where(recipient => (recipient.AsString() != client) && (console.GetVarString(recipient.AsString() + ".team") != team))
                )
                MessageClient(recipient.AsString(), msgtype, msgstring, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
            }

        //---------------------------------------------------------------------------
        // Server side client chat'n
        //---------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "spamMessageTimeout", "%this", 1, 11000, false)]
        public void GameConnectSpamMessageTimeOut(string thisobj)
            {
            if (console.GetVarInt(string.Format("{0}.spamMessageCount", thisobj)) > 0)
                console.SetVar(string.Format("{0}.spamMessageCount", thisobj), console.GetVarInt(string.Format("{0}.spamMessageCount", thisobj)) - 1);
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "spamReset", "%this", 1, 11000, false)]
        public void GameConnectionspamReset(string thisobj)
            {
            console.SetVar(string.Format("{0}.isSpamming", thisobj), false);
            }

        [Torque_Decorations.TorqueCallBack("", "", "spamAlert", "%client", 1, 11000, false)]
        public bool SpamAlert(string client)
            {
            if (console.GetVarBool("$Pref::Server::FloodProtectionEnabled") != true)
                return false;

            if (!console.GetVarBool(client + ".isSpamming") && console.GetVarInt(client + ".spamMessageCount") >= console.GetVarInt("$SPAM_MESSAGE_THRESHOLD"))
                {
                console.error("Client " + client + " is spamming, message count = " + console.GetVarInt(client + ".spamMessageCount"));
                console.SetVar(client + ".spamProtectStart", console.getSimTime());
                console.SetVar(client + ".isSpamming", true);
                SimObject.schedule(client, console.GetVarString("$SPAM_PENALTY_PERIOD"), "spamReset");
                }

            if (console.GetVarBool(client + ".isSpamming"))
                {
                double wait = Math.Floor((console.GetVarInt("$SPAM_PENALTY_PERIOD") - (console.getSimTime() - console.GetVarDouble(client + "spamProtectStart")) / 1000));
                MessageClient(client, "", console.GetVarString("$SPAM_MESSAGE"), wait.AsString());
                return true;
                }

            console.SetVar(string.Format("{0}.spamMessageCount", client), console.GetVarInt(string.Format("{0}.spamMessageCount", client)) + 1);
            SimObject.schedule(client, console.GetVarString("$SPAM_PROTECTION_PERIOD"), "spamMessageTimeout");
            return false;
            }

        [Torque_Decorations.TorqueCallBack("", "", "chatMessageClient",
            "%client, %sender, %voiceTag, %voicePitch, %msgString, %a1, %a2, %a3, %a4, %a5, %a6, %a7, %a8, %a9, %a10 ", 15, 11000, false)]
        public void ChatMessageClient(string client, string sender, string voiceTag, string voicePitch, string msgString, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "")
            {
            if (!console.GetVarBool(string.Format("{0}.muted[{1}]", client, sender)))
                {
                console.commandToClient(client, "ChatMessage", new[] { sender, voiceTag, voicePitch, console.addTaggedString(msgString), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 });
                }

            }

        [Torque_Decorations.TorqueCallBack("", "", "chatMessageTeam", " %sender, %team, %msgString, %a1, %a2, %a3, %a4, %a5, %a6, %a7, %a8, %a9, %a10", 13, 11000, false)]
        public void ChatMessageTeam(string sender, string team, string msgString, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "")
            {
            if ((msgString.Trim().Length == 0) || SpamAlert(sender))
                return;

            foreach (uint obj in ClientGroup.Where(obj => console.GetVarString(string.Format("{0}.team", obj)) == console.GetVarString(string.Format("{0}.team", sender))))
                ChatMessageClient(obj.AsString(), sender, console.GetVarString(string.Format("{0}.voiceTag", sender)), console.GetVarString(string.Format("{0}.voicePitch", sender)), msgString, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
            }

        [Torque_Decorations.TorqueCallBack("", "", "chatMessageAll", " %sender, %msgString, %a1, %a2, %a3, %a4, %a5, %a6, %a7, %a8, %a9, %a10", 12, 11000, false)]
        public void ChatMessageAll(string sender, string msgString, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "")
            {
            if ((msgString.Trim().Length == 0) || SpamAlert(sender))
                return;
            foreach (uint obj in ClientGroup)
                {
                if (console.GetVarInt(string.Format("{0}.team", sender)) != 0)
                    ChatMessageClient(obj.AsString(), sender, console.GetVarString(string.Format("{0}.voiceTag", sender)),
                                     console.GetVarString(string.Format("{0}.voicePitch", sender)), msgString, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
                else
                    {
                    if (console.GetVarString(string.Format("{0}.team", obj)) == console.GetVarString(string.Format("{0}.team", sender)))
                        ChatMessageClient(obj.AsString(), sender, console.GetVarString(string.Format("{0}.voiceTag", sender)), console.GetVarString(string.Format("{0}.voicePitch", sender)), msgString, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
                    }
                }
            }


        [Torque_Decorations.TorqueCallBack("", "", "messageClient", "", 16, 11000, false)]
        public void MessageClient(string client, string msgtype, string msgstring, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "", string a11 = "", string a12 = "", string a13 = "")
            {
            string function = console.addTaggedString("ServerMessage");


            string tmsgtype = "";
            if (msgtype.Length > 0)
                tmsgtype = (byte)msgtype[0] == (byte)1 ? msgtype : console.addTaggedString(msgtype);
            string tmsgstring = "";
            if (msgstring.Length > 0)
                tmsgstring = (byte)msgstring[0] == (byte)1 ? msgstring : console.addTaggedString(msgstring);

            //console.error("Sending " + client + " message '" + console.getTaggedString(function) + "' - ' " + console.getTaggedString(tmsgtype) + "' - '" + tmsgstring + "'");
            console.commandToClient(client, function, new[] { tmsgtype, tmsgstring, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13 });
            }

        [Torque_Decorations.TorqueCallBack("", "", "messageAll", "", 15, 11000, false)]
        public void MessageAll(string msgtype, string msgstring, string a1 = "", string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "", string a9 = "", string a10 = "", string a11 = "", string a12 = "", string a13 = "")
            {
            foreach (uint clientid in ClientGroup.Where(clientid => clientid != 0))
                {
                MessageClient(clientid.AsString(), msgtype, msgstring, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
                }
            }
        }
    }