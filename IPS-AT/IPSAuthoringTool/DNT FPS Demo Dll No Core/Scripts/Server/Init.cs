using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
using WinterLeaf;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
//-----------------------------------------------------------------------------

// Variables used by server scripts & code.  The ones marked with (c)
// are accessed from code.  Variables preceeded by Pref:: are server
// preferences and stored automatically in the ServerPrefs.cs file
// in between server sessions.
//
//    (c) Server::ServerType              {SinglePlayer, MultiPlayer}
//    (c) Server::GameType                Unique game name
//    (c) Server::Dedicated               Bool
//    ( ) Server::MissionFile             Mission .mis file name
//    (c) Server::MissionName             DisplayName from .mis file
//    (c) Server::MissionType             Not used
//    (c) Server::PlayerCount             Current player count
//    (c) Server::GuidList                Player GUID (record list?)
//    (c) Server::Status                  Current server status
//
//    (c) Pref::Server::Name              Server Name
//    (c) Pref::Server::Password          Password for client connections
//    ( ) Pref::Server::AdminPassword     Password for client admins
//    (c) Pref::Server::Info              Server description
//    (c) Pref::Server::MaxPlayers        Max allowed players
//    (c) Pref::Server::RegionMask        Registers this mask with master server
//    ( ) Pref::Server::BanTime           Duration of a player ban
//    ( ) Pref::Server::KickBanTime       Duration of a player kick & ban
//    ( ) Pref::Server::MaxChatLen        Max chat message len
//    ( ) Pref::Server::FloodProtectionEnabled Bool

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "initServer", "",  0, 80,false)]
        public void InitServer()
            {
            console.print("\n----------------Initializing " + console.GetVarString("$appName") + ": Server Scripts -------------------");
            // Server::Status is returned in the Game Info Query and represents the
            // current status of the server. This string sould be very short.
            console.SetVar("$Server::Status", "Unknown");
            // Turn on testing/debug script functions
            console.SetVar("$Server::TestCheats","false");
            // Specify where the mission files are.
            console.SetVar("$Server::MissionFileSpec", "levels/*.mis");
            // The common module provides the basic server functionality

            InitBaseServer();
            
            
            dnt.DoScriptInjection(ScriptType.Server, 100, 100);//Load the command.cs file.
            dnt.DoScriptInjection(ScriptType.Server, 50, 51);//Load the game.cs file.
            }
        [Torque_Decorations.TorqueCallBack("", "", "initDedicated", "",  0, 80,false)]
        public void InitDedicated()
            {
            console.Call("enableWinConsole", new string[] {"true"});
            //con.Eval("enableWinConsole(true);");
            console.print(@"\n--------- Starting Dedicated Server ---------");
            // Make sure this variable reflects the correct state.
            console.SetVar("$Server::Dedicated","true");
            // The server isn't started unless a mission has been specified.
            if (console.GetVarString("$missionArg") != "")
                CreateServer("MultiPlayer", console.GetVarString("$missionArg"));
                
            else
                console.print("No mission specified (use -mission filename)");
            }

        }
    }
