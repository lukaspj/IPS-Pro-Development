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
        [Torque_Decorations.TorqueCallBack("", "", "Initialize_AudioDescriptions", "", 0, 27000, true)]
        public void Initialize_AudioDescriptions()
            {
            TorqueSingleton ts = new TorqueSingleton("SFXDescription", "AudioDefault3D : AudioEffect");

            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("ReferenceDistance ", " 20.0");
            ts.Props.Add("MaxDistance ", " 100.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", " AudioSoft3D : AudioEffect ");

            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("ReferenceDistance ", " 20.0");
            ts.Props.Add("MaxDistance ", " 100.0");
            ts.Props.Add("volume ", " 0.4");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", " AudioClose3D : AudioEffect ");

            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("ReferenceDistance ", " 10.0");
            ts.Props.Add("MaxDistance ", " 60.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", " AudioClosest3D : AudioEffect ");

            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("ReferenceDistance ", " 5.0");
            ts.Props.Add("MaxDistance ", " 10.0");
            ts.Create(m_ts);

            //-----------------------------------------------------------------------------
            // Looping sounds
            //-----------------------------------------------------------------------------

            ts = new TorqueSingleton("SFXDescription", " AudioDefaultLoop3D : AudioEffect ");

            ts.Props.Add("isLooping", " true");
            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("ReferenceDistance ", " 20.0");
            ts.Props.Add("MaxDistance ", " 100.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", " AudioCloseLoop3D : AudioEffect ");

            ts.Props.Add("isLooping", " true");
            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("ReferenceDistance ", " 18.0");
            ts.Props.Add("MaxDistance ", " 25.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", " AudioClosestLoop3D : AudioEffect ");

            ts.Props.Add("isLooping", " true");
            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("ReferenceDistance ", " 5.0");
            ts.Props.Add("MaxDistance ", " 10.0");
            ts.Create(m_ts);

            //-----------------------------------------------------------------------------
            // 2d sounds
            //-----------------------------------------------------------------------------

            // Used for non-looping environmental sounds ,"like power on, power off")");
            ts = new TorqueSingleton("SFXDescription", " Audio2D : AudioEffect ");

            ts.Props.Add("isLooping", " false");


            // Used for Looping Environmental Sounds
            ts = new TorqueSingleton("SFXDescription", " AudioLoop2D : AudioEffect ");

            ts.Props.Add("isLooping", " true");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", " AudioStream2D : AudioEffect ");

            ts.Props.Add("isStreaming ", " true");
            ts.Create(m_ts);
            ts = new TorqueSingleton("SFXDescription", " AudioStreamLoop2D : AudioEffect ");

            ts.Props.Add("isLooping", " true");
            ts.Props.Add("isStreaming ", " true");


            //-----------------------------------------------------------------------------
            // Music
            //-----------------------------------------------------------------------------
            ts.Create(m_ts);
            ts = new TorqueSingleton("SFXDescription", " AudioMusic2D : AudioMusic ");

            ts.Props.Add("isStreaming ", " true");

            ts.Create(m_ts);
            ts = new TorqueSingleton("SFXDescription", " AudioMusicLoop2D : AudioMusic ");

            ts.Props.Add("isLooping", " true");
            ts.Props.Add("isStreaming ", " true");

            ts.Create(m_ts);
            ts = new TorqueSingleton("SFXDescription", " AudioMusic3D : AudioMusic ");

            ts.Props.Add("isStreaming ", " true");
            ts.Props.Add("is3D  ", " true");

            ts.Create(m_ts);
            ts = new TorqueSingleton("SFXDescription", " AudioMusicLoop3D : AudioMusic ");

            ts.Props.Add("isStreaming ", " true");
            ts.Props.Add("is3D  ", " true");
            ts.Props.Add("isLooping", " true");

            }
        }
    }
