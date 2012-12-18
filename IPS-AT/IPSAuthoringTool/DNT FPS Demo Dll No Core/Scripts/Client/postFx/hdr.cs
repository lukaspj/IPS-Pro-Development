﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
using System.Threading;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {

        [Torque_Decorations.TorqueCallBack("", "", "hdr_Init", "", 0, 99000, true)]
        public void hdr_Init()
            {
            // Blends between the scene and the tone mapped scene.
            //$HDRPostFX::enableToneMapping = 1.0;
            console.SetVar("$HDRPostFX::enableToneMapping", "1.0");

            // The tone mapping middle grey or exposure value used
            // to adjust the overall "balance" of the image.
            //
            // 0.18 is fairly common value.
            //
            //$HDRPostFX::keyValue = 0.18;
            console.SetVar("$HDRPostFX::keyValue", "0.18");

            // The minimum luninace value to allow when tone mapping 
            // the scene.  Is particularly useful if your scene very 
            // dark or has a black ambient color in places.
            //$HDRPostFX::minLuminace = 0.001;
            console.SetVar("$HDRPostFX::minLuminace", "0.001");

            // The lowest luminance value which is mapped to white.  This
            // is usually set to the highest visible luminance in your 
            // scene.  By setting this to smaller values you get a contrast
            // enhancement.
            //$HDRPostFX::whiteCutoff = 1.0;
            console.SetVar("$HDRPostFX::whiteCutoff", "1.0");

            // The rate of adaptation from the previous and new 
            // average scene luminance. 
            //$HDRPostFX::adaptRate = 2.0;
            console.SetVar("$HDRPostFX::adaptRate", "2.0");


            // Blends between the scene and the blue shifted version
            // of the scene for a cinematic desaturated night effect.

            //$HDRPostFX::enableBlueShift = 0.0;
            console.SetVar("$HDRPostFX::enableBlueShift", "0.0");

            // The blue shift color value.

            //$HDRPostFX::blueShiftColor = "1.05 0.97 1.27";
            console.SetVar("$HDRPostFX::blueShiftColor", "1.05 0.97 1.27");


            // Blends between the scene and the bloomed scene.

            //$HDRPostFX::enableBloom = 1.0;
            console.SetVar("$HDRPostFX::enableBloom", "1.0");

            // The threshold luminace value for pixels which are
            // considered "bright" and need to be bloomed.

            //$HDRPostFX::brightPassThreshold = 1.0;
            console.SetVar("$HDRPostFX::brightPassThreshold", "1.0");

            // These are used in the gaussian blur of the
            // bright pass for the bloom effect.

            //$HDRPostFX::gaussMultiplier = 0.3;
            console.SetVar("$HDRPostFX::gaussMultiplier", "0.3");

            //$HDRPostFX::gaussMean = 0.0;
            console.SetVar("$HDRPostFX::gaussMean", "0.0");

            //$HDRPostFX::gaussStdDev = 0.8;
            console.SetVar("$HDRPostFX::gaussStdDev", "0.8");

            // The 1x255 color correction ramp texture used
            // by both the HDR shader and the GammaPostFx shader
            // for doing full screen color correction. 

            //$HDRPostFX::colorCorrectionRamp = "core/scripts/client/postFx/null_color_ramp.png";
            console.SetVar("$HDRPostFX::colorCorrectionRamp", default_color_correction);//"core/scripts/client/postFx/null_color_ramp.png");


            /*
             *singleton ShaderData( HDR_BrightPassShader )
            {
               DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
               DXPixelShaderFile 	= "shaders/common/postFx/hdr/brightPassFilterP.hlsl";
               pixVersion = 3.0;
            };
             */
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "HDR_BrightPassShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/brightPassFilterP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);
            /*
            singleton ShaderData( HDR_DownScale4x4Shader )
            {
               DXVertexShaderFile 	= "shaders/common/postFx/hdr/downScale4x4V.hlsl";
               DXPixelShaderFile 	= "shaders/common/postFx/hdr/downScale4x4P.hlsl";
               pixVersion = 2.0;
            };

            */
            ts = new TorqueSingleton("ShaderData", "HDR_DownScale4x4Shader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/hdr/downScale4x4V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/downScale4x4P.hlsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);


            /*
            singleton ShaderData( HDR_BloomGaussBlurHShader )
            {
               DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
               DXPixelShaderFile 	= "shaders/common/postFx/hdr/bloomGaussBlurHP.hlsl";
               pixVersion = 3.0;
            };
            */
            ts = new TorqueSingleton("ShaderData", "HDR_BloomGaussBlurHShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/bloomGaussBlurHP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            /*
              singleton ShaderData( HDR_BloomGaussBlurVShader )
                {
                   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
                   DXPixelShaderFile 	= "shaders/common/postFx/hdr/bloomGaussBlurVP.hlsl";
                   pixVersion = 3.0;
                };
            */
            ts = new TorqueSingleton("ShaderData", "HDR_BloomGaussBlurVShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/bloomGaussBlurVP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            /*
            
            singleton ShaderData( HDR_SampleLumShader )
            {
               DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
               DXPixelShaderFile 	= "shaders/common/postFx/hdr/sampleLumInitialP.hlsl";
               pixVersion = 3.0;
            };
             */

            ts = new TorqueSingleton("ShaderData", "HDR_SampleLumShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/sampleLumInitialP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            /*singleton ShaderData( HDR_DownSampleLumShader )
{
   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
   DXPixelShaderFile 	= "shaders/common/postFx/hdr/sampleLumIterativeP.hlsl";
   pixVersion = 3.0;
};*/

            ts = new TorqueSingleton("ShaderData", "HDR_DownSampleLumShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/sampleLumIterativeP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            /*
             * singleton ShaderData( HDR_CalcAdaptedLumShader )
{
   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
   DXPixelShaderFile 	= "shaders/common/postFx/hdr/calculateAdaptedLumP.hlsl";
   pixVersion = 3.0;
};
             */
            ts = new TorqueSingleton("ShaderData", "HDR_CalcAdaptedLumShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/calculateAdaptedLumP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            /*
             * singleton ShaderData( HDR_CombineShader )
{
   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
   DXPixelShaderFile 	= "shaders/common/postFx/hdr/finalPassCombineP.hlsl";
   pixVersion = 3.0;
};
             */
            ts = new TorqueSingleton("ShaderData", "HDR_CombineShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/hdr/finalPassCombineP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            /*
             * singleton GFXStateBlockData( HDR_SampleStateBlock : PFX_DefaultStateBlock )
{
   samplersDefined = true;
   samplerStates[0] = SamplerClampPoint;
   samplerStates[1] = SamplerClampPoint;
};
             */


            ts = new TorqueSingleton("GFXStateBlockData", "HDR_SampleStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[1]", "SamplerClampPoint");
            ts.Create(m_ts);

            /*
             * singleton GFXStateBlockData( HDR_DownSampleStateBlock : PFX_DefaultStateBlock )
{
   samplersDefined = true;
   samplerStates[0] = SamplerClampLinear;
   samplerStates[1] = SamplerClampLinear;
};
             * */


            ts = new TorqueSingleton("GFXStateBlockData", "HDR_DownSampleStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Create(m_ts);

            /*
             * singleton GFXStateBlockData( HDR_CombineStateBlock : PFX_DefaultStateBlock )
{
   samplersDefined = true;
   samplerStates[0] = SamplerClampPoint;
   samplerStates[1] = SamplerClampLinear;
   samplerStates[2] = SamplerClampLinear;
   samplerStates[3] = SamplerClampLinear;
};
             */

            ts = new TorqueSingleton("GFXStateBlockData", "HDR_CombineStateBlock : PFX_DefaultStateBlock");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[2]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[3]", "SamplerClampLinear");
            ts.Create(m_ts);

            /*
             * singleton GFXStateBlockData( HDRStateBlock )
{
   samplersDefined = true;
   samplerStates[0] = SamplerClampLinear;
   samplerStates[1] = SamplerClampLinear;
   samplerStates[2] = SamplerClampLinear;
   samplerStates[3] = SamplerClampLinear;
   
   blendDefined = true;
   blendDest = GFXBlendOne;
   blendSrc = GFXBlendZero;
   
   zDefined = true;
   zEnable = false;
   zWriteEnable = false;
   
   cullDefined = true;
   cullMode = GFXCullNone;
};
             * */

            ts = new TorqueSingleton("GFXStateBlockData", "HDRStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[2]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[3]", "SamplerClampLinear");

            ts.Props.Add("blendDefined", "true");
            ts.Props.Add("blendDest", "GFXBlendOne");
            ts.Props.Add("blendSrc", "GFXBlendZero");

            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("cullDefined", "true");
            ts.Props.Add("cullMode", "GFXCullNone");
            ts.Create(m_ts);
            }

        [Torque_Decorations.TorqueCallBack("", "HDRPostFX", "setShaderConsts", "%this", 1, 99100, false)]
        public void HDRPostFXsetShaderConsts(string thisobj)
            {
            //%this.setShaderConst( "$brightPassThreshold", $HDRPostFX::brightPassThreshold );
            PostEffect.setShaderConst(thisobj, "$brightPassThreshold", console.GetVarString("$HDRPostFX::brightPassThreshold"));

            //%this.setShaderConst( "$g_fMiddleGray", $HDRPostFX::keyValue );   
            PostEffect.setShaderConst(thisobj, "$g_fMiddleGray", console.GetVarString("$HDRPostFX::keyValue"));

            //%bloomH = %this-->bloomH;
            string bloomh = SimSet.findObjectByInternalName(thisobj, "bloomH", true);

            //%bloomH.setShaderConst( "$gaussMultiplier", $HDRPostFX::gaussMultiplier );
            PostEffect.setShaderConst(bloomh, "$gaussMultiplier", console.GetVarString("$HDRPostFX::gaussMultiplier"));

            //%bloomH.setShaderConst( "$gaussMean", $HDRPostFX::gaussMean );
            PostEffect.setShaderConst(bloomh, "$gaussMean", console.GetVarString("$HDRPostFX::gaussMean"));

            //%bloomH.setShaderConst( "$gaussStdDev", $HDRPostFX::gaussStdDev );
            PostEffect.setShaderConst(bloomh, "$gaussStdDev", console.GetVarString("$HDRPostFX::gaussStdDev"));

            //%bloomV = %this-->bloomV;
            string bloomV = SimSet.findObjectByInternalName(thisobj, "bloomV", true);

            //%bloomV.setShaderConst( "$gaussMultiplier", $HDRPostFX::gaussMultiplier );
            PostEffect.setShaderConst(bloomV, "$gaussMultiplier", console.GetVarString("$HDRPostFX::gaussMultiplier"));

            //%bloomV.setShaderConst( "$gaussMean", $HDRPostFX::gaussMean );
            PostEffect.setShaderConst(bloomV, "$gaussMean", console.GetVarString("$HDRPostFX::gaussMean"));

            //%bloomV.setShaderConst( "$gaussStdDev", $HDRPostFX::gaussStdDev );  
            PostEffect.setShaderConst(bloomV, "$gaussStdDev", console.GetVarString("$HDRPostFX::gaussStdDev"));

            //%minLuminace = $HDRPostFX::minLuminace;
            double minLuminace = console.GetVarDouble("$HDRPostFX::minLuminace");

            /*
   if ( %minLuminace <= 0.0 )
   {
      // The min should never be pure zero else the
      // log() in the shader will generate INFs.      
      %minLuminace = 0.00001;
   }             
             */

            if (minLuminace <= 0.0)
                {
                minLuminace = (double)0.00001;
                }

            //%this-->adaptLum.setShaderConst( "$g_fMinLuminace", %minLuminace );
            PostEffect.setShaderConst(SimSet.findObjectByInternalName(thisobj, "adaptLum", true), "$g_fMinLuminace", minLuminace.AsString());

            //%this-->finalLum.setShaderConst( "$adaptRate", $HDRPostFX::adaptRate );
            PostEffect.setShaderConst(SimSet.findObjectByInternalName(thisobj, "finalLum", true), "$adaptRate", console.GetVarString("$HDRPostFX::adaptRate"));

            //%combinePass = %this-->combinePass;
            string combinePass = SimSet.findObjectByInternalName(thisobj, "combinePass", true);

            //%combinePass.setShaderConst( "$g_fEnableToneMapping", $HDRPostFX::enableToneMapping );
            PostEffect.setShaderConst(combinePass, "$g_fEnableToneMapping", console.GetVarString("$HDRPostFX::enableToneMapping"));

            //%combinePass.setShaderConst( "$g_fMiddleGray", $HDRPostFX::keyValue );
            PostEffect.setShaderConst(combinePass, "$g_fMiddleGray", console.GetVarString("$HDRPostFX::keyValue"));

            //%combinePass.setShaderConst( "$g_fBloomScale", $HDRPostFX::enableBloom );      
            PostEffect.setShaderConst(combinePass, "$g_fBloomScale", console.GetVarString("$HDRPostFX::enableBloom"));

            //%combinePass.setShaderConst( "$g_fEnableBlueShift", $HDRPostFX::enableBlueShift );
            PostEffect.setShaderConst(combinePass, "$g_fEnableBlueShift", console.GetVarString("$HDRPostFX::enableBlueShift"));

            //%combinePass.setShaderConst( "$g_fBlueShiftColor", $HDRPostFX::blueShiftColor );
            PostEffect.setShaderConst(combinePass, "$g_fBlueShiftColor", console.GetVarString("$HDRPostFX::blueShiftColor"));

            //%clampedGamma  = mClamp( $pref::Video::Gamma, 0.001, 2.2);
            float clampedGamma = Util.mClamp(console.GetVarFloat("$pref::Video::Gamma"), (float)0.001, (float)2.2);

            //%combinePass.setShaderConst( "$g_fOneOverGamma",  1 / %clampedGamma ); 
            PostEffect.setShaderConst(combinePass, "$g_fOneOverGamma", (1 / clampedGamma).AsString());

            //%whiteCutoff = ( $HDRPostFX::whiteCutoff * $HDRPostFX::whiteCutoff ) * ( $HDRPostFX::whiteCutoff * $HDRPostFX::whiteCutoff );                  
            float whiteCutoff = console.GetVarFloat("$HDRPostFX::whiteCutoff") * console.GetVarFloat("$HDRPostFX::whiteCutoff") * console.GetVarFloat("$HDRPostFX::whiteCutoff") *
                                console.GetVarFloat("$HDRPostFX::whiteCutoff");

            //%combinePass.setShaderConst( "$g_fWhiteCutoff", %whiteCutoff );
            PostEffect.setShaderConst(combinePass, "$g_fWhiteCutoff", whiteCutoff.AsString());
            }

        [Torque_Decorations.TorqueCallBack("", "HDRPostFX", "preProcess", "%this", 1, 99200, false)]
        public void HDRPostFXpreProcess(string thisobj)
            {
            //%combinePass = %this-->combinePass;
            string combinePass = SimSet.findObjectByInternalName(thisobj, "combinePass", true);

            //if ( %combinePass.texture[3] !$= $HDRPostFX::colorCorrectionRamp )
            if (console.GetVarString(combinePass + ".texture[3]") != console.GetVarString("$HDRPostFX::colorCorrectionRamp"))
                //%combinePass.setTexture( 3, $HDRPostFX::colorCorrectionRamp );  
                PostEffect.setTexture(combinePass, 3, console.GetVarString("$HDRPostFX::colorCorrectionRamp"));
            //Thread.Sleep(5);
            }

        [Torque_Decorations.TorqueCallBack("", "HDRPostFX", "onEnabled", "%this", 1, 99300, false)]
        public bool HDRPostFXonEnabled(string thisobj)
            {
            // We don't allow hdr on OSX yet.
            if (console.GetVarString("$platform") == "macos")
                return false;
            // See what HDR format would be best.
            //%format = getBestHDRFormat();
            GFXFormat format = Util.getBestHDRFormat();
            if ((format == GFXFormat.GFXFormat_UNKNOWNSIZE) || (format == GFXFormat.GFXFormatR8G8B8A8))
                // We didn't get a valid HDR format... so fail.
                return false;
            // HDR does it's own gamma calculation so 
            // disable this postFx.
            console.Call("GammaPostFX", "disable");

            // Set the right global shader define for HDR.
            switch (format)
                {
                case GFXFormat.GFXFormatR10G10B10A2:
                    Util.addGlobalShaderMacro("TORQUE_HDR_RGB10", "");
                    break;
                case GFXFormat.GFXFormatR16G16B16A16:
                    Util.addGlobalShaderMacro("TORQUE_HDR_RGB16", "");
                    break;
                }

            console.print("HDR FORMAT: " + format.ToString());
            // Change the format of the offscreen surface
            // to an HDR compatible format.

            console.SetVar("AL_FormatToken.format", format.ToString());

            Util.setReflectFormat(format);
            // Reset the light manager which will ensure the new
            // hdr encoding takes effect in all the shaders and
            // that the offscreen surface is enabled.
            Util.resetLightManager();

            return true;
            }

        [Torque_Decorations.TorqueCallBack("", "HDRPostFX", "onDisabled", "%this", 1, 99400, false)]
        public void HDRPostFXonDisabled(string thisobj)
            {
            // Enable a special GammaCorrection PostFX when this is disabled.
            //GammaPostFX.enable();
            console.Call("GammaPostFX", "enable");

            //%format = "GFXFormatR8G8B8A8";
            //AL_FormatToken.format = %format;
            console.SetVar("AL_FormatToken.format", "GFXFormatR8G8B8A8");

            // Restore the non-HDR offscreen surface format.
            //setReflectFormat( %format );
            Util.setReflectFormat(GFXFormat.GFXFormatR8G8B8A8);

            //removeGlobalShaderMacro( "TORQUE_HDR_RGB10" );
            Util.removeGlobalShaderMacro("TORQUE_HDR_RGB10");

            //removeGlobalShaderMacro( "TORQUE_HDR_RGB16" );
            Util.removeGlobalShaderMacro("TORQUE_HDR_RGB16");

            //resetLightManager();
            Util.resetLightManager();
            }


        [Torque_Decorations.TorqueCallBack("", "", "hdr_Init7", "", 0, 99501, true)]
        public void hdr_Init7()
            {
            console.Eval(
                @"
singleton PostEffect( HDRPostFX )
{
   isEnabled = false;
   allowReflectPass = false;
   renderTime = ""PFXBeforeBin"";
   renderBin = ""EditorBin"";
   renderPriority = 9999;
   shader = HDR_BrightPassShader;
   stateBlock = HDR_DownSampleStateBlock;
   texture[0] = ""$backBuffer"";
   texture[1] = ""#adaptedLum"";
   target = ""$outTex"";
   targetFormat = ""GFXFormatR16G16B16A16F""; 
   targetScale = ""0.5 0.5"";
   new PostEffect()
      {
         shader = HDR_DownScale4x4Shader;
         stateBlock = HDR_DownSampleStateBlock;
         texture[0] = ""$inTex"";
         target = ""$outTex"";
         targetFormat = ""GFXFormatR16G16B16A16F"";
         targetScale = ""0.25 0.25"";
      };
   new PostEffect()
      {
         internalName = ""bloomH"";
         shader = HDR_BloomGaussBlurHShader;
         stateBlock = HDR_DownSampleStateBlock;
         texture[0] = ""$inTex"";
         target = ""$outTex"";
         targetFormat = ""GFXFormatR16G16B16A16F"";   
      };
   new PostEffect()
      {
         internalName = ""bloomV"";
         shader = HDR_BloomGaussBlurVShader;
         stateBlock = HDR_DownSampleStateBlock;
         texture[0] = ""$inTex"";
         target = ""#bloomFinal"";
         targetFormat = ""GFXFormatR16G16B16A16F"";    
      };
   new PostEffect()
      {
      internalName = ""adaptLum"";
      shader = HDR_SampleLumShader;
      stateBlock = HDR_DownSampleStateBlock;
      texture[0] = ""$backBuffer"";
      target = ""$outTex"";
      targetScale = ""0.0625 0.0625"";
      targetFormat = ""GFXFormatR16F"";
      new PostEffect()
         {
         shader = HDR_DownSampleLumShader;
         stateBlock = HDR_DownSampleStateBlock;
         texture[0] = ""$inTex"";
         target = ""$outTex"";
         targetScale = ""0.25 0.25"";
         targetFormat = ""GFXFormatR16F"";
         };
      new PostEffect()
         {
         shader = HDR_DownSampleLumShader;
         stateBlock = HDR_DownSampleStateBlock;
         texture[0] = ""$inTex"";
         target = ""$outTex"";
         targetScale = ""0.25 0.25"";
         targetFormat = ""GFXFormatR16F"";
         };
      new PostEffect()
         {
         shader = HDR_DownSampleLumShader;
         stateBlock = HDR_DownSampleStateBlock;
         texture[0] = ""$inTex"";
         target = ""$outTex"";
         targetScale = ""0.25 0.25"";
         targetFormat = ""GFXFormatR16F"";
         };
      new PostEffect()
         {
         internalName = ""finalLum"";         
         shader = HDR_CalcAdaptedLumShader;
         stateBlock = HDR_DownSampleStateBlock;
         texture[0] = ""$inTex"";
         texture[1] = ""#adaptedLum"";
         target = ""#adaptedLum"";
         targetFormat = ""GFXFormatR16F"";
         targetClear = ""PFXTargetClear_OnCreate"";
         targetClearColor = ""1 1 1 1"";
         };
   };
   new PostEffect()
   {
      internalName = ""combinePass"";
      shader = HDR_CombineShader;
      stateBlock = HDR_CombineStateBlock;
      texture[0] = ""$backBuffer"";
      texture[1] = ""#adaptedLum"";            
      texture[2] = ""#bloomFinal"";
      texture[3] = ""$HDRPostFX::colorCorrectionRamp"";" + @"

      target = ""$backBuffer"";
   };
};

");
            }


        [Torque_Decorations.TorqueCallBack("", "LuminanceVisPostFX", "setShaderConsts", "%this", 1, 99505, false)]
        public void LuminanceVisPostFXsetShaderConsts(string thisobj)
            {
            //   %this.setShaderConst( ""$brightPassThreshold"", $HDRPostFX::brightPassThreshold );
            PostEffect.setShaderConst(thisobj, "$brightPassThreshold", console.GetVarString("$HDRPostFX::brightPassThreshold"));
            //Thread.Sleep(5);
            }

        [Torque_Decorations.TorqueCallBack("", "", "hdr_Init_3", "%this", 0, 99502, true)]
        public void hdr_Init_3()
            {
            //singleton PostEffect( LuminanceVisPostFX )
            //{
            //   isEnabled = false;
            //   allowReflectPass = false;

            //   // Render before we do any editor rendering.  
            //   renderTime = ""PFXBeforeBin"";
            //   renderBin = ""EditorBin"";
            //   renderPriority = 9999;

            //   shader = LuminanceVisShader;
            //   stateBlock = LuminanceVisStateBlock;
            //   texture[0] = ""$backBuffer"";
            //   target = ""$backBuffer"";
            //   //targetScale = ""0.0625 0.0625""; // 1/16th
            //   //targetFormat = ""GFXFormatR16F"";
            //};
            TorqueSingleton ts = new TorqueSingleton("PostEffect", "LuminanceVisPostFX");
            ts.Props.Add("isEnabled", "false");
            ts.Props.Add("allowReflectPass", "false");
            ts.PropsAddString("renderTime", "PFXBeforeBin");
            ts.PropsAddString("renderBin", "EditorBin");
            ts.Props.Add("renderPriority", "9999");
            ts.Props.Add("shader", "LuminanceVisShader");
            ts.Props.Add("stateBlock", "LuminanceVisStateBlock");
            ts.PropsAddString("texture[0]", "$backBuffer");
            ts.PropsAddString("target", "$backBuffer");
            ts.Create(m_ts);

            console.SetVar("$HDRPostFX::colorCorrectionRamp", default_color_correction);

            }

        [Torque_Decorations.TorqueCallBack("", "LuminanceVisPostFX", "onEnabled", "%this", 1, 99600, false)]
        public bool LuminanceVisPostFXonEnabled(string thisobj)
            {
            //if ( !HDRPostFX.isEnabled() )
            if (!console.Call("HDRPostFX", "isEnabled").AsBool())
                //HDRPostFX.enable();
                console.Call("HDRPostFX", "enable");

            //HDRPostFX.skip = true;
            console.SetVar("HDRPostFX.skip", true);

            //return true;
            return true;
            }

        [Torque_Decorations.TorqueCallBack("", "LuminanceVisPostFX", "onDisabled", "%this", 1, 99700, false)]
        public void LuminanceVisPostFXonDisabled(string thisobj)
            {

            //HDRPostFX.skip = false; 
            console.SetVar("HDRPostFX.skip", false);
            }
     
        }
    }
