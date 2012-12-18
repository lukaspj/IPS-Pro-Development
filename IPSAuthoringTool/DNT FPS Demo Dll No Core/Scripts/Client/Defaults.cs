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
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        // First we execute the core default preferences.

        [Torque_Decorations.TorqueCallBack("", "", "clientdefaults_load", "()",  0, 1000, true)]
        public string ClientdefaultsLoad()
            {
            dnt.DoScriptInjection(ScriptType.Client, 80000,80999);
            //console.Eval(@"dnEval(""core_client.init_Defaultscs()"");");
            //Util.exec("core/scripts/client/defaults.cs", false, false);


            // Now add your own game specific client preferences as
            // well as any overloaded core defaults here.
            

            // Finally load the preferences saved from the last
            // game execution if they exist.


            if (console.GetVarString("$platform") != "xenon")
                {
                if (Util.isFile("scripts/client/prefs.cs"))
                    Util.exec("scripts/client/prefs.cs", false, false);
                }
            else
                console.print("Not loading client prefs.cs on Xbox360");
            return string.Empty;
            }












        //public void init_Defaultscs()
        //    {
        //    dnt.DoScriptInjection(ScriptType.Core_Client, 60000, 60400);
        //    }


        [Torque_Decorations.TorqueCallBack("", "TextureQualityGroup", "onApply", "%this, %level", 2, 80100, false)]
        public void TextureQualityGrouponApply(string thisobj, string level)
            {
            Util.reloadTextures();
            }
        [Torque_Decorations.TorqueCallBack("", "LightingQualityGroup", "onApply", "%this, %level", 2, 80200, false)]
        public void LightingQualityGrouponApply(string thisobj, string level)
            {
            Util.setLightManager(console.GetVarString("$pref::lightManager"));
            }

        [Torque_Decorations.TorqueCallBack("", "", "GraphicsQualityAutodetect", "", 0, 80300, false)]
        public string GraphicsQualityAutodetect()
            {
            console.SetVar("$pref::Video::autoDetect", false);
            float shaderVer = Util.getPixelShaderVersion();
            bool intel = (Util.strstr(Util.strupr(Util.getDisplayDeviceInformation()), "INTEL") != -1) ? true : false;

            string videoMem = console.Call_Classname("GFXCardProfilerAPI", "getVideoMemoryMB");

            return console.Call("GraphicsQualityAutodetect_Apply", new string[] { shaderVer.AsString(), intel.AsString(), videoMem });
            }
        [Torque_Decorations.TorqueCallBack("", "", "GraphicsQualityAutodetect_Apply", "%shaderVer, %intel, %videoMem ", 3, 80400, false)]
        public string GraphicsQualityAutodetect_Apply(string shaderVer, string intel, string videoMem)
            {
            if (shaderVer.AsFloat() < 2.0)
                {
                return "Your video card does not meet the minimum requirment of shader model 2.0.";
                }
            if (shaderVer.AsFloat() < 3 || intel.AsBool())
                {
                if (shaderVer.AsFloat() > 2.0)
                    {
                    console.Call(SimSet.findObjectByInternalName("MeshQualityGroup", "Lowest", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("TextureQualityGroup", "Lowest", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("LightingQualityGroup", "Lowest", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("ShaderQualityGroup", "Lowest", true), "apply");
                    }
                else
                    {
                    console.Call(SimSet.findObjectByInternalName("MeshQualityGroup", "Lowest", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("TextureQualityGroup", "Lowest", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("LightingQualityGroup", "Lowest", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("ShaderQualityGroup", "Lowest", true), "apply");
                    }
                }
            else
                {
                if (videoMem.AsInt() > 1000)
                    {
                    console.Call(SimSet.findObjectByInternalName("MeshQualityGroup", "High", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("TextureQualityGroup", "High", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("LightingQualityGroup", "High", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("ShaderQualityGroup", "High", true), "apply");
                    }
                else if (videoMem.AsInt() > 400 || videoMem.AsInt() == 0)
                    {
                    console.Call(SimSet.findObjectByInternalName("MeshQualityGroup", "Normal", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("TextureQualityGroup", "Normal", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("LightingQualityGroup", "Normal", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("ShaderQualityGroup", "Normal", true), "apply");
                    if (videoMem.AsInt() == 0)
                        return "Torque was unable to detect available video memory. Applying 'Normal' quality.";
                    }
                else
                    {
                    console.Call(SimSet.findObjectByInternalName("MeshQualityGroup", "Low", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("TextureQualityGroup", "Low", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("LightingQualityGroup", "Low", true), "apply");
                    console.Call(SimSet.findObjectByInternalName("ShaderQualityGroup", "Low", true), "apply");
                    }
                }
            return "Graphics quality settings have been auto detected.";
            }



        [Torque_Decorations.TorqueCallBack("", "", "Initialize_defaults", "", 0, 80000, true)]
        public void Initialize_defaults()
            {
            // The master server is declared with the server defaults, which is
            // loaded on both clients & dedicated servers.  If the server mod
            // is not loaded on a client, then the master must be defined. 
            // $pref::Master[0] = "2:master.garagegames.com:28002";

            console.SetVar("$pref::Player::Name", "Visitor");
            console.SetVar("$pref::Player::defaultFov", 65);
            console.SetVar("$pref::Player::zoomSpeed", 0);

            console.SetVar("$pref::Net::LagThreshold", 400);
            console.SetVar("$pref::Net::Port", 28000);

            console.SetVar("$pref::HudMessageLogSize", 40);
            console.SetVar("$pref::ChatHudLength", 1);

            console.SetVar("$pref::Input::LinkMouseSensitivity", 1);
            // DInput keyboard, mouse, and joystick prefs
            console.SetVar("$pref::Input::KeyboardEnabled", 1);
            console.SetVar("$pref::Input::MouseEnabled", 1);
            console.SetVar("$pref::Input::JoystickEnabled", 0);
            console.SetVar("$pref::Input::KeyboardTurnSpeed", 0.1);

            console.SetVar("$sceneLighting::cacheSize", 20000);
            console.SetVar("$sceneLighting::purgeMethod", "lastCreated");
            console.SetVar("$sceneLighting::cacheLighting", 1);

            console.SetVar("$pref::Video::displayDevice", "D3D9");
            console.SetVar("$pref::Video::disableVerticalSync", 1);
            console.SetVar("$pref::Video::mode", "800 600 false 32 60 4");
            console.SetVar("$pref::Video::defaultFenceCount", 0);
            console.SetVar("$pref::Video::screenShotSession", 0);
            console.SetVar("$pref::Video::screenShotFormat", "PNG");

            // This disables the hardware FSAA/MSAA so that
            // we depend completely on the FXAA post effect
            // which works on all cards and in deferred mode.
            //
            // Note the new Intel Hybrid graphics on laptops
            // will fail to initialize when hardware AA is
            // enabled... so you've been warned.
            //

            console.SetVar("$pref::Video::disableHardwareAA", true);

            console.SetVar("$pref::Video::disableNormalmapping", false);

            console.SetVar("$pref::Video::disablePixSpecular", false);

            console.SetVar("$pref::Video::disableCubemapping", false);


            console.SetVar("$pref::Video::disableParallaxMapping", false);

            console.SetVar("$pref::Video::Gamma", 1.0);

            if (console.GetVarString("$platform") == "xenon")
                {
                console.SetVar("$pref::Video::Resolution", "1152 640");
                console.SetVar("$pref::Video::mode", console.GetVarString("$pref::Video::Resolution") + " true 32 60 0");
                console.SetVar("$pref::Video::fullScreen", 1);
                }

            // This is the path used by ShaderGen to cache procedural
            // shaders.  If left blank ShaderGen will only cache shaders
            // to memory and not to disk.
            console.SetVar("$shaderGen::cachePath", "shaders/procedural");

            // The perfered light manager to use at startup.  If blank
            // or if the selected one doesn't work on this platfom it
            // will try the defaults below.
            console.SetVar("$pref::lightManager", "");

            // This is the default list of light managers ordered from
            // most to least desirable for initialization.
            console.SetVar("$lightManager::defaults", "Advanced Lighting\nBasic Lighting");

            // A scale to apply to the camera view distance
            // typically used for tuning performance.
            console.SetVar("$pref::camera::distanceScale", 1.0);

            // Causes the system to do a one time autodetect
            // of an SFX provider and device at startup if the
            // provider is unset.
            console.SetVar("$pref::SFX::autoDetect", true);

            // The sound provider to select at startup.  Typically
            // this is DirectSound, OpenAL, or XACT.  There is also 
            // a special Null provider which acts normally, but 
            // plays no sound.
            console.SetVar("$pref::SFX::provider", "");

            // The sound device to select from the provider.  Each
            // provider may have several different devices.
            console.SetVar("$pref::SFX::device", "OpenAL");

            // If true the device will try to use hardware buffers
            // and sound mixing.  If not it will use software.
            console.SetVar("$pref::SFX::useHardware", false);

            // If you have a software device you have a 
            // choice of how many software buffers to
            // allow at any one time.  More buffers cost
            // more CPU time to process and mix.
            console.SetVar("$pref::SFX::maxSoftwareBuffers", 16);

            // This is the playback frequency for the primary 
            // sound buffer used for mixing.  Although most
            // providers will reformat on the fly, for best 
            // quality and performance match your sound files
            // to this setting.
            console.SetVar("$pref::SFX::frequency", 44100);

            // This is the playback bitrate for the primary 
            // sound buffer used for mixing.  Although most
            // providers will reformat on the fly, for best 
            // quality and performance match your sound files
            // to this setting.
            console.SetVar("$pref::SFX::bitrate", 32);

            // The overall system volume at startup.  Note that 
            // you can only scale volume down, volume does not
            // get louder than 1.
            console.SetVar("$pref::SFX::masterVolume", 0.8);

            // The startup sound channel volumes.  These are 
            // used to control the overall volume of different 
            // classes of sounds.
            console.SetVar("$pref::SFX::channelVolume1", 1);
            console.SetVar("$pref::SFX::channelVolume2", 1);
            console.SetVar("$pref::SFX::channelVolume3", 1);
            console.SetVar("$pref::SFX::channelVolume4", 1);
            console.SetVar("$pref::SFX::channelVolume5", 1);
            console.SetVar("$pref::SFX::channelVolume6", 1);
            console.SetVar("$pref::SFX::channelVolume7", 1);
            console.SetVar("$pref::SFX::channelVolume8", 1);

            console.SetVar("$pref::PostEffect::PreferedHDRFormat", "GFXFormatR8G8B8A8");

            // This is an scalar which can be used to reduce the 
            // reflection textures on all objects to save fillrate.
            console.SetVar("$pref::Reflect::refractTexScale", 1.0);

            // This is the total frame in milliseconds to budget for
            // reflection rendering.  If your CPU bound and have alot
            // of smaller reflection surfaces try reducing this time.
            console.SetVar("$pref::Reflect::frameLimitMS", 10);

            // Set true to force all water objects to use static cubemap reflections.
            console.SetVar("$pref::Water::disableTrueReflections", false);

            // A global LOD scalar which can reduce the overall density of placed GroundCover.
            console.SetVar("$pref::GroundCover::densityScale", 1.0);

            // An overall scaler on the lod switching between DTS models.
            // Smaller numbers makes the lod switch sooner.
            console.SetVar("$pref::TS::detailAdjust", 1.0);

            //
            console.SetVar("$pref::Decals::enabled", true);

            //
            console.SetVar("$pref::Decals::lifeTimeScale", "1");

            // The number of mipmap levels to drop on loaded textures
            // to reduce video memory usage.  
            //
            // It will skip any textures that have been defined as not 
            // allowing down scaling.
            //
            console.SetVar("$pref::Video::textureReductionLevel", 0);

            //
            console.SetVar("$pref::Shadows::textureScalar", 1.0);

            //
            console.SetVar("$pref::Shadows::disable", false);

            // Sets the shadow filtering mode.
            //
            // None - Disables filtering.
            //
            // SoftShadow - Does a simple soft shadow
            //
            // SoftShadowHighQuality 
            //
            console.SetVar("$pref::Shadows::filterMode", "SoftShadow");

            //
            console.SetVar("$pref::Video::defaultAnisotropy", 1);

            // Radius in meters around the camera that ForestItems are affected by wind.
            // Note that a very large number with a large number of items is not cheap.
            console.SetVar("$pref::windEffectRadius", 25);

            // AutoDetect graphics quality levels the next startup.
            console.SetVar("$pref::Video::autoDetect", 1);

            //-----------------------------------------------------------------------------
            // Graphics Quality Groups
            //-----------------------------------------------------------------------------

            // The graphics quality groups are used by the options dialog to
            // control the state of the con.SetVar("$prefs.  You should overload these in
            // your game specific defaults.cs file if they need to be changed.
            if (console.isObject("MeshQualityGroup"))
                console.Call("MeshQualityGroup", "delete");
            if (console.isObject("TextureQualityGroup"))
                console.Call("TextureQualityGroup", "delete");
            if (console.isObject("LightingQualityGroup"))
                console.Call("LightingQualityGroup", "delete");
            if (console.isObject("ShaderQualityGroup"))
                console.Call("ShaderQualityGroup", "delete");

            //Nested objects... no support yet.
            console.Eval(
                @"

new SimGroup( MeshQualityGroup )
{ 
   new ArrayObject( [Lowest] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::TS::detailAdjust""] = 0.5;
      key[""$pref::TS::skipRenderDLs""] = 1;      
      key[""$pref::Terrain::lodScale""] = 2.0;
      key[""$pref::decalMgr::enabled""] = false;
      key[""$pref::GroundCover::densityScale""] = 0.5;
   };
   
   new ArrayObject( [Low] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
            
      key[""$pref::TS::detailAdjust""] = 0.75;
      key[""$pref::TS::skipRenderDLs""] = 0;      
      key[""$pref::Terrain::lodScale""] = 1.5;
      key[""$pref::decalMgr::enabled""] = true;
      key[""$pref::GroundCover::densityScale""] = 0.75;
   };
   
   new ArrayObject( [Normal] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;

      key[""$pref::TS::detailAdjust""] = 1.0;
      key[""$pref::TS::skipRenderDLs""] = 0;      
      key[""$pref::Terrain::lodScale""] = 1.0;
      key[""$pref::decalMgr::enabled""] = true;
      key[""$pref::GroundCover::densityScale""] = 1.0;
   };

   new ArrayObject( [High] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;

      key[""$pref::TS::detailAdjust""] = 1.5;
      key[""$pref::TS::skipRenderDLs""] = 0;      
      key[""$pref::Terrain::lodScale""] = 0.75;
      key[""$pref::decalMgr::enabled""] = true;
      key[""$pref::GroundCover::densityScale""] = 1.0;
   };   
};


new SimGroup( TextureQualityGroup )
{
   new ArrayObject( [Lowest] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::Video::textureReductionLevel""] = 2;
      key[""$pref::Reflect::refractTexScale""] = 0.5;
      key[""$pref::Terrain::detailScale""] = 0.5;      
   };
   
   new ArrayObject( [Low] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
            
      key[""$pref::Video::textureReductionLevel""] = 1;
      key[""$pref::Reflect::refractTexScale""] = 0.75;
      key[""$pref::Terrain::detailScale""] = 0.75;      
   };
   
   new ArrayObject( [Normal] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;

      key[""$pref::Video::textureReductionLevel""] = 0;
      key[""$pref::Reflect::refractTexScale""] = 1;
      key[""$pref::Terrain::detailScale""] = 1;      
   };

   new ArrayObject( [High] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;

      key[""$pref::Video::textureReductionLevel""] = 0;
      key[""$pref::Reflect::refractTexScale""] = 1.25;
      key[""$pref::Terrain::detailScale""] = 1.5;      
   };   
};

new SimGroup( LightingQualityGroup )
{ 
   new ArrayObject( [Lowest] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::lightManager""] = ""Basic Lighting"";
      key[""$pref::Shadows::disable""] = false;
      key[""$pref::Shadows::textureScalar""] = 0.5;
      key[""$pref::Shadows::filterMode""] = ""None"";     
   };
   
   new ArrayObject( [Low] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
                  
      key[""$pref::lightManager""] = ""Advanced Lighting"";
      key[""$pref::Shadows::disable""] = false;
      key[""$pref::Shadows::textureScalar""] = 0.5;
      key[""$pref::Shadows::filterMode""] = ""SoftShadow"";     
   };
   
   new ArrayObject( [Normal] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;

      key[""$pref::lightManager""] = ""Advanced Lighting"";
      key[""$pref::Shadows::disable""] = false;
      key[""$pref::Shadows::textureScalar""] = 1.0;
      key[""$pref::Shadows::filterMode""] = ""SoftShadowHighQuality"";     
   };

   new ArrayObject( [High] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::lightManager""] = ""Advanced Lighting"";
      key[""$pref::Shadows::disable""] = false;
      key[""$pref::Shadows::textureScalar""] = 2.0;
      key[""$pref::Shadows::filterMode""] = ""SoftShadowHighQuality"";          
   };   
};

new SimGroup( ShaderQualityGroup )
{
   new ArrayObject( [Lowest] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::Video::disablePixSpecular""] = true;
      key[""$pref::Video::disableNormalmapping""] = true;
      key[""$pref::Video::disableParallaxMapping""] = true;
      key[""$pref::Water::disableTrueReflections""] = true;
   };
   
   new ArrayObject( [Low] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::Video::disablePixSpecular""] = false;
      key[""$pref::Video::disableNormalmapping""] = false;
      key[""$pref::Video::disableParallaxMapping""] = true;
      key[""$pref::Water::disableTrueReflections""] = true;
   };
   
   new ArrayObject( [Normal] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::Video::disablePixSpecular""] = false;
      key[""$pref::Video::disableNormalmapping""] = false;
      key[""$pref::Video::disableParallaxMapping""] = false;   
      key[""$pref::Water::disableTrueReflections""] = false;   
   };
   
   new ArrayObject( [High] )
   {
      class = ""GraphicsQualityLevel"";
      caseSensitive = true;
      
      key[""$pref::Video::disablePixSpecular""] = false;
      key[""$pref::Video::disableNormalmapping""] = false;
      key[""$pref::Video::disableParallaxMapping""] = false;     
      key[""$pref::Water::disableTrueReflections""] = false;          
   };   
};


");

            }
        }
    }
