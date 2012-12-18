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

        /// Return the first active SFXState in the given SimSet/SimGroup.
        [Torque_Decorations.TorqueCallBack("", "", "sfxGetActiveStateInGroup", "", 1, 28000, false)]
        public string sfxGetActiveStateInGroup(string group)
            {
            for (uint i = 0; i < SimSet.getCount(group); i++)
                {
                string obj = SimSet.getObject(group, i);
                if (Util.isMemberOfClass(obj, "SFXState"))
                    continue;

                if (GuiControl.isActive(obj))
                    return obj;

                }
            return "0";
            }
        [Torque_Decorations.TorqueCallBack("", "", "initialize_AudioStates", "", 0, 28000, true)]
        public void initialize_AudioStates()
            {
            TorqueSingleton ts = new TorqueSingleton("SFXState", "AudioStateNone");
            ts.Create(m_ts);
            
            console.Call("AudioStateNone", "activate");
            }

        [Torque_Decorations.TorqueCallBack("", "SFXState", "onActivate", "this", 1, 28000, false)]
        public void SFXState_onActivate(string thisobj)
            {
            if (console.GetObjectID(thisobj) != console.GetObjectID("AudioStateNone"))
                console.Call("AudioStateNone", "disable");
            }
        [Torque_Decorations.TorqueCallBack("", "SFXState", "onDeactivate", "this", 1, 28000, false)]
        public void onDeactivate(string thisobj)
            {
            if (console.GetObjectID(thisobj) != console.GetObjectID("AudioStateNone"))

                console.Call("AudioStateNone", "enable");
            }
        //-----------------------------------------------------------------------------
        // AudioStateExclusive class.
        //
        // Automatically deactivates sibling SFXStates in its parent SimGroup
        // when activated.

        [Torque_Decorations.TorqueCallBack("", "AudioStateExclusive", "onActivate", "this", 1, 28000, false)]
        public void AudioStateExclusiveonActivate(string thisobj)
            {
            
            console.ParentExecute(thisobj, "onActivate", 1, new string[] { thisobj });


            string group = console.GetVarString(thisobj + ".parentGroup");
            for (uint i = 0; i < SimSet.getCount(group); i++)
                {
                string obj = SimSet.getObject(group, i);
                if (obj != thisobj && SimObject.SimObject_isMemberOfClass(obj, "SFXState") && GuiControl.isActive(obj))// console.Call(obj, "isActive").AsBool())
                    console.Call(obj, "deactivate");
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "initialize_AudioStates1", "", 0, 28000, true)]
        public void initialize_AudioState1s()
            {
            TorqueSingleton ts = new TorqueSingleton("SimGroup", "AudioLocation");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXState", "AudioLocationOutside");
            ts.Props.Add("parentGroup", "AudioLocation");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXState", "AudioLocationUnderwater");
            ts.Props.Add("parentGroup", "AudioLocation");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXState", "AudioLocationInside");
            ts.Props.Add("parentGroup", "AudioLocation");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);


            }
        [Torque_Decorations.TorqueCallBack("", "", "sfxGetLocation", "this", 0, 28000, false)]
        public string sfxGetLocation()
            {
            return console.Call("sfxGetActiveStateInGroup", new string[] { "AudioLocation" });
            }

        [Torque_Decorations.TorqueCallBack("", "", "initialize_AudioStates2", "", 0, 28000, true)]
        public void initialize_AudioState2s()
            {
            TorqueSingleton ts = new TorqueSingleton("SimGroup", "AudioMood");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXState", "AudioMoodNeutral");
            ts.Props.Add("parentGroup", "AudioMood");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXState", "AudioMoodAggressive");
            ts.Props.Add("parentGroup", "AudioMood");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);


            ts = new TorqueSingleton("SFXState", "AudioMoodTense");
            ts.Props.Add("parentGroup", "AudioMood");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXState", "AudioMoodVictory");
            ts.Props.Add("parentGroup", "AudioMood");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXState", "AudioMoodCalm");
            ts.Props.Add("parentGroup", "AudioMood");
            ts.PropsAddString("className", "AudioStateExclusive");
            ts.Create(m_ts);


            }
        [Torque_Decorations.TorqueCallBack("", "", "sfxGetMood", "this", 0, 28000, false)]
        public string sfxGetMood()
            {
           return sfxGetActiveStateInGroup("AudioMood");
            }
        }
    }
