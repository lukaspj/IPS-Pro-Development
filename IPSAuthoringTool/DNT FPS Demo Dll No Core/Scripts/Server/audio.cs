using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using System;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "ServerPlay2D", "(profile)", 1, 10000, false)]
        public void AudioServerPlay2D(string profile)
            {
            foreach (uint clientid in ClientGroup)
                {
                GameConnection.play2D(clientid.AsString(), profile);
                }
            }


        [Torque_Decorations.TorqueCallBack("", "", "ServerPlay3D", "(profile,transform)", 2, 10000, false)]
        public void AudioServerPlay3D(string profile, string transform)
            {
            foreach (uint clientid in ClientGroup)
                {
                GameConnection.play3D(clientid.AsString(), profile, new TransformF(transform));
                }
            }
        }
    }
