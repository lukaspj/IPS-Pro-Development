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
        [Torque_Decorations.TorqueCallBack("", "", "postFXManagerguisettingscs", "", 0, 104000, true)]
        public void postFXManagerguisettingscs()
            {
            console.SetVar("$PostFXManager::defaultPreset", "art/postfx/default.postfxpreset.cs");
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsSetEnabled", "%this, %bEnablePostFX", 2, 104010, false)]
        public void PostFXManagersettingsSetEnabled(string thisobj, string bEnablePostFX)
            {
            console.SetVar("$PostFXManager::PostFX::Enabled", bEnablePostFX);
            //if to enable the postFX, apply the ones that are enabled

            if (bEnablePostFX.AsBool())
                {
                console.Call("SSAOPostFx", console.GetVarBool("$PostFXManager::PostFX::EnableSSAO") ? "enable" : "disable");
                console.Call("HDRPostFX", console.GetVarBool("$PostFXManager::PostFX::EnableHDR") ? "enable" : "disable");
                console.Call("LightRayPostFX", console.GetVarBool("$PostFXManager::PostFX::EnableLightRays") ? "enable" : "disable");
                console.Call("DOFPostEffect", console.GetVarBool("$PostFXManager::PostFX::EnableDOF") ? "enable" : "disable");
                console.Call("postVerbose", new string[] { "% - PostFX Manager - PostFX enabled" });
                }
            else
                {
                console.Call("SSAOPostFx", "disable");
                console.Call("HDRPostFX", "disable");
                console.Call("LightRayPostFX", "disable");
                console.Call("DOFPostEffect", "disable");
                console.Call("postVerbose", new string[] { "% - PostFX Manager - PostFX disabled" });
                }
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsEffectSetEnabled", "%this, %sName, %bEnable", 2, 104020, false)]
        public void PostFXManagersettingsEffectSetEnabled(string thisobj, string name, string benabled)
            {
            string postEffect = "0";
            //Determine the postFX to enable, and apply the boolean
            if (name == "SSAO")
                {
                postEffect = console.GetVarString("SSAOPostFx");
                console.SetVar("$PostFXManager::PostFX::EnableSSAO", benabled);
                }
            else if (name == "HDR")
                {
                postEffect = console.GetVarString("HDRPostFX");
                console.SetVar("$PostFXManager::PostFX::EnableHDR", benabled);
                }
            else if (name == "LightRays")
                {
                postEffect = console.GetVarString("LightRayPostFX");
                console.SetVar("$PostFXManager::PostFX::EnableLightRays", benabled);
                }
            else if (name == "DOF")
                {
                postEffect = console.GetVarString("DOFPostEffect");
                console.SetVar("$PostFXManager::PostFX::EnableDOF", benabled);
                }
            // Apply the change
            if (benabled.AsBool())
                {
                console.Call(postEffect, "enable");
                console.Call("postVerbose", new string[] { "% - PostFX Manager - " + name + " enabled" });
                }
            else
                {
                console.Call(postEffect, "disable");
                console.Call("postVerbose", new string[] { "% - PostFX Manager - " + name + " disable" });
                }
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsRefreshSSAO", "%this", 1, 104030, false)]
        public void PostFXManagersettingsRefreshSSAO(string thisobj)
            {
            //Apply the enabled flag 
            console.Call("ppOptionsEnableSSAO", "setValue", new string[] { console.GetVarString("$PostFXManager::PostFX::EnableSSAO") });
            //Add the items we need to display
            console.Call("ppOptionsSSAOQuality", "clear");
            console.Call("ppOptionsSSAOQuality", "add", new string[] { "Low", "0" });
            console.Call("ppOptionsSSAOQuality", "add", new string[] { "Medium", "1" });
            console.Call("ppOptionsSSAOQuality", "add", new string[] { "High", "2" });
            //Set the selected, after adding the items!

            console.Call("ppOptionsSSAOQuality", "setSelected", new string[] { console.GetVarString("$SSAOPostFx::quality") });
            //SSAO - Set the values of the sliders, General Tab

            console.Call("ppOptionsSSAOOverallStrength", "setValue", new string[] { console.GetVarString("$SSAOPostFx::overallStrength") });
            console.Call("ppOptionsSSAOBlurDepth", "setValue", new string[] { console.GetVarString("$SSAOPostFx::blurDepthTol") });
            console.Call("ppOptionsSSAOBlurNormal", "setValue", new string[] { console.GetVarString("$SSAOPostFx::blurNormalTol") });
            //SSAO - Set the values for the near tab

            console.Call("ppOptionsSSAONearDepthMax", "setValue", new string[] { console.GetVarString("$SSAOPostFx::sDepthMax") });
            console.Call("ppOptionsSSAONearDepthMin", "setValue", new string[] { console.GetVarString("$SSAOPostFx::sDepthMin") });
            console.Call("ppOptionsSSAONearRadius", "setValue", new string[] { console.GetVarString("$SSAOPostFx::sRadius") });
            console.Call("ppOptionsSSAONearStrength", "setValue", new string[] { console.GetVarString("$SSAOPostFx::sStrength") });
            console.Call("ppOptionsSSAONearToleranceNormal", "setValue", new string[] { console.GetVarString("$SSAOPostFx::sNormalTol") });
            console.Call("ppOptionsSSAONearTolerancePower", "setValue", new string[] { console.GetVarString("$SSAOPostFx::sNormalPow") });

            //SSAO - Set the values for the far tab
            console.Call("ppOptionsSSAOFarDepthMax", "setValue", new string[] { console.GetVarString("$SSAOPostFx::lDepthMax") });
            console.Call("ppOptionsSSAOFarDepthMin", "setValue", new string[] { console.GetVarString("$SSAOPostFx::lDepthMin") });
            console.Call("ppOptionsSSAOFarRadius", "setValue", new string[] { console.GetVarString("$SSAOPostFx::lRadius") });
            console.Call("ppOptionsSSAOFarStrength", "setValue", new string[] { console.GetVarString("$SSAOPostFx::lStrength") });
            console.Call("ppOptionsSSAOFarToleranceNormal", "setValue", new string[] { console.GetVarString("$SSAOPostFx::lNormalTol") });
            console.Call("ppOptionsSSAOFarTolerancePower", "setValue", new string[] { console.GetVarString("$SSAOPostFx::lNormalPow") });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsRefreshHDR", "%this", 1, 104040, false)]
        public void PostFXManagersettingsRefreshHDR(string thisobj)
            {
            //Apply the enabled flag 
            console.Call("ppOptionsEnableHDR", "setValue", new string[] { console.GetVarString("$PostFXManager::PostFX::EnableHDR") });
            console.Call("ppOptionsHDRBloom", "setValue", new string[] { console.GetVarString("$HDRPostFX::enableBloom") });
            console.Call("ppOptionsHDRBloomBlurBrightPassThreshold", "setValue", new string[] { console.GetVarString("$HDRPostFX::brightPassThreshold") });
            console.Call("ppOptionsHDRBloomBlurMean", "setValue", new string[] { console.GetVarString("$HDRPostFX::gaussMean") });
            console.Call("ppOptionsHDRBloomBlurMultiplier", "setValue", new string[] { console.GetVarString("$HDRPostFX::gaussMultiplier") });
            console.Call("ppOptionsHDRBloomBlurStdDev", "setValue", new string[] { console.GetVarString("$HDRPostFX::gaussStdDev") });
            console.Call("ppOptionsHDRBrightnessAdaptRate", "setValue", new string[] { console.GetVarString("$HDRPostFX::adaptRate") });
            console.Call("ppOptionsHDREffectsBlueShift", "setValue", new string[] { console.GetVarString("$HDRPostFX::enableBlueShift") });


            console.SetVar("ppOptionsHDREffectsBlueShiftColor.BaseColor", console.GetVarString("$HDRPostFX::blueShiftColor"));
            console.SetVar("ppOptionsHDREffectsBlueShiftColor.PickColor", console.GetVarString("$HDRPostFX::blueShiftColor"));

            console.Call("ppOptionsHDRKeyValue", "setValue", new string[] { console.GetVarString("$HDRPostFX::keyValue") });
            console.Call("ppOptionsHDRMinLuminance", "setValue", new string[] { console.GetVarString("$HDRPostFX::minLuminace") });
            console.Call("ppOptionsHDRToneMapping", "setValue", new string[] { console.GetVarString("$HDRPostFX::enableToneMapping") });
            console.Call("ppOptionsHDRToneMappingAmount", "setValue", new string[] { console.GetVarString("$HDRPostFX::enableToneMapping") });
            console.Call("ppOptionsHDRWhiteCutoff", "setValue", new string[] { console.GetVarString("$HDRPostFX::whiteCutoff") });

            console.SetVar(SimSet.findObjectByInternalName(thisobj, "ColorCorrectionFileName", true), console.GetVarString("$HDRPostFX::colorCorrectionRamp"));

            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsRefreshLightrays", "%this", 1, 104050, false)]
        public void PostFXManagersettingsRefreshLightrays(string thisobj)
            {
            console.Call("ppOptionsEnableLightRays", "setValue", new string[] { console.GetVarString("$PostFXManager::PostFX::EnableLightRays") });
            console.Call("ppOptionsLightRaysBrightScalar", "setValue", new string[] { console.GetVarString("$LightRayPostFX::brightScalar") });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsRefreshDOF", "%this", 1, 104060, false)]
        public void PostFXManagersettingsRefreshDOF(string thisobj)
            {
            console.Call("ppOptionsEnableDOF", "setValue", new string[] { console.GetVarString("$PostFXManager::PostFX::EnableDOF") });
            console.Call("ppOptionsDOFEnableAutoFocus", "setValue", new string[] { console.GetVarString("$DOFPostFx::EnableAutoFocus") });
            console.Call("ppOptionsDOFFarBlurMinSlider", "setValue", new string[] { console.GetVarString("$DOFPostFx::BlurMin") });
            console.Call("ppOptionsDOFFarBlurMaxSlider", "setValue", new string[] { console.GetVarString("$DOFPostFx::BlurMax") });
            console.Call("ppOptionsDOFFocusRangeMinSlider", "setValue", new string[] { console.GetVarString("$DOFPostFx::FocusRangeMin") });
            console.Call("ppOptionsDOFFocusRangeMaxSlider", "setValue", new string[] { console.GetVarString("$DOFPostFx::FocusRangeMax") });
            console.Call("ppOptionsDOFBlurCurveNearSlider", "setValue", new string[] { console.GetVarString("$DOFPostFx::BlurCurveNear") });
            console.Call("ppOptionsDOFBlurCurveFarSlider", "setValue", new string[] { console.GetVarString("$DOFPostFx::BlurCurveFar") });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsRefreshAll", "%this", 1, 104070, false)]
        public void PostFXManagersettingsRefreshAll(string thisobj)
            {
            console.SetVar("$PostFXManager::PostFX::Enabled", console.GetVarString("$pref::enablePostEffects"));
            console.SetVar("$PostFXManager::PostFX::EnableSSAO", console.Call("SSAOPostFx", "isEnabled"));
            console.SetVar("$PostFXManager::PostFX::EnableHDR", console.Call("HDRPostFX", "isEnabled"));
            console.SetVar("$PostFXManager::PostFX::EnableLightRays", console.Call("LightRayPostFX", "isEnabled"));
            console.SetVar("$PostFXManager::PostFX::EnableDOF", console.Call("DOFPostEffect", "isEnabled"));
            //For all the postFX here, apply the active settings in the system
            //to the gui controls.

            console.Call(thisobj, "settingsRefreshSSAO");
            console.Call(thisobj, "settingsRefreshHDR");
            console.Call(thisobj, "settingsRefreshLightrays");
            console.Call(thisobj, "settingsRefreshDOF");

            console.Call("ppOptionsEnable", "setValue", new string[] { console.GetVarString("$PostFXManager::PostFX::Enabled") });

            console.Call("postVerbose", new string[] { "% - PostFX Manager - GUI values updated." });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsApplyFromPreset", "%this", 1, 104070, false)]
        public void PostFXManagersettingsApplyFromPreset(string thisobj)
            {
            console.Call("postVerbose", new string[] { "% - PostFX Manager - Applying from preset" });


            console.SetVar("$SSAOPostFx::blurDepthTol", console.GetVarString("$PostFXManager::Settings::SSAO::blurDepthTol"));
            console.SetVar("$SSAOPostFx::blurNormalTol", console.GetVarString("$PostFXManager::Settings::SSAO::blurNormalTol"));
            console.SetVar("$SSAOPostFx::lDepthMax", console.GetVarString("$PostFXManager::Settings::SSAO::lDepthMax"));
            console.SetVar("$SSAOPostFx::lDepthMin", console.GetVarString("$PostFXManager::Settings::SSAO::lDepthMin"));
            console.SetVar("$SSAOPostFx::lDepthPow", console.GetVarString("$PostFXManager::Settings::SSAO::lDepthPow"));
            console.SetVar("$SSAOPostFx::lNormalPow", console.GetVarString("$PostFXManager::Settings::SSAO::lNormalPow"));
            console.SetVar("$SSAOPostFx::lNormalTol", console.GetVarString("$PostFXManager::Settings::SSAO::lNormalTol"));
            console.SetVar("$SSAOPostFx::lRadius", console.GetVarString("$PostFXManager::Settings::SSAO::lRadius"));
            console.SetVar("$SSAOPostFx::lStrength", console.GetVarString("$PostFXManager::Settings::SSAO::lStrength"));
            console.SetVar("$SSAOPostFx::overallStrength", console.GetVarString("$PostFXManager::Settings::SSAO::overallStrength"));
            console.SetVar("$SSAOPostFx::quality", console.GetVarString("$PostFXManager::Settings::SSAO::quality"));
            console.SetVar("$SSAOPostFx::sDepthMax", console.GetVarString("$PostFXManager::Settings::SSAO::sDepthMax"));
            console.SetVar("$SSAOPostFx::sDepthMin", console.GetVarString("$PostFXManager::Settings::SSAO::sDepthMin"));
            console.SetVar("$SSAOPostFx::sDepthPow", console.GetVarString("$PostFXManager::Settings::SSAO::sDepthPow"));
            console.SetVar("$SSAOPostFx::sNormalPow", console.GetVarString("$PostFXManager::Settings::SSAO::sNormalPow"));
            console.SetVar("$SSAOPostFx::sNormalTol", console.GetVarString("$PostFXManager::Settings::SSAO::sNormalTol"));
            console.SetVar("$SSAOPostFx::sRadius", console.GetVarString("$PostFXManager::Settings::SSAO::sRadius"));
            console.SetVar("$SSAOPostFx::sStrength", console.GetVarString("$PostFXManager::Settings::SSAO::sStrength"));

            //HDRsettings
            console.SetVar("$HDRPostFX::adaptRate", console.GetVarString("$PostFXManager::Settings::HDR::adaptRate"));
            console.SetVar("$HDRPostFX::blueShiftColor", console.GetVarString("$PostFXManager::Settings::HDR::blueShiftColor"));
            console.SetVar("$HDRPostFX::brightPassThreshold", console.GetVarString("$PostFXManager::Settings::HDR::brightPassThreshold"));
            console.SetVar("$HDRPostFX::enableBloom", console.GetVarString("$PostFXManager::Settings::HDR::enableBloom"));
            console.SetVar("$HDRPostFX::enableBlueShift", console.GetVarString("$PostFXManager::Settings::HDR::enableBlueShift"));
            console.SetVar("$HDRPostFX::enableToneMapping", console.GetVarString("$PostFXManager::Settings::HDR::enableToneMapping"));
            console.SetVar("$HDRPostFX::gaussMean", console.GetVarString("$PostFXManager::Settings::HDR::gaussMean"));
            console.SetVar("$HDRPostFX::gaussMultiplier", console.GetVarString("$PostFXManager::Settings::HDR::gaussMultiplier"));
            console.SetVar("$HDRPostFX::gaussStdDev", console.GetVarString("$PostFXManager::Settings::HDR::gaussStdDev"));
            console.SetVar("$HDRPostFX::keyValue", console.GetVarString("$PostFXManager::Settings::HDR::keyValue"));
            console.SetVar("$HDRPostFX::minLuminace", console.GetVarString("$PostFXManager::Settings::HDR::minLuminace"));
            console.SetVar("$HDRPostFX::whiteCutoff", console.GetVarString("$PostFXManager::Settings::HDR::whiteCutoff"));

            //Not sure what is going on here, but for some reason we got some swaping going on and it ain't pretty.
            if (console.GetVarString("$PostFXManager::Settings::ColorCorrectionRamp") != "")
                console.SetVar("$HDRPostFX::colorCorrectionRamp", console.GetVarString("$PostFXManager::Settings::ColorCorrectionRamp"));

            //Lightrayssettings
            console.SetVar("$LightRayPostFX::brightScalar", console.GetVarString("$PostFXManager::Settings::LightRays::brightScalar"));

            //DOFsettings
            console.SetVar("$DOFPostFx::EnableAutoFocus", console.GetVarString("$PostFXManager::Settings::DOF::EnableAutoFocus"));
            console.SetVar("$DOFPostFx::BlurMin", console.GetVarString("$PostFXManager::Settings::DOF::BlurMin"));
            console.SetVar("$DOFPostFx::BlurMax", console.GetVarString("$PostFXManager::Settings::DOF::BlurMax"));
            console.SetVar("$DOFPostFx::FocusRangeMin", console.GetVarString("$PostFXManager::Settings::DOF::FocusRangeMin"));
            console.SetVar("$DOFPostFx::FocusRangeMax", console.GetVarString("$PostFXManager::Settings::DOF::FocusRangeMax"));
            console.SetVar("$DOFPostFx::BlurCurveNear", console.GetVarString("$PostFXManager::Settings::DOF::BlurCurveNear"));
            console.SetVar("$DOFPostFx::BlurCurveFar", console.GetVarString("$PostFXManager::Settings::DOF::BlurCurveFar"));


            if (console.GetVarBool("$PostFXManager::forceEnableFromPresets"))
                {
                console.SetVar("$PostFXManager::PostFX::Enabled", console.GetVarString("$PostFXManager::Settings::EnablePostFX"));
                console.SetVar("$PostFXManager::PostFX::EnableDOF", console.GetVarString("$PostFXManager::Settings::EnableDOF"));
                console.SetVar("$PostFXManager::PostFX::EnableLightRays", console.GetVarString("$PostFXManager::Settings::EnableLightRays"));
                console.SetVar("$PostFXManager::PostFX::EnableHDR", console.GetVarString("$PostFXManager::Settings::EnableHDR"));
                console.SetVar("$PostFXManager::PostFX::EnableSSAO", console.GetVarString("$PostFXManager::Settings::EnabledSSAO"));

                console.Call(thisobj, "settingsSetEnabled", new string[] { "true" });
                }
            //make sure we apply the correct settings to the DOF
            console.Call("ppOptionsUpdateDOFSettings");
            // Update the actual GUI controls if its awake ( otherwise it will when opened ).
            if (console.Call("PostFXManager", "isAwake").AsBool())
                {
                console.Call(thisobj, "settingsRefreshAll");
                }
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsApplySSAO", "%this", 1, 104070, false)]
        public void PostFXManagersettingsApplySSAO(string thisobj)
            {
            console.SetVar("$PostFXManager::Settings::SSAO::blurDepthTol", console.GetVarString("$SSAOPostFx::blurDepthTol"));
            console.SetVar("$PostFXManager::Settings::SSAO::blurNormalTol", console.GetVarString("$SSAOPostFx::blurNormalTol"));
            console.SetVar("$PostFXManager::Settings::SSAO::lDepthMax", console.GetVarString("$SSAOPostFx::lDepthMax"));
            console.SetVar("$PostFXManager::Settings::SSAO::lDepthMin", console.GetVarString("$SSAOPostFx::lDepthMin"));
            console.SetVar("$PostFXManager::Settings::SSAO::lDepthPow", console.GetVarString("$SSAOPostFx::lDepthPow"));
            console.SetVar("$PostFXManager::Settings::SSAO::lNormalPow", console.GetVarString("$SSAOPostFx::lNormalPow"));
            console.SetVar("$PostFXManager::Settings::SSAO::lNormalTol", console.GetVarString("$SSAOPostFx::lNormalTol"));
            console.SetVar("$PostFXManager::Settings::SSAO::lRadius", console.GetVarString("$SSAOPostFx::lRadius"));
            console.SetVar("$PostFXManager::Settings::SSAO::lStrength", console.GetVarString("$SSAOPostFx::lStrength"));
            console.SetVar("$PostFXManager::Settings::SSAO::overallStrength", console.GetVarString("$SSAOPostFx::overallStrength"));
            console.SetVar("$PostFXManager::Settings::SSAO::quality", console.GetVarString("$SSAOPostFx::quality"));
            console.SetVar("$PostFXManager::Settings::SSAO::sDepthMax", console.GetVarString("$SSAOPostFx::sDepthMax"));
            console.SetVar("$PostFXManager::Settings::SSAO::sDepthMin", console.GetVarString("$SSAOPostFx::sDepthMin"));
            console.SetVar("$PostFXManager::Settings::SSAO::sDepthPow", console.GetVarString("$SSAOPostFx::sDepthPow"));
            console.SetVar("$PostFXManager::Settings::SSAO::sNormalPow", console.GetVarString("$SSAOPostFx::sNormalPow"));
            console.SetVar("$PostFXManager::Settings::SSAO::sNormalTol", console.GetVarString("$SSAOPostFx::sNormalTol"));
            console.SetVar("$PostFXManager::Settings::SSAO::sRadius", console.GetVarString("$SSAOPostFx::sRadius"));
            console.SetVar("$PostFXManager::Settings::SSAO::sStrength", console.GetVarString("$SSAOPostFx::sStrength"));

            console.Call("postVerbose", new string[] { "% - PostFX Manager - Settings Saved - SSAO" });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsApplyHDR", "%this", 1, 104080, false)]
        public void PostFXManagersettingsApplyHDR(string thisobj)
            {
            console.SetVar("$PostFXManager::Settings::HDR::adaptRate", console.GetVarString("$HDRPostFX::adaptRate"));
            console.SetVar("$PostFXManager::Settings::HDR::blueShiftColor", console.GetVarString("$HDRPostFX::blueShiftColor"));
            console.SetVar("$PostFXManager::Settings::HDR::brightPassThreshold", console.GetVarString("$HDRPostFX::brightPassThreshold"));
            console.SetVar("$PostFXManager::Settings::HDR::enableBloom", console.GetVarString("$HDRPostFX::enableBloom"));
            console.SetVar("$PostFXManager::Settings::HDR::enableBlueShift", console.GetVarString("$HDRPostFX::enableBlueShift"));
            console.SetVar("$PostFXManager::Settings::HDR::enableToneMapping", console.GetVarString("$HDRPostFX::enableToneMapping"));
            console.SetVar("$PostFXManager::Settings::HDR::gaussMean", console.GetVarString("$HDRPostFX::gaussMean"));
            console.SetVar("$PostFXManager::Settings::HDR::gaussMultiplier", console.GetVarString("$HDRPostFX::gaussMultiplier"));
            console.SetVar("$PostFXManager::Settings::HDR::gaussStdDev", console.GetVarString("$HDRPostFX::gaussStdDev"));
            console.SetVar("$PostFXManager::Settings::HDR::keyValue", console.GetVarString("$HDRPostFX::keyValue"));
            console.SetVar("$PostFXManager::Settings::HDR::minLuminace", console.GetVarString("$HDRPostFX::minLuminace"));
            console.SetVar("$PostFXManager::Settings::HDR::whiteCutoff", console.GetVarString("$HDRPostFX::whiteCutoff"));
            console.SetVar("$PostFXManager::Settings::ColorCorrectionRamp", console.GetVarString("$HDRPostFX::colorCorrectionRamp"));
            console.Call("postVerbose", new string[] { "% - PostFX Manager - Settings Saved - HDR" });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsApplyLightRays", "%this", 1, 104090, false)]
        public void PostFXManagersettingsApplyLightRays(string thisobj)
            {
            console.SetVar("$PostFXManager::Settings::LightRays::brightScalar", console.GetVarString("$LightRayPostFX::brightScalar"));
            console.Call("postVerbose", new string[] { "% - PostFX Manager - Settings Saved - Light Rays" });
            }

        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsApplyDOF", "%this", 1, 104100, false)]
        public void PostFXManagersettingsApplyDOF(string thisobj)
            {
            console.SetVar("$PostFXManager::Settings::DOF::EnableAutoFocus", console.GetVarString("$DOFPostFx::EnableAutoFocus"));
            console.SetVar("$PostFXManager::Settings::DOF::BlurMin", console.GetVarString("$DOFPostFx::BlurMin"));
            console.SetVar("$PostFXManager::Settings::DOF::BlurMax", console.GetVarString("$DOFPostFx::BlurMax"));
            console.SetVar("$PostFXManager::Settings::DOF::FocusRangeMin", console.GetVarString("$DOFPostFx::FocusRangeMin"));
            console.SetVar("$PostFXManager::Settings::DOF::FocusRangeMax", console.GetVarString("$DOFPostFx::FocusRangeMax"));
            console.SetVar("$PostFXManager::Settings::DOF::BlurCurveNear", console.GetVarString("$DOFPostFx::BlurCurveNear"));
            console.SetVar("$PostFXManager::Settings::DOF::BlurCurveFar", console.GetVarString("$DOFPostFx::BlurCurveFar"));
            console.Call("postVerbose", new string[] { "% - PostFX Manager - Settings Saved - DOF" });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsApplyAll", "%this,%sFrom", 2, 104110, false)]
        public void PostFXManagersettingsApplyAll(string thisobj, string sfrom)
            {
            // Apply settings which control if effects are on/off altogether.
            console.SetVar("$PostFXManager::Settings::EnablePostFX", console.GetVarString("$PostFXManager::PostFX::Enabled"));
            console.SetVar("$PostFXManager::Settings::EnableDOF", console.GetVarString("$PostFXManager::PostFX::EnableDOF"));
            console.SetVar("$PostFXManager::Settings::EnableLightRays", console.GetVarString("$PostFXManager::PostFX::EnableLightRays"));
            console.SetVar("$PostFXManager::Settings::EnableHDR", console.GetVarString("$PostFXManager::PostFX::EnableHDR"));
            console.SetVar("$PostFXManager::Settings::EnabledSSAO", console.GetVarString("$PostFXManager::PostFX::EnableSSAO"));
            // Apply settings should save the values in the system to the 
            // the preset structure ($PostFXManager::Settings::*)

            // SSAO Settings
            console.Call(thisobj, "settingsApplySSAO");
            // HDR settings
            console.Call(thisobj, "settingsApplyHDR");
            // Light rays settings
            console.Call(thisobj, "settingsApplyLightRays");
            // DOF
            console.Call(thisobj, "settingsApplyDOF");

            console.Call("postVerbose", new string[] { "% - PostFX Manager - All Settings applied to $PostFXManager::Settings" });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "settingsApplyDefaultPreset", "%this", 1, 104120, false)]
        public void PostFXManagersettingsApplyDefaultPreset(String thisobj)
            {
            //Ok, looks like someone made an opps, according to the syntax, this should be a classname call.
            //But instead it's an object call.
            //I did add a handler so it checks on call if it's a classname, if it isn't
            //it them checks to see if it's an object.
            //con.Eval("PostFXManager::loadPresetHandler($PostFXManager::defaultPreset);");
            //con.Call_Classname("PostFXManager", "loadPresetHandler", new string[] { con.GetVarString("$PostFXManager::defaultPreset") });
            console.Call("PostFXManager", "loadPresetHandler", new string[] { console.GetVarString("$PostFXManager::defaultPreset") });


            }
        }
    }
