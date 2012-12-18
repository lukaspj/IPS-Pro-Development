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
        [Torque_Decorations.TorqueCallBack("", "", "postFXManager_gui_cs_cs_Init", "", 0, 103000, true)]
        public void postFXManager_gui_cs_cs_Init()
            {
            console.SetVar("$PostFXManager::vebose", true);
            }
        [Torque_Decorations.TorqueCallBack("", "", "postVerbose", "%string", 1, 103010, false)]
        public void postVerbose(string sstring)
            {
            if (console.GetVarBool("$PostFXManager::vebose"))
                console.print(sstring);
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "onDialogPush", "%this", 1, 103020, false)]
        public void PostFXManageronDialogPush(string thisobj)
            {
            //Apply the settings to the controls
            postVerbose("% - PostFX Manager - Loading GUI.");
            console.Call(thisobj, "settingsRefreshAll");
            }
        // :: Controls for the overall postFX manager dialog
        [Torque_Decorations.TorqueCallBack("", "ppOptionsEnable", "onAction", "%this", 1, 103030, false)]
        public void ppOptionsEnableonAction(string thisobj)
            {
            console.Call("PostFXManager", "settingsSetEnabled", console.Call("ppOptionsEnable", "getValue").AsBool() ? new string[] { "true" } : new string[] { "false" });
            }
        [Torque_Decorations.TorqueCallBack("", "PostFXManager", "getEnableResultFromControl", "%this, %control", 2, 103040, false)]
        public bool PostFXManagergetEnableResultFromControl(string thisobj, string control)
            {
            return console.Call(control, "getValue").AsBool();
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsEnableSSAO", "onAction", "%this", 1, 103050, false)]
        public void ppOptionsEnableSSAOonAction(string thisobj)
            {
            console.Call("PostFXManager", "settingsEffectSetEnabled",
                     new string[] { "SSAO", console.Call("PostFXManager", "getEnableResultFromControl", new string[] { thisobj }) });
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsEnableHDR", "onAction", "%this", 1, 103060, false)]
        public void ppOptionsEnableHDRonAction(string thisobj)
            {
            console.Call("PostFXManager", "settingsEffectSetEnabled",
                    new string[] { "HDR", console.Call("PostFXManager", "getEnableResultFromControl", new string[] { thisobj }) });
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsEnableLightRays", "onAction", "%this", 1, 103070, false)]
        public void ppOptionsEnableLightRaysonAction(string thisobj)
            {
            console.Call("PostFXManager", "settingsEffectSetEnabled",
                    new string[] { "LightRays", console.Call("PostFXManager", "getEnableResultFromControl", new string[] { thisobj }) });
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsEnableDOF", "onAction", "%this", 1, 103080, false)]
        public void ppOptionsEnableDOFsonAction(string thisobj)
            {
            console.Call("PostFXManager", "settingsEffectSetEnabled",
                    new string[] { "DOF", console.Call("PostFXManager", "getEnableResultFromControl", new string[] { thisobj }) });
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSavePreset", "onClick", "%this", 1, 103090, false)]
        public void ppOptionsSavePresetonClick(string thisobj)
            {
            //Stores the current settings into a preset file for loading and use later on 
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsLoadPreset", "onClick", "%this", 1, 103100, false)]
        public void ppOptionsLoadPresetonClick(string thisobj)
            {
            //Loads and applies the settings from a postfxpreset file
            }

        //Other controls, Quality dropdown
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOQuality", "onSelect", "%this, %id, %text", 3, 103110, false)]
        public void ppOptionsSSAOQualityonSelect(string thisobj, string id, string text)
            {
            if (id.AsInt() > -1 && id.AsInt() < 3)
                console.SetVar("$SSAOPostFx::quality", id);
            }

        //SSAO Slider controls
        //General Tab
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOOverallStrength", "onMouseDragged", "%this", 1, 103120, false)]
        public void ppOptionsSSAOOverallStrengthonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::overallStrength", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOBlurDepth", "onMouseDragged", "%this", 1, 103130, false)]
        public void ppOptionsSSAOBlurDepthonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::blurDepthTol", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOBlurNormal", "onMouseDragged", "%this", 1, 103140, false)]
        public void ppOptionsSSAOBlurNormalonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::blurNormalTol", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAONearRadius", "onMouseDragged", "%this", 1, 103150, false)]
        public void ppOptionsSSAONearRadiusonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::sRadius", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAONearStrength", "onMouseDragged", "%this", 1, 103160, false)]
        public void ppOptionsSSAONearStrengthonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::sStrength", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAONearDepthMin", "onMouseDragged", "%this", 1, 103170, false)]
        public void ppOptionsSSAONearDepthMinonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::sDepthMin", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAONearToleranceNormal", "onMouseDragged", "%this", 1, 103180, false)]
        public void ppOptionsSSAONearToleranceNormalonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::sNormalTol", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAONearTolerancePower", "onMouseDragged", "%this", 1, 103190, false)]
        public void ppOptionsSSAONearTolerancePoweronMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::sNormalPow", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOFarRadius", "onMouseDragged", "%this", 1, 103200, false)]
        public void ppOptionsSSAOFarRadiusonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::lRadius", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOFarStrength", "onMouseDragged", "%this", 1, 103210, false)]
        public void ppOptionsSSAOFarStrengthonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::lStrength", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOFarDepthMin", "onMouseDragged", "%this", 1, 103220, false)]
        public void ppOptionsSSAOFarDepthMinonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::lDepthMin", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOFarDepthMax", "onMouseDragged", "%this", 1, 103230, false)]
        public void ppOptionsSSAOFarDepthMaxonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::lDepthMax", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOFarToleranceNormal", "onMouseDragged", "%this", 1, 103240, false)]
        public void ppOptionsSSAOFarToleranceNormalonMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::lNormalTol", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsSSAOFarTolerancePower", "onMouseDragged", "%this", 1, 103250, false)]
        public void ppOptionsSSAOFarTolerancePoweronMouseDragged(string thisobj)
            {
            console.SetVar("$SSAOPostFx::lNormalPow", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRToneMappingAmount", "onMouseDragged", "%this", 1, 103260, false)]
        public void ppOptionsHDRToneMappingAmountonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::enableToneMapping", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRKeyValue", "onMouseDragged", "%this", 1, 103270, false)]
        public void ppOptionsHDRKeyValueonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::keyValue", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRMinLuminance", "onMouseDragged", "%this", 1, 103280, false)]
        public void ppOptionsHDRMinLuminanceonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::minLuminace", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRWhiteCutoff", "onMouseDragged", "%this", 1, 103290, false)]
        public void ppOptionsHDRWhiteCutoffonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::whiteCutoff", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRBrightnessAdaptRate", "onMouseDragged", "%this", 1, 103300, false)]
        public void ppOptionsHDRBrightnessAdaptRateonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::adaptRate", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRBloomBlurBrightPassThreshold", "onMouseDragged", "%this", 1, 103310, false)]
        public void ppOptionsHDRBloomBlurBrightPassThresholdonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::brightPassThreshold", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRBloomBlurMultiplier", "onMouseDragged", "%this", 1, 103320, false)]
        public void ppOptionsHDRBloomBlurMultiplieronMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::gaussMultiplier", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRBloomBlurMean", "onMouseDragged", "%this", 1, 103330, false)]
        public void ppOptionsHDRBloomBlurMeanonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::gaussMean", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRBloomBlurStdDev", "onMouseDragged", "%this", 1, 103340, false)]
        public void ppOptionsHDRBloomBlurStdDevonMouseDragged(string thisobj)
            {
            console.SetVar("$HDRPostFX::gaussStdDev", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRBloom", "onAction", "%this", 1, 103350, false)]
        public void ppOptionsHDRBloomonAction(string thisobj)
            {
            console.SetVar("$HDRPostFX::enableBloom", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }


        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDRToneMapping", "onAction", "%this", 1, 103360, false)]
        public void ppOptionsHDRToneMappingonAction(string thisobj)
            {

            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDREffectsBlueShift", "onAction", "%this", 1, 103370, false)]
        public void ppOptionsHDREffectsBlueShiftonAction(string thisobj)
            {
            console.SetVar("$HDRPostFX::enableBlueShift", console.Call(thisobj, "getvalue"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDREffectsBlueShiftColorBlend", "onAction", "%this", 1, 103380, false)]
        public void ppOptionsHDREffectsBlueShiftColorBlendonAction(string thisobj)
            {
            console.SetVar("$HDRPostFX::blueShiftColor", console.GetVarString(thisobj + ".PickColor"));
            console.SetVar(thisobj + ".toolTip", "Color Values : " + console.GetVarString(thisobj + ".PickColor"));
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsHDREffectsBlueShiftColorBaseColor", "onAction", "%this", 1, 103390, false)]
        public void ppOptionsHDREffectsBlueShiftColorBaseColoronAction(string thisobj)
            {
            console.SetVar("ppOptionsHDREffectsBlueShiftColorBlend.baseColor", console.GetVarString(thisobj + ".PickColor"));
            console.SetVar(thisobj + ".toolTip", "Color Values : " + console.GetVarString(thisobj + ".PickColor"));
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsLightRaysBrightScalar", "onMouseDragged", "%this", 1, 103400, false)]
        public void ppOptionsLightRaysBrightScalaronMouseDragged(string thisobj)
            {
            console.SetVar("$LightRayPostFX::brightScalar", console.GetVarString(thisobj + ".value"));
            console.SetVar(thisobj + ".toolTip", "Value : " + console.GetVarString(thisobj + ".value"));
            }

        [Torque_Decorations.TorqueCallBack("", "", "ppOptionsUpdateDOFSettings", "", 0, 103410, false)]
        public void ppOptionsUpdateDOFSettings()
            {
            DOFPostEffectsetFocusParams("DOFPostEffect", console.GetVarFloat("$DOFPostFx::BlurMin"),
                console.GetVarFloat("$DOFPostFx::BlurMax"), console.GetVarFloat("$DOFPostFx::FocusRangeMin"), console.GetVarFloat("$DOFPostFx::FocusRangeMax"),
                (console.GetVarFloat("$DOFPostFx::BlurCurveNear") * -1), console.GetVarFloat("$DOFPostFx::BlurCurveFar"));
            DOFPostEffectsetAutoFocus("DOFPostEffect", console.GetVarBool("$DOFPostFx::EnableAutoFocus"));
            DOFPostEffectsetFocalDist("DOFPostEffect", "0");

            console.Call("DOFPostEffect", console.GetVarBool("$PostFXManager::PostFX::EnableDOF") ? "enable" : "disable");
            }



        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFEnableDOF", "onAction", "%this", 1, 103420, false)]
        public void ppOptionsDOFEnableDOFonAction(string thisobj)
            {
            console.SetVar("$PostFXManager::PostFX::EnableDOF", console.Call(thisobj, "getValue"));
            ppOptionsUpdateDOFSettings();

            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFEnableAutoFocus", "onAction", "%this", 1, 103430, false)]
        public void ppOptionsDOFEnableAutoFocusonAction(string thisobj)
            {
            console.SetVar("$DOFPostFx::EnableAutoFocus", console.Call(thisobj, "getValue"));
            DOFPostEffectsetAutoFocus("DOFPostEffect", console.Call(thisobj, "getValue").AsBool());
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFFarBlurMinSlider", "onMouseDragged", "%this", 1, 103440, false)]
        public void ppOptionsDOFFarBlurMinSlideronMouseDragged(string thisobj)
            {
            console.SetVar("$DOFPostFx::BlurMin", console.GetVarString(thisobj + ".value"));
            ppOptionsUpdateDOFSettings();
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFFarBlurMaxSlider", "onMouseDragged", "%this", 1, 103450, false)]
        public void ppOptionsDOFFarBlurMaxSlideronMouseDragged(string thisobj)
            {
            console.SetVar("$DOFPostFx::BlurMax", console.GetVarString(thisobj + ".value"));
            ppOptionsUpdateDOFSettings();
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFFocusRangeMinSlider", "onMouseDragged", "%this", 1, 103460, false)]
        public void ppOptionsDOFFocusRangeMinSlideronMouseDragged(string thisobj)
            {
            console.SetVar("$DOFPostFx::FocusRangeMin", console.GetVarString(thisobj + ".value"));
            ppOptionsUpdateDOFSettings();
            }

        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFFocusRangeMaxSlider", "onMouseDragged", "%this", 1, 103470, false)]
        public void ppOptionsDOFFocusRangeMaxSlideronMouseDragged(string thisobj)
            {
            console.SetVar("$DOFPostFx::FocusRangeMax", console.GetVarString(thisobj + ".value"));
            ppOptionsUpdateDOFSettings();
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFBlurCurveNearSlider", "onMouseDragged", "%this", 1, 103480, false)]
        public void ppOptionsDOFBlurCurveNearSlideronMouseDragged(string thisobj)
            {
            console.SetVar("$DOFPostFx::BlurCurveNear", console.GetVarString(thisobj + ".value"));
            ppOptionsUpdateDOFSettings();
            }
        [Torque_Decorations.TorqueCallBack("", "ppOptionsDOFBlurCurveFarSlider", "onMouseDragged", "%this", 1, 103490, false)]
        public void ppOptionsDOFBlurCurveFarSlideronMouseDragged(string thisobj)
            {
            console.SetVar("$DOFPostFx::BlurCurveFar", console.GetVarString(thisobj + ".value"));
            ppOptionsUpdateDOFSettings();
            }


        [Torque_Decorations.TorqueCallBack("", "ppOptionsEnableHDRDebug", "onAction", "%this", 1, 103500, false)]
        public void ppOptionsEnableHDRDebugonAction(string thisobj)
            {
            console.Call("LuminanceVisPostFX", console.Call(thisobj, "getValue").AsBool() ? "enable" : "disable");
            }

        [Torque_Decorations.TorqueCallBack("", "", "ppColorCorrection_selectFile", "", 0, 103510, false)]
        public void ppColorCorrection_selectFile()
            {
            string filter = "Image Files (*.png, *.jpg, *.dds, *.bmp, *.gif, *.jng. *.tga)|*.png;*.jpg;*.dds;*.bmp;*.gif;*.jng;*.tga|All Files (*.*)|*.*|";
            console.Call("getLoadFilename", new string[] { filter, "ppColorCorrection_selectFileHandler" });
            }




       

        [Torque_Decorations.TorqueCallBack("", "", "ppColorCorrection_selectFileHandler", "%filename", 1, 103520, false)]
        public void ppColorCorrection_selectFileHandler(string filename)
            {
            if (filename == "" || !Util.isFile(filename))
                filename = default_color_correction;
            else
                filename = Util.makeRelativePath(filename, Util.getMainDotCsDir());

            console.SetVar("$HDRPostFX::colorCorrectionRamp", filename);

            console.SetVar(SimSet.findObjectByInternalName("PostFXManager", "ColorCorrectionFileName", true) + ".text", filename);


            }
        }
    }
