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
        [Torque_Decorations.TorqueCallBack("", "", "Initialize_AudioAmbiences", "", 0, 29000, true)]
        public void Initialize_AudioAmbiences()
            {
            TorqueSingleton ts = new TorqueSingleton("SFXAmbience", "AudioAmbienceDefault");
            ts.Props.Add("environment", " AudioEnvOff");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXAmbience", "AudioAmbienceOutside");
            ts.Props.Add("environment", "AudioEnvPlain");
            ts.Props.Add("states[ 0 ]", "AudioLocationOutside");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXAmbience", "AudioAmbienceInside");
            ts.Props.Add("environment", "AudioEnvRoom");
            ts.Props.Add("states[ 0 ]", "AudioLocationInside");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXAmbience", "AudioAmbienceUnderwater");
            ts.Props.Add("environment", "AudioEnvUnderwater");
            ts.Props.Add("states[ 0 ]", "AudioLocationUnderwater");
            ts.Create(m_ts);
            }
        }
    }
