using System;
using System.Threading;
using WinterLeaf;
using WinterLeaf.Classes;
using System.ComponentModel;
using System.Collections.Concurrent;


namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {


        /*
         * 
         * Function Weights per scriptexec
         * 50       Game
         * 80       Init
         * 90        Defaults
         * 100      Commands
         * 1000     Camera
         * 1100     triggers
         * 1200     inventory
         * 1300     shapeBase
         * 1400     item
         * 1500     health
         * 1600     projectile
         * 1700     radiusDamage
         * 1800     teleporter
         * 1900     weapon
         * 2000     rocketLauncher
         * 2100     soldierGun
         * 2200     grenadeLauncher
         * 2300     proximityMine
         * 2400     player
         * 2500     aiPlayer
         * 2600     vehicle
         * 2700     vehicleWheeled
         * 2800     cheetah
         * 2900     turret
         * 3000     gameCore
         * 3100     gameDM
         * 
         * 10000    audio.cs
         * 11000    message.cs
         * 12000    commands.cs
         * 12200    levelInfo.cs
         * 13000    MissionLoad
         * 14000    missionDownload
         * 
         * clientConnection     15000    
         * kickban              16000
         * game                 17000
         * spawn                18000
         * camera               19000
         * centerprint          20000
         * * 
         * 
         * 
         * 
         */

        private dnTorque dnt;

        public Main(ref dnTorque c)
            : base(ref c)
            {
            dnt = c;

            
            }

        public void InitGlobals()
            {
            console.SetVar("$Camera::movementSpeed", "40");
            console.SetVar("$WeaponSlot", "0");
            }

        
        public void Load_Defaults()
            {
            dnt.DoScriptInjection(ScriptType.Server, 90, 90);
            }
        public void Load_ServerInit()
            {
            //Calls the Init.cs file.
            dnt.DoScriptInjection(ScriptType.Server, 80, 80);
            }

        
        }
    }