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
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "server_defaults_init", "", 0, 90, false)]
        public void server_defaults_init()
            {
            // List of master servers to query, each one is tried in order
            // until one responds
            console.SetVar("$Pref::Server::RegionMask", 2);
            console.SetVar("$pref::Master[0]", "2:master.garagegames.com:28002");

            // Information about the server
            console.SetVar("$Pref::Server::Name", "Torque 3D Vince Server");
            console.SetVar("$Pref::Server::Info", "Vince's Server.");

            // The connection error message is transmitted to the client immediatly
            // on connection, if any further error occures during the connection
            // process, such as network traffic mismatch, or missing files, this error
            // message is display. This message should be replaced with information
            // usefull to the client, such as the url or ftp address of where the
            // latest version of the game can be obtained.
            console.SetVar("$Pref::Server::ConnectionError", "You do not have the correct version of the FPS starter kit or " +
            "the related art needed to play on this server, please contact " +
            "the server operator for more information.")
            ;

            // The network port is also defined by the client, this value 
            // overrides pref::net::port for dedicated servers
            console.SetVar("$Pref::Server::Port", 28003);

            // If the password is set, clients must provide it in order
            // to connect to the server
            console.SetVar("$Pref::Server::Password", "");

            // Password for admin clients
            console.SetVar("$Pref::Server::AdminPassword", "");

            //Not needed anymore.
            // Misc server settings.
            //console.SetVar("$Pref::Server::MaxPlayers", 64);



            console.SetVar("$Pref::Server::TimeLimit", 20); // In minutes
            console.SetVar("$Pref::Server::KickBanTime", 300); // specified in seconds
            console.SetVar("$Pref::Server::BanTime", 1800); // specified in seconds
            console.SetVar("$Pref::Server::FloodProtectionEnabled", 1);
            console.SetVar("$Pref::Server::MaxChatLen", 120);

            }
        [Torque_Decorations.TorqueCallBack("", "", "defaultcs_init", "", 0, 90, true)]
        public void DefaultcsInit()
            {
            //-----------------------------------------------------------------------------
            // Torque
            // Copyright GarageGames, LLC 2011
            //-----------------------------------------------------------------------------

            // First we execute the core default preferences.
            //dnt.DoScriptInjection(ScriptType.Core_Server, 13000, 13000);

            server_defaults_init();


            //Init_Server_Defaults

            // Now add your own game specific server preferences as
            // well as any overloaded core defaults here.

            // Finally load the preferences saved from the last
            // game execution if they exist.
            if (Util.isFile("./scripts/server/prefs.cs"))
                Util.exec("./scripts/server/prefs.cs", false, false);

            console.SetVar("$pref::Net::PacketRateToClient", 32);
            console.SetVar("$pref::Net::PacketSize", 200);
            }
        }
    }
