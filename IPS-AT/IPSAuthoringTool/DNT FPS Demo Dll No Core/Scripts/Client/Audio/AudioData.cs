using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {

    public partial class Main : TorqueScriptTemplate
        {
        // Game specific audio descriptions. Always declare SFXDescription's (the type of sound)
        // before SFXProfile's (the sound itself) when creating new ones
         [Torque_Decorations.TorqueCallBack("", "", "init_AudioData", "()",  0, 12000, true)]
        public void InitAudioData()
             {
             TorqueSingleton ts = new TorqueSingleton("SFXDescription", "BulletFireDesc : AudioEffect");
             ts.Props.Add("isLooping","false");
             ts.Props.Add("is3D","true");
             ts.Props.Add("ReferenceDistance","10.0");
             ts.Props.Add("MaxDistance","60.0");
             ts.Create(m_ts);

             ts = new TorqueSingleton("SFXDescription", "BulletImpactDesc : AudioEffect");
             ts.Props.Add("isLooping", "false");
             ts.Props.Add("is3D", "true");
             ts.Props.Add("ReferenceDistance", "10.0");
             ts.Props.Add("MaxDistance", "30.0");
             ts.Create(m_ts);
             }


        }
    }

