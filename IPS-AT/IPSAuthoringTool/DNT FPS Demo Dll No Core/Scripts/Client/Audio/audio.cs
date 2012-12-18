﻿using System;
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
        //-----------------------------------------------------------------------------
        //    Source groups.
        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "Initialize_Audio", "", 0, 21000, true)]
        public void Initialize_Audio()
            {
            new TorqueSingleton("SFXDescription", "AudioMaster").Create(m_ts);
            TorqueSingleton ts = new TorqueSingleton("SFXSource", "AudioChannelMaster");
            ts.Props.Add("description", "AudioMaster");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", "AudioChannel");
            ts.Props.Add("sourceGroup", "AudioChannelMaster");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXSource", "AudioChannelDefault");
            ts.Props.Add("description", "AudioChannel");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXSource", "AudioChannelGui");
            ts.Props.Add("description", "AudioChannel");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXSource", "AudioChannelEffects");
            ts.Props.Add("description", "AudioChannel");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXSource", "AudioChannelMessages");
            ts.Props.Add("description", "AudioChannel");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXSource", "AudioChannelMusic");
            ts.Props.Add("description", "AudioChannel");
            ts.Create(m_ts);

            SFXSource.play("AudioChannelMaster", -1);
            SFXSource.play("AudioChannelDefault", -1);
            SFXSource.play("AudioChannelGui", -1);
            SFXSource.play("AudioChannelMusic", -1);
            SFXSource.play("AudioChannelMessages", -1);

            SFXSource.stop("AudioChannelEffects", -1);
            //-----------------------------------------------------------------------------
            //    Master SFXDescriptions.
            //-----------------------------------------------------------------------------

            // Master description for interface audio.
            ts = new TorqueSingleton("SFXDescription", "AudioGui");
            ts.Props.Add("volume", "1.0");
            ts.Props.Add("sourceGroup", "AudioChannelGui");
            ts.Create(m_ts);


            ts = new TorqueSingleton("SFXDescription", "AudioEffect");
            ts.Props.Add("volume", "1.0");
            ts.Props.Add("sourceGroup", "AudioChannelEffects");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", "AudioMessage");
            ts.Props.Add("volume", "1.0");
            ts.Props.Add("sourceGroup", "AudioChannelMessages");
            ts.Create(m_ts);

            ts = new TorqueSingleton("SFXDescription", "AudioMusic");
            ts.Props.Add("volume", "1.0");
            ts.Props.Add("sourceGroup", "AudioChannelMusic");
            ts.Create(m_ts);

            console.SetVar("$GuiAudioType", 1);
            console.SetVar("$SimAudioType", 2);
            console.SetVar("$MessageAudioType", 3);
            console.SetVar("$MusicAudioType", 4);


            console.SetVar("$AudioChannels[0]", "AudioChannelDefault");
            console.SetVar("$AudioChannels[1]", "AudioChannelGui");
            console.SetVar("$AudioChannels[2]", "AudioChannelEffects");
            console.SetVar("$AudioChannels[3]", "AudioChannelMessages");
            console.SetVar("$AudioChannels[4]", "AudioChannelMusic");
            new TorqueSingleton("SimSet", "SFXPausedSet").Create(m_ts);
            }

        //-----------------------------------------------------------------------------
        //    SFX Functions.
        //-----------------------------------------------------------------------------

        /// This initializes the sound system device from
        /// the defaults in the $pref::SFX:: globals.
        [Torque_Decorations.TorqueCallBack("", "", "sfxStartup", "", 0, 21000, false)]
        public void sfxStartup()
            {
            // The console builds should re-detect, by default, so that it plays nicely 
            // along side a PC build in the same script directory.
            //if (console.GetVarString("$platform") == "xenon")
            //    {
            //    if (console.GetVarString("$pref::SFX::provider") == "DirectSound" || console.GetVarString("$pref::SFX::provider") == "OpenAL")
            //        console.SetVar("$pref::SFX::provider", "");
            //    if (console.GetVarString("$pref::SFX::provider") == "")
            //        {
            //        console.SetVar("$pref::SFX::autoDetect", 1);
            //        console.warn("Xbox360 is auto-detecting available sound providers...");
            //        console.warn("   - You may wish to alter this functionality before release (core/scripts/client/audio.cs)");
            //        }
            //    }
            console.print("sfxStartup...");
            // If we have a provider set, try initialize a device now.

            if (console.GetVarString("$pref::SFX::provider") == "")
                {
                if (sfxInit())
                    return;
                else
                    {
                    // Force auto-detection.
                    console.SetVar("$pref::SFX::autoDetect", true);
                    if (sfxAutodetect())
                        return;
                    }
                }

           
            // Failure.
            console.error("   Failed to initialize device!\n\n");
            console.SetVar("$pref::SFX::provider", "");
            console.SetVar("$pref::SFX::device", "");
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxInit", "", 0, 21000, false)]
        public bool sfxInit()
            {

            // This initializes the sound system device from
            // the defaults in the $pref::SFX:: globals.
            // If already initialized, shut down the current device first.
            if (Util.sfxGetDeviceInfo() != "")
                sfxShutdown();
            // Start it up!

            int maxBuffers = console.GetVarBool("$pref::SFX::useHardware") ? -1 : console.GetVarInt("$pref::SFX::maxSoftwareBuffers");
            if (!Util.sfxCreateDevice(console.GetVarString("$pref::SFX::provider"), console.GetVarString("$pref::SFX::device"), console.GetVarBool("$pref::SFX::useHardware"),
                                  maxBuffers))
                return false;

            // This returns a tab seperated string with
            // the initialized system info.
            string info = Util.sfxGetDeviceInfo();
            console.SetVar("$pref::SFX::provider", Util.getField(info, 0));
            console.SetVar("$pref::SFX::device", Util.getField(info, 1));
            console.SetVar("$pref::SFX::useHardware", Util.getField(info, 2));
            string useHardware = console.GetVarBool("$pref::SFX::useHardware") ? "Yes" : "No";
            maxBuffers = Util.getField(info, 3).AsInt();

            console.print("   Provider: " + console.GetVarString("$pref::SFX::provider"));
            console.print("   Device: " + console.GetVarString("$pref::SFX::device"));
            console.print("   Hardware: " + useHardware);
            console.print("   Buffers: " + maxBuffers.AsString());


            if (Util._isDefined("$pref::SFX::distanceModel"))
                {
                SFXDistanceModel t;
                if (SFXDistanceModel.TryParse(console.GetVarString("$pref::SFX::distanceModel"), out t))
                    {
                    Util.sfxSetDistanceModel(t);
                    }
                }
            if (Util._isDefined("$pref::SFX::dopplerFactor"))
                {
                Util.sfxSetDopplerFactor(console.GetVarFloat("$pref::SFX::dopplerFactor"));
                }
            if (Util._isDefined("$pref::SFX::rolloffFactor"))
                {
                Util.sfxSetRolloffFactor(console.GetVarFloat("$pref::SFX::rolloffFactor"));
                }
            // Restore master volume.

            sfxSetMasterVolume(console.GetVarString("$pref::SFX::masterVolume"));

            // Restore channel volumes.
            for (int channel = 0; channel <= 8; channel++)
                sfxSetChannelVolume(channel.AsString(), console.GetVarFloat("$pref::SFX::channelVolume[" + channel.AsString() + "]"));
            return true;
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxShutdown", "", 0, 21000, false)]
        public void sfxShutdown()
            {
            console.SetVar("$pref::SFX::masterVolume", console.Call("sfxGetMasterVolume"));
            for (int channel = 0; channel <= 8; channel++)
                console.SetVar("$pref::SFX::channelVolume[" + channel.AsString() + "]", console.Call("sfxGetChannelVolume", new string[] { channel.AsString() }));
            // We're assuming here that a null info 
            // string means that no device is loaded.
            if (Util.sfxGetDeviceInfo() == "")
                return;
            Util.sfxDeleteDevice();
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxCompareProvider", "(%providerA, %providerB)", 2, 21000, false)]
        public string sfxCompareProvider(string providerA, string providerB)
            {
            if (providerA == providerB)
                return "0";
            switch (providerA)
                {
                // Always prefer FMOD over anything else.
                case "FMOD":
                    return "1";
                // Prefer OpenAL over anything but FMOD.
                case "OpenAL":
                    return providerB == "FMOD" ? "-1" : "1";
                // As long as the XAudio SFX provider still has issues,
                // choose stable DSound over it.
                case "DirectSound":
                    return providerB == "FMOD" || providerB == "OpenAL" ? "-1" : "0";
                case "XAudio":
                    return providerB == "FMOD" && providerB != "OpenAL" && providerB != "DirectSound" ? "1" : "-1";
                default:
                    return "-1";
                }
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxAutodetect", "()", 0, 21000, false)]
        public bool sfxAutodetect()
            {
            // Get all the available devices.
            string devices = Util.sfxGetAvailableDevices();
            // Collect and sort the devices by preferentiality.

            int bestmatch = -1;
            int count = Util.getRecordCount(devices);


            string deviceTrySequence = new Torque_Class_Helper("ArrayObject").Create(m_ts).AsString();

            for (int i = 0; i < count; i++)
                {
                string info = Util.getRecord(devices, i);
                string provider = Util.getField(info, 0);
                ArrayObject.push_back(deviceTrySequence,provider, info );
                }
            ArrayObject.sortfk(deviceTrySequence, "sfxCompareProvider");
            // Try the devices in order.

            count = ArrayObject.count(deviceTrySequence);
            for (int i = 0; i < count; i++)
                {
                string provider = ArrayObject.getKey(deviceTrySequence, i);
                string info = ArrayObject.getValue(deviceTrySequence, i);

                console.SetVar("$pref::SFX::provider", provider);
                console.SetVar("$pref::SFX::device", Util.getField(info, 1));
                console.SetVar("$pref::SFX::useHardware", Util.getField(info, 2));
                // By default we've decided to avoid hardware devices as
                // they are buggy and prone to problems.
                console.SetVar("$pref::SFX::useHardware", false);
                if (!sfxInit()) continue;
                console.SetVar("$pref::SFX::autoDetect", false);
                console.Call(deviceTrySequence, "delete");
                return true;
                }
            // Found no suitable device.
            console.error("sfxAutodetect - Could not initialize a valid SFX device.");
            console.SetVar("$pref::SFX::provider", "");
            console.SetVar("$pref::SFX::device", "");
            console.SetVar("$pref::SFX::useHardware", "");

            console.Call(deviceTrySequence, "delete");
            return false;
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxOldChannelToGroup", "(%channel)", 1, 21000, false)]
        public string sfxOldChannelToGroup(string channel)
            {
            return console.GetVarString("$AudioChannels[" + channel + "]");
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxGroupToOldChannel", "(%group)", 1, 21000, false)]

        public string sfxGroupToOldChannel(string group)
            {
            string id = console.GetObjectID(group).AsString();
            for (int i = 0; ; i++)
                if (!console.isObject("$AudioChannels[" + i.AsString() + "]"))
                    return "-1";
                else if (console.GetVarString("$AudioChannels[" + i.AsString() + "]") == id)
                    return i.AsString();
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxSetMasterVolume", "(%volume)", 1, 21000, false)]
        public void sfxSetMasterVolume(string volume)
            {
            SFXSource.setVolume("AudioChannelMaster", volume.AsFloat());
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxGetMasterVolume", "()", 0, 21000, false)]
        public string sfxGetMasterVolume()
            {
            return SFXSource.getVolume("AudioChannelMaster").AsString();
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxStopAll", "(%channel)", 1, 21000, false)]
        public void sfxStopAll(string channel)
            {
            channel = sfxOldChannelToGroup(channel);
            if (!console.isObject(channel)) return;
            for (uint i = 0; i < SimSet.getCount(channel); i++)
                SFXSource.stop(SimSet.getObject(channel, i), -1);
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxGetChannelVolume", "(%channel)", 1, 21000, false)]
        public string sfxGetChannelVolume(string channel)
            {
            string obj = sfxOldChannelToGroup(channel);
            return console.isObject(obj) ? SFXSource.getVolume(obj).AsString() : "0";
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxSetChannelVolume", "(%channel,%volume)", 2, 21000, false)]
        public void sfxSetChannelVolume(string channel, float volume)
            {
            string obj = sfxOldChannelToGroup(channel);
            if (console.isObject(obj))
                SFXSource.setVolume(obj, volume);
            }

        /// Pauses the playback of active sound sources.
        /// 
        /// @param %channels    An optional word list of channel indices or an empty 
        ///                     string to pause sources on all channels.
        /// @param %pauseSet    An optional SimSet which is filled with the paused 
        ///                     sources.  If not specified the global SfxSourceGroup 
        ///                     is used.
        ///
        /// @deprecated
        ///
        [Torque_Decorations.TorqueCallBack("", "", "sfxPause", "(%channels, %pauseSet)", 2, 21000, false)]
        public void sfxPause(string channels, string pauseSet)
            {
            if (!console.isObject(pauseSet))
                pauseSet = console.GetVarString("SFXPausedSet");

            int count = SimSet.getCount("SFXSourceSet");
            for (uint i = 0; i < count; i++)
                {
                string source = SimSet.getObject("SFXSourceSet", i);
                string channel = sfxGroupToOldChannel(console.Call(source, "getGroup"));
                if (channel != "" &&

                    console.Call("findWord", new string[] { channels, channel }).AsInt() == -1) continue;
                SFXSource.pause(source, -1);
                SimSet.pushToBack(pauseSet, source);
                }
            }

        /// Resumes the playback of paused sound sources.
        /// 
        /// @param %pauseSet    An optional SimSet which contains the paused sound 
        ///                     sources to be resumed.  If not specified the global 
        ///                     SfxSourceGroup is used.
        /// @deprecated
        ///
        /// 
        [Torque_Decorations.TorqueCallBack("", "", "sfxResume", "(%pauseSet)", 1, 21000, false)]
        public void sfxResume(string pauseSet)
            {
            if (!console.isObject(pauseSet))
                pauseSet = console.GetVarString("SFXPausedSet");

            int count = SimSet.getCount(pauseSet);
            for (uint i = 0; i < count; i++)
                {
                string source = SimSet.getObject(pauseSet, i);
                SFXSource.play(source, -1);
                }
            SimSet.clear(pauseSet);
            }
        }
    }
