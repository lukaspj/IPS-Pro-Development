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
        [Torque_Decorations.TorqueCallBack("", "", "initialize_SSAO", "", 0, 106000, true)]
        public void initialize_SSAO()
            {
            console.SetVar("$SSAOPostFx::overallStrength", "2.0");

            //TODO:Addsmall/largeparamdocs.

            //ThesmallradiusSSAOsettings.
            console.SetVar("$SSAOPostFx::sRadius", "0.1");
            console.SetVar("$SSAOPostFx::sStrength", "6.0");
            console.SetVar("$SSAOPostFx::sDepthMin", "0.1");
            console.SetVar("$SSAOPostFx::sDepthMax", "1.0");
            console.SetVar("$SSAOPostFx::sDepthPow", "1.0");
            console.SetVar("$SSAOPostFx::sNormalTol", "0.0");
            console.SetVar("$SSAOPostFx::sNormalPow", "1.0");

            //ThelargeradiusSSAOsettings.
            console.SetVar("$SSAOPostFx::lRadius", "1.0");
            console.SetVar("$SSAOPostFx::lStrength", "10.0");
            console.SetVar("$SSAOPostFx::lDepthMin", "0.2");
            console.SetVar("$SSAOPostFx::lDepthMax", "2.0");
            console.SetVar("$SSAOPostFx::lDepthPow", "0.2");
            console.SetVar("$SSAOPostFx::lNormalTol", "-0.5");
            console.SetVar("$SSAOPostFx::lNormalPow", "2.0");

            //Validvalues:0,1,2
            console.SetVar("$SSAOPostFx::quality", "0");

            //
            console.SetVar("$SSAOPostFx::blurDepthTol", "0.001");

            //
            console.SetVar("$SSAOPostFx::blurNormalTol", "0.95");

            //
            console.SetVar("$SSAOPostFx::targetScale", "0.50.5");
            }
        [Torque_Decorations.TorqueCallBack("", "SSAOPostFx", "onAdd", "%this", 1, 106010, false)]
        public void SSAOPostFxonAdd(string thisobj)
            {
            console.SetVar(thisobj + ".wasVis", "Uninitialized");
            console.SetVar(thisobj + ".quality", "Uninitialized");
            }
        [Torque_Decorations.TorqueCallBack("", "SSAOPostFx", "preProcess", "%this", 1, 106015, false)]
        public void SSAOPostFxpreProcess(string thisobj)
            {
            if (console.GetVarString("$SSAOPostFx::quality") != console.GetVarString(thisobj + ".quality"))
                {
                console.SetVar(thisobj + ".quality", Util.mClamp(Util.mRound(console.GetVarFloat("$SSAOPostFx::quality")), 0, 2));
                PostEffect.setShaderMacro(thisobj, "QUALITY", console.GetVarString(thisobj + ".quality"));
                }
            console.SetVar(thisobj + ".targetScale", console.GetVarString("$SSAOPostFx::targetScale"));

            }


        [Torque_Decorations.TorqueCallBack("", "SSAOPostFx", "setShaderConsts", "%this", 1, 106020, false)]
        public void SSAOPostFxsetShaderConsts(string thisobj)
            {
            PostEffect.setShaderConst(thisobj, "$sRadius", console.GetVarString("$SSAOPostFx::sRadius"));
            PostEffect.setShaderConst(thisobj, "$sStrength", console.GetVarString("$SSAOPostFx::sStrength"));
            PostEffect.setShaderConst(thisobj, "$sDepthMin", console.GetVarString("$SSAOPostFx::sDepthMin"));
            PostEffect.setShaderConst(thisobj, "$sDepthMax", console.GetVarString("$SSAOPostFx::sDepthMax"));
            PostEffect.setShaderConst(thisobj, "$sDepthPow", console.GetVarString("$SSAOPostFx::sDepthPow"));
            PostEffect.setShaderConst(thisobj, "$sNormalTol", console.GetVarString("$SSAOPostFx::sNormalTol"));
            PostEffect.setShaderConst(thisobj, "$sNormalPow", console.GetVarString("$SSAOPostFx::sNormalPow"));
            PostEffect.setShaderConst(thisobj, "$lRadius", console.GetVarString("$SSAOPostFx::lRadius"));
            PostEffect.setShaderConst(thisobj, "$lStrength", console.GetVarString("$SSAOPostFx::lStrength"));
            PostEffect.setShaderConst(thisobj, "$lDepthMin", console.GetVarString("$SSAOPostFx::lDepthMin"));
            PostEffect.setShaderConst(thisobj, "$lDepthMax", console.GetVarString("$SSAOPostFx::lDepthMax"));
            PostEffect.setShaderConst(thisobj, "$lDepthPow", console.GetVarString("$SSAOPostFx::lDepthPow"));
            PostEffect.setShaderConst(thisobj, "$lNormalTol", console.GetVarString("$SSAOPostFx::lNormalTol"));
            PostEffect.setShaderConst(thisobj, "$lNormalPow", console.GetVarString("$SSAOPostFx::lNormalPow"));
            string blur = SimSet.findObjectByInternalName(thisobj, "blurY", false);
            PostEffect.setShaderConst(blur, "$blurDepthTol", console.GetVarString("$SSAOPostFx::blurDepthTol"));
            PostEffect.setShaderConst(blur, "$blurNormalTol", console.GetVarString("$SSAOPostFx::blurNormalTol"));
            blur = SimSet.findObjectByInternalName(thisobj, "blurX", false);
            PostEffect.setShaderConst(blur, "$blurDepthTol", console.GetVarString("$SSAOPostFx::blurDepthTol"));
            PostEffect.setShaderConst(blur, "$blurNormalTol", console.GetVarString("$SSAOPostFx::blurNormalTol"));
            blur = SimSet.findObjectByInternalName(thisobj, "blurY2", false);
            PostEffect.setShaderConst(blur, "$blurDepthTol", console.GetVarString("$SSAOPostFx::blurDepthTol"));
            PostEffect.setShaderConst(blur, "$blurNormalTol", console.GetVarString("$SSAOPostFx::blurNormalTol"));
            blur = SimSet.findObjectByInternalName(thisobj, "blurX2", false);
            PostEffect.setShaderConst(blur, "$blurDepthTol", console.GetVarString("$SSAOPostFx::blurDepthTol"));
            PostEffect.setShaderConst(blur, "$blurNormalTol", console.GetVarString("$SSAOPostFx::blurNormalTol"));
            }
        [Torque_Decorations.TorqueCallBack("", "SSAOPostFx", "onEnabled", "%this", 1, 106030, false)]
        public bool SSAOPostFxonEnabled(string thisobj)
            {
            // This tells the AL shaders to reload and sample
            // from our #ssaoMask texture target. 
            console.SetVar("$AL::UseSSAOMask", true);
            return true;
            }
        [Torque_Decorations.TorqueCallBack("", "SSAOPostFx", "onDisabled", "%this", 1, 106040, false)]
        public bool SSAOPostFxonDisabled(string thisobj)
            {
            console.SetVar("$AL::UseSSAOMask", false);
            return false;
            }
        [Torque_Decorations.TorqueCallBack("", "", "initialize_SSAO1", "", 0, 106050, true)]

        public void initialize_SSAO1()
            {

            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "SSAOStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[1]", "SamplerWrapLinear");
            ts.Props.Add("samplerStates[2]", "SamplerClampPoint");
            ts.Create(m_ts);


            ts = new TorqueSingleton("GFXStateBlockData", "SSAOBlurStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampPoint");
            ts.Create(m_ts);


            ts = new TorqueSingleton("ShaderData", "SSAOShader");

            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/ssao/SSAO_P.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);


            ts = new TorqueSingleton("ShaderData", "SSAOBlurYShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/ssao/SSAO_Blur_V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/ssao/SSAO_Blur_P.hlsl");
            ts.Props.Add("pixVersion", "3.0");

            ts.PropsAddString("defines", "BLUR_DIR=float2(0.0,1.0)");
            ts.Create(m_ts);


            ts = new TorqueSingleton("ShaderData", "SSAOBlurXShader : SSAOBlurYShader");
            ts.PropsAddString("defines", "BLUR_DIR=float2(1.0,0.0)");
            ts.Create(m_ts);

            console.Eval(
                @"
singleton PostEffect( SSAOPostFx )
{     
   allowReflectPass = false;
     
   renderTime = ""PFXBeforeBin"";
   renderBin = ""AL_LightBinMgr"";   
   renderPriority = 10;
   
   shader = SSAOShader;
   stateBlock = SSAOStateBlock;
         
   texture[0] = ""#prepass"";         
   texture[1] = ""$noise_png"";
   texture[2] = ""#ssao_pow_table"";
   
   target = ""$outTex"";
   targetScale = ""0.5 0.5"";
   
   singleton PostEffect()
   {
      internalName = ""blurY"";
      
      shader = SSAOBlurYShader;
      stateBlock = SSAOBlurStateBlock;
      
      texture[0] = ""$inTex"";
      texture[1] = ""#prepass"";
      
      target = ""$outTex""; 
   };
      
   singleton PostEffect()
   {
      internalName = ""blurX"";
      
      shader = SSAOBlurXShader;
      stateBlock = SSAOBlurStateBlock;
      
      texture[0] = ""$inTex"";
      texture[1] = ""#prepass"";
      
      target = ""$outTex""; 
   };   
   
   singleton PostEffect()
   {
      internalName = ""blurY2"";
      
      shader = SSAOBlurYShader;
      stateBlock = SSAOBlurStateBlock;
            
      texture[0] = ""$inTex"";
      texture[1] = ""#prepass"";
      
      target = ""$outTex""; 
   };
   
   singleton PostEffect()
   {
      internalName = ""blurX2"";
            
      shader = SSAOBlurXShader;
      stateBlock = SSAOBlurStateBlock;
            
      texture[0] = ""$inTex"";
      texture[1] = ""#prepass"";

      target = ""#ssaoMask"";   
   };  
};


");

            ts = new TorqueSingleton("PostEffect", "SSAOVizPostFx");
            ts.Props.Add("allowReflectPass", "false");
            ts.Props.Add("shader", "PFX_PassthruShader");
            ts.Props.Add("stateBlock", "PFX_DefaultStateBlock");
            ts.PropsAddString("texture[0]", "#ssaoMask");
            ts.PropsAddString("target", "$backbuffer");
            ts.Create(m_ts);


            ts = new TorqueSingleton("ShaderData", "SSAOPowTableShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/ssao/SSAO_PowerTable_V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/ssao/SSAO_PowerTable_P.hlsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "SSAOPowTablePostFx");
            ts.Props.Add("shader", "SSAOPowTableShader");
            ts.Props.Add("stateBlock", "PFX_DefaultStateBlock");

            ts.PropsAddString("renderTime", "PFXTexGenOnDemand");

            ts.PropsAddString("target", "#ssao_pow_table");

            ts.PropsAddString("targetFormat", "GFXFormatR16F");
            ts.PropsAddString("targetSize", "256 1");
            ts.Create(m_ts);

            console.SetVar("$noise_png",noise_png);
            }
        }
    }