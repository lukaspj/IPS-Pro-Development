using System;
using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "loadMission", "(%this)", 1, 14000, false)]
        public void GameConnectionLoadMission(string client)
            {
            if (!console.isObject(client))
                return;

            // Send over the information that will display the server info
            // when we learn it got there, we'll send the data blocks

            console.SetVar(string.Format("{0}.currentPhase", client), 0);

            if (GameConnection.isAIControlled(client))
                {
                GameConnectionOnClientEnterGame(client);
                }
            else
                {

                console.commandToClient(client, "MissionStartPhase1",
                                    new[]
                                        {
                                            console.GetVarString("$missionSequence"), console.GetVarString("$Server::MissionFile"),
                                            console.GetVarString("MissionGroup.musicTrack")
                                        });
                console.print(string.Format("*** Sending mission load to client: {0}", console.GetVarString("$Server::MissionFile")));
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "serverCmdMissionStartPhase1Ack", "(%client, %seq)", 2, 14000, false)]
        public void ServerCmdMissionStartPhase1Ack(string client, string seq)
            {
            // Make sure to ignore calls from a previous mission load
            if (seq != console.GetVarString("$missionSequence") || !missionRunning)
                return;
            if (console.GetVarDouble(string.Format("{0}.currentPhase", client)) != 0.0)
                return;

            console.SetVar(string.Format("{0}.currentPhase", client), 1);
            // Start with the CRC

            GameConnection.setMissionCRC(client, console.GetVarInt("$missionCRC"));

            // Send over the datablocks...
            // OnDataBlocksDone will get called when have confirmation
            // that they've all been received.
            console.print("Transmitting Datablocks");
            GameConnection.transmitDataBlocks(client, console.GetVarInt("$missionSequence"));
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onDataBlocksDone", "( %this, %missionSequence)", 2, 14000, false)]
        public void GameConnectiononDataBlocksDone(string thisobj, string missionSequence)
            {
            // Make sure to ignore calls from a previous mission load
            if (missionSequence != console.GetVarString("$missionSequence"))
                return;
            if (console.GetVarInt(string.Format("{0}.currentPhase", thisobj)) != 1)
                return;
            console.SetVar(string.Format("{0}.currentPhase", thisobj), 1.5);
            // On to the next phase
            console.commandToClient(thisobj, "MissionStartPhase2", new[] { console.GetVarString("$missionSequence"), console.GetVarString("$Server::MissionFile") });
            }

        [Torque_Decorations.TorqueCallBack("", "", "serverCmdMissionStartPhase2Ack", "(%client, %seq, %playerDB)", 3, 14000, false)]
        public void ServerCmdMissionStartPhase2Ack(string client, string seq, string playerDB)
            {
            // Make sure to ignore calls from a previous mission load
            if (seq != console.GetVarString("$missionSequence") || !missionRunning)
                return;
            if (console.GetVarDouble(string.Format("{0}.currentPhase", client)) != 1.5)
                return;

            console.SetVar(string.Format("{0}.currentPhase", client), 2);
            // Set the player datablock choice

            console.SetVar(string.Format("{0}.playerDB", client), playerDB);
            // Update mod paths, this needs to get there before the objects.

            NetConnection.transmitPaths(client);

            // Start ghosting objects to the client
            GameConnection.activateGhosting(client);
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "clientWantsGhostAlwaysRetry", "(%client)", 1, 14000, false)]
        public void ClientWantsGhostAlwaysRetry(string client)
            {
            if (missionRunning)
                GameConnection.activateGhosting(client);
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onGhostAlwaysFailed", "(%client)", 1, 14000, false)]
        public void OnGhostAlwaysFailed(string client)
            {
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onGhostAlwaysObjectsReceived", "(%client)", 1, 14000, false)]
        public void OnGhostAlwaysObjectsReceived(string client)
            {
            // Ready for next phase.
            console.commandToClient(client, "MissionStartPhase3", new[] { console.GetVarString("$missionSequence"), console.GetVarString("$Server::MissionFile") });
            }

        [Torque_Decorations.TorqueCallBack("", "", "serverCmdMissionStartPhase3Ack", "(%client, %seq)", 2, 14000, false)]
        public void ServerCmdMissionStartPhase3Ack(string client, string seq)
            {
            if (seq != console.GetVarString("$missionSequence") || !missionRunning)
                return;
            if (console.GetVarDouble(string.Format("{0}.currentPhase", client)) != 2.0)
                return;

            console.SetVar(string.Format("{0}.currentPhase", client), 3);
            // Server is ready to drop into the game


            GameConnectionstartMission(client);
            GameConnectionOnClientEnterGame(client);

            }
        }
    }
