﻿using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
using WinterLeaf;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {

        public int Server__PlayerCount = 0;

        public int pref__Server__MaxPlayers = 64;

        public string Server__LoadFailMsg = string.Empty;

        [Torque_Decorations.TorqueCallBack("", "", "sendLoadInfoToClient", "(%client)", 1, 15000, false)]
        private void SendLoadInfoToClient(string client)
            {
            MessageClient(client, "MsgLoadInfo", "", console.GetVarString("theLevelInfo.levelName"));

            for (int i = 0; console.GetVarString(string.Format("theLevelInfo.desc[{0}]", i)) != ""; i++)
                
                MessageClient(client, "MsgLoadDescripition", "", console.GetVarString(string.Format("theLevelInfo.desc[{0}]", i)));
            
            MessageClient(client, "MsgLoadInfoDone","");
            }


        //-----------------------------------------------------------------------------
        // This script function is called before a client connection
        // is accepted.  Returning "" will accept the connection,
        // anything else will be sent back as an error to the client.
        // All the connect args are passed also to onConnectRequest
        //
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnectRequest", "( %client, %netAddress, %name )", 3, 15000, false)]
        public string GameConnectionOnConnectRequest(string client, string netAddress, string name)
            {
            console.print(string.Format("Connect request from:{0}", netAddress));
            return Server__PlayerCount >= pref__Server__MaxPlayers ? "CR_SERVERFULL" : "";
            }

        //-----------------------------------------------------------------------------
        // This script function is the first called on a client accept
        //
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnect", "( %client, %name )", 2, 15000, false)]
        public void GameConnectionOnConnect(string client, string name)
            {
            // Send down the connection error info, the client is
            // responsible for displaying this message if a connection
            // error occures.
            MessageClient(client, "MsgConnectionError", "", console.GetVarString("$Pref::Server::ConnectionError"));
            // Send mission information to the client
            SendLoadInfoToClient(client);
            // Simulated client lag for testing...
            // %client.setSimulatedNetParams(0.1, 30);

            // Get the client's unique id:
            // %authInfo = %client.getAuthInfo();
            // %client.guid = getField( %authInfo, 3 );
            console.SetVar(client + ".guid", 0);
            AddToServerGuidList(console.GetVarString(client + ".guid"));
            // Set admin status

            if (NetConnection.getAddress(client) == "local")
                {
                console.SetVar(string.Format("{0}.isAdmin", client), true);
                console.SetVar(string.Format("{0}.isSuperAdmin", client), true);
                }
            else
                {
                console.SetVar(string.Format("{0}.isAdmin", client), false);
                console.SetVar(string.Format("{0}.isSuperAdmin", client), false);
                }
            // Save client preferences on the connection object for later use.
            console.SetVar(string.Format("{0}.gender", client), "Male");
            console.SetVar(string.Format("{0}.armor", client), "Light");
            console.SetVar(string.Format("{0}.race", client), "human");
            console.SetVar(string.Format("{0}.skin", client), console.addTaggedString("base"));
            
            GameConnectionSetPlayerName(client,name);
        
            console.SetVar(string.Format("{0}.team", client), "");
            console.SetVar(string.Format("{0}.score", client), 0);

            console.print("CADD: " + client + " " + NetConnection.getAddress(client));


            // If the mission is running, go ahead download it to the client
            if (missionRunning)
                GameConnectionLoadMission(client);
            else if (Server__LoadFailMsg != "")
                MessageClient(client, "MsgLoadFailed", Server__LoadFailMsg);

            Server__PlayerCount++;
            }

        //-----------------------------------------------------------------------------
        // A player's name could be obtained from the auth server, but for
        // now we use the one passed from the client.
        // %realName = getField( %authInfo, 0 );
        //

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "setPlayerName", "( %client, %name )", 2, 15000, false)]
        public void GameConnectionSetPlayerName(string client, string name)
            {
            console.SetVar(string.Format("{0}.sendGuid", client), 0);
            // Minimum length requirements
            name = Util._strToPlayerName(name).Trim();
            if (name.Length < 3)
                name = "Poser";

            // Make sure the alias is unique, we'll hit something eventually
            if (!IsNameUnique(name))
                {
                bool isUnique = false;
                string nametry = "NotSet";
                for (int suffix = 1; !isUnique; suffix++)
                    {
                    nametry = name + "." + suffix;
                    isUnique = IsNameUnique(nametry);
                    }
                name = nametry;
                }
            console.SetVar(string.Format("{0}.nameBase", client), name);
            console.SetVar(string.Format("{0}.playerName", client), console.addTaggedString(console.ColorEncode(string.Format(@"\cp\c8{0}\co", name))));
            }

        [Torque_Decorations.TorqueCallBack("", "", "isNameUnique", "(  %name )", 1, 15000, false)]
        public bool IsNameUnique(string name)
            {
            foreach (uint client in ClientGroup)
                {
                string rawName = "";
                if (console.GetVarString(string.Format("{0}.playerName", client)).Trim().Length > 0)
                    rawName = Util.stripChars(Util.detag(console.getTaggedString(console.GetVarString(string.Format("{0}.playerName", client)))), console.ColorEncode(@"\cp\co\c6\c8\c9"));
                if (name == rawName)
                    return false;
                }
            return true;
            }

        //-----------------------------------------------------------------------------
        // This function is called when a client drops for any reason
        //

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onDrop", "( %client, %reason )", 2, 15000, false)]
        public void GameConnectiononDrop(string client, string reason)
            {
            GameConnectionOnClientLeaveGame(client);

            RemoveFromServerGuidList(console.GetVarString(string.Format("{0}.guid", client)));

            MessageAllExcept(client,
                         "-1",
                         "MsgClientDrop",
                         console.ColorEncode(@"\c1%1 has left the game."),
                         console.GetVarString(string.Format("{0}.playerName", client)),
                         client);

            

            console.removeTaggedString(console.GetVarString(string.Format("{0}.playerName", client)));

            console.print(string.Format("CDROP: {0} {1}", client, NetConnection.getAddress(client)));

            
            Server__PlayerCount--;
            
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "startMission", "( %this )", 1, 15000, false)]
        public void GameConnectionstartMission(string client)
            {
            // Inform the client the mission starting
            console.commandToClient(client, "MissionStart", new string[] { console.GetVarString("$missionSequence") });
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "endMission", "( %this )", 1, 15000, false)]
        public void GameConnectionendMission(string client)
            {
            // Inform the client the mission is done.  Note that if this is
            // called as part of the server destruction routine, the client will
            // actually never see this comment since the client connection will
            // be destroyed before another round of command processing occurs.
            // In this case, the client will only see the disconnect from the server
            // and should manually trigger a mission cleanup.
            console.commandToClient(client, "MissionEnd", new string[] { console.GetVarString("$missionSequence") });
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "syncClock", "( %%client, %time )", 2, 15000, false)]
        public void GameConnectionsyncClock(string client, string time)
            {
            console.commandToClient(client, "syncClock", new string[] { time });
            }

        //--------------------------------------------------------------------------
        // Update all the clients with the new score

        //[Torque_Decorations.TorqueCallBack("", "GameConnection", "incScore", "(%this,%delta)", 2, 15000, false)]
        //public void GameConnectionincScore(string client, string delta)
        //    {
        //    console.SetVar(client + ".score", console.GetVarInt(client + ".score") + delta);

        //    MessageAll("MsgClientScoreChanged", "", console.GetVarString(client + ".score"), client);
        //    }
        }
    }
