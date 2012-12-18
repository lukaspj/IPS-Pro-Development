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

        /*

        ================================================================================
         The DOFPostEffect API
        ================================================================================

        DOFPostEffect::setFocalDist( %dist )

        @summary
        This method is for manually controlling the focus distance. It will have no 
        effect if auto focus is currently enabled. Makes use of the parameters set by 
        setFocusParams.

        @param dist 
        float distance in meters

        --------------------------------------------------------------------------------

        DOFPostEffect::setAutoFocus( %enabled )

        @summary
        This method sets auto focus enabled or disabled. Makes use of the parameters set 
        by setFocusParams. When auto focus is enabled it determines the focal depth
        by performing a raycast at the screen-center.

        @param enabled
        bool

        --------------------------------------------------------------------------------

        DOFPostEffect::setFocusParams( %nearBlurMax, %farBlurMax, %minRange, %maxRange, %nearSlope, %farSlope )

        Set the parameters that control how the near and far equations are calculated
        from the focal distance. If you are not using auto focus you will need to call
        setFocusParams PRIOR to calling setFocalDist.

        @param nearBlurMax   
        float between 0.0 and 1.0
        The max allowed value of near blur.

        @param farBlurMax    
        float between 0.0 and 1.0
        The max allowed value of far blur.

        @param minRange/maxRange 
        float in meters
        The distance range around the focal distance that remains in focus is a lerp 
        between the min/maxRange using the normalized focal distance as the parameter. 
        The point is to allow the focal range to expand as you focus farther away since this is 
        visually appealing.

        Note: since min/maxRange are lerped by the "normalized" focal distance it is
        dependant on the visible distance set in your level.

        @param nearSlope
        float less than zero
        The slope of the near equation. A small number causes bluriness to increase gradually
        at distances closer than the focal distance. A large number causes bluriness to 
        increase quickly.

        @param farSlope
        float greater than zero
        The slope of the far equation. A small number causes bluriness to increase gradually
        at distances farther than the focal distance. A large number causes bluriness to
        increase quickly.

        Note: To rephrase, the min/maxRange parameters control how much area around the
        focal distance is completely in focus where the near/farSlope parameters control
        how quickly or slowly bluriness increases at distances outside of that range.

        ================================================================================
         Examples
        ================================================================================

        Example1: Turn on DOF while zoomed in with a weapon.

        NOTE: These are not real callbacks! Hook these up to your code where appropriate!

        function onSniperZoom()
        {
           // Parameterize how you want DOF to look.
           DOFPostEffect.setFocusParams( 0.3, 0.3, 50, 500, -5, 5 );
   
           // Turn on auto focus
           DOFPostEffect.setAutoFocus( true );
   
           // Turn on the PostEffect
           DOFPostEffect.enable();
        }

        function onSniperUnzoom()
        {
           // Turn off the PostEffect
           DOFPostEffect.disable();
        }

        Example2: Manually control DOF with the mouse wheel.

        // Somewhere on startup...

        // Parameterize how you want DOF to look.
        DOFPostEffect.setFocusParams( 0.3, 0.3, 50, 500, -5, 5 );

        // Turn off auto focus
        DOFPostEffect.setAutoFocus( false );

        // Turn on the PostEffect
        DOFPostEffect.enable();


        NOTE: These are not real callbacks! Hook these up to your code where appropriate!

        function onMouseWheelUp()
        {
           // Since setFocalDist is really just a wrapper to assign to the focalDist
           // dynamic field we can shortcut and increment it directly.
           DOFPostEffect.focalDist += 8;
        }

        function onMouseWheelDown()
        {
           DOFPostEffect.focalDist -= 8;
        }
        */

        /// This method is for manually controlling the focal distance. It will have no 
        /// effect if auto focus is currently enabled. Makes use of the parameters set by 
        /// setFocusParams.
        [Torque_Decorations.TorqueCallBack("", "DOFPostEffect", "setFocalDist", "%this, %dist", 2, 92000, false)]
        public void DOFPostEffectsetFocalDist(string thisobj, string dist)
            {
            console.SetVar(thisobj + ".focalDist", dist);
            }
        // This method sets auto focus enabled or disabled. Makes use of the parameters set 
        // by setFocusParams. When auto focus is enabled it determine the focal depth
        // by performing a raycast at the screen-center.

        [Torque_Decorations.TorqueCallBack("", "DOFPostEffect", "setAutoFocus", "%this, %enabled", 2, 92100, false)]
        public void DOFPostEffectsetAutoFocus(string thisobj, bool enabled)
            {
            console.SetVar(thisobj + ".autoFocusEnabled", enabled);
            }
        // Set the parameters that control how the near and far equations are calculated
        // from the focal distance. If you are not using auto focus you will need to call
        // setFocusParams PRIOR to calling setFocalDist.


        [Torque_Decorations.TorqueCallBack("", "DOFPostEffect", "setFocusParams", "%this, %nearBlurMax, %farBlurMax, %minRange, %maxRange, %nearSlope, %farSlope", 7, 92200, false)]
        public void DOFPostEffectsetFocusParams(string thisobj, float nearBlurMax, float farBlurMax, float minRange, float maxRange, float nearSlope, float farSlope)
            {
            console.SetVar(thisobj + ".nearBlurMax", nearBlurMax);
            console.SetVar(thisobj + ".farBlurMax", farBlurMax);
            console.SetVar(thisobj + ".minRange", minRange);
            console.SetVar(thisobj + ".maxRange", maxRange);
            console.SetVar(thisobj + ".nearSlope", nearSlope);
            console.SetVar(thisobj + ".farSlope", farSlope);
            }

        /*

        More information...

        This DOF technique is based on this paper:
        http://http.developer.nvidia.com/GPUGems3/gpugems3_ch28.html

        ================================================================================
        1. Overview of how we represent "Depth of Field"
        ================================================================================

        DOF is expressed as an amount of bluriness per pixel according to its depth.
        We represented this by a piecewise linear curve depicted below.

        Note: we also refer to "bluriness" as CoC ( circle of confusion ) which is the term
        used in the basis paper and in photography.
   

        X-axis (depth) 
        x = 0.0----------------------------------------------x = 1.0       

        Y-axis (bluriness)
        y = 1.0   
          |
          |   ____(x1,y1)                         (x4,y4)____
          |       (ns,nb)\  <--Line1  line2--->  /(fe,fb)
          |               \                     /
          |                \(x2,y2)     (x3,y3)/
          |                 (ne,0)------(fs,0)  
        y = 0.0
                 

        I have labeled the "corners" of this graph with (Xn,Yn) to illustrate that
        this is in fact a collection of line segments where the x/y of each point
        corresponds to the key below.

        key:
        ns - (n)ear blur (s)tart distance
        nb - (n)ear (b)lur amount (max value)
        ne - (n)ear blur (e)nd distance
        fs - (f)ar blur (s)tart distance
        fe - (f)ar blur (e)nd distance
        fb - (f)ar (b)lur amount (max value)

        Of greatest importance in this graph is Line1 and Line2. Where...
        L1 { (x1,y1), (x2,y2) }
        L2 { (x3,y3), (x4,y4) }

        Line one represents the amount of "near" blur given a pixels depth and line two
        represents the amount of "far" blur at that depth.

        Both these equations are evaluated for each pixel and then the larger of the two
        is kept. Also the output blur (for each equation) is clamped between 0 and its
        maximum allowable value.

        Therefore, to specify a DOF "qualify" you need to specify the near-blur-line, 
        far-blur-line, and maximum near and far blur value.

        ================================================================================
        2. Abstracting a "focal depth"
        ================================================================================

        Although the shader(s) work in terms of a near and far equation it is more
        useful to express DOF as an adjustable focal depth and derive the other parameters
        "under the hood".

        Given a maximum near/far blur amount and a near/far slope we can calculate the
        near/far equations for any focal depth. We extend this to also support a range
        of depth around the focal depth that is also in focus and for that range to
        shrink or grow as the focal depth moves closer or farther.

        Keep in mind this is only one implementation and depending on the effect you
        desire you may which to express the relationship between focal depth and 
        the shader paramaters different. 

        */

        //-----------------------------------------------------------------------------
        // GFXStateBlockData / ShaderData
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "dof_init", "", 0, 92300, true)]
        public void dof_init()
            {
            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "PFX_DefaultDOFStateBlock");
            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[1]", "SamplerClampPoint");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "PFX_DOFCalcCoCStateBlock");
            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "PFX_DOFDownSampleStateBlock");
            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampPoint");
            ts.Create(m_ts);


            ts = new TorqueSingleton("GFXStateBlockData", "PFX_DOFBlurStateBlock");
            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "PFX_DOFFinalStateBlock");
            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[2]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[3]", "SamplerClampPoint");

            ts.Props.Add("blendDefined", "true");
            ts.Props.Add("blendEnable", "true");
            ts.Props.Add("blendDest", "GFXBlendInvSrcAlpha");
            ts.Props.Add("blendSrc", "GFXBlendOne");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_DOFDownSampleShader");

            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/dof/DOF_DownSample_V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/dof/DOF_DownSample_P.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_DOFBlurYShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/dof/DOF_Gausian_V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/dof/DOF_Gausian_P.hlsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.PropsAddString("defines", "BLUR_DIR=float2(0.0,1.0)");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_DOFBlurXShader : PFX_DOFBlurYShader");
            ts.PropsAddString("defines", "BLUR_DIR=float2(1.0,0.0)");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_DOFCalcCoCShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/dof/DOF_CalcCoC_V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/dof/DOF_CalcCoC_P.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_DOFSmallBlurShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/dof/DOF_SmallBlur_V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/dof/DOF_SmallBlur_P.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_DOFFinalShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/dof/DOF_Final_V.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/dof/DOF_Final_P.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            }
        //-----------------------------------------------------------------------------
        // PostEffects
        //-----------------------------------------------------------------------------


        [Torque_Decorations.TorqueCallBack("", "DOFPostEffect", "onAdd", "%this", 1, 92400, false)]
        public void DOFPostEffectonAdd(string thisobj)
            {
            // The weighted distribution of CoC value to the three blur textures
            // in the order small, medium, large. Most likely you will not need to
            // change this value.
            DOFPostEffectsetLerpDist(thisobj, "0.2", "0.3", "0.5");
            
            console.SetVar(thisobj + ".autoFocusEnabled", false);
            console.SetVar(thisobj + ".focalDist", 0);
            console.SetVar(thisobj + ".nearBlurMax", .5);
            console.SetVar(thisobj + ".farBlurMax", .5);
            console.SetVar(thisobj + ".minRange", 50);
            console.SetVar(thisobj + ".maxRange", 500);
            console.SetVar(thisobj + ".nearSlope", -5);
            console.SetVar(thisobj + ".farSlope", 5);
            }

        [Torque_Decorations.TorqueCallBack("", "DOFPostEffect", "setLerpDist", "%this, %d0, %d1, %d2", 4, 92500, false)]
        public void DOFPostEffectsetLerpDist(string thisobj, string d0, string d1, string d2)
            {
            console.SetVar(thisobj + ".lerpScale", (-1.0 / d0.AsFloat()).AsString() + " " + (-1.0 / d1.AsFloat()).AsString() + " " + (-1.0 / d2.AsFloat()).AsString() + " " + (1.0 / d2.AsFloat()).AsString());

            string lerpbias = "1.0 ";
            lerpbias += ((1 - d2.AsFloat()) / d1.AsFloat()).AsString() + " ";
            lerpbias += (1.0 / d2.AsFloat()).AsString() + " ";
            lerpbias += ((d2.AsFloat() - 1.0) / d2.AsFloat()).AsString();
            console.SetVar(thisobj + ".lerpBias", lerpbias);
            }

        [Torque_Decorations.TorqueCallBack("", "", "dof_init1", "", 0, 92600, true)]
        public void dof_init1()
            {
            TorqueSingleton ts = new TorqueSingleton("PostEffect", "DOFPostEffect");
            ts.PropsAddString("renderTime", "PFXAfterBin");
            ts.PropsAddString("renderBin", "GlowBin");
            ts.Props.Add("renderPriority", "0.1");

            ts.Props.Add("shader", "PFX_DOFDownSampleShader");
            ts.Props.Add("stateBlock", "PFX_DOFDownSampleStateBlock");
            ts.PropsAddString("texture[0]", "$backBuffer");
            ts.PropsAddString("texture[1]", "#prepass");
            ts.PropsAddString("target", "#shrunk");
            ts.PropsAddString("targetScale", "0.25 0.25");

            ts.Props.Add("isEnabled", "false");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "DOFBlurY");
            ts.Props.Add("shader", "PFX_DOFBlurYShader");
            ts.Props.Add("stateBlock", "PFX_DOFBlurStateBlock");
            ts.PropsAddString("texture[0]", "#shrunk");
            ts.PropsAddString("target", "$outTex");
            ts.Create(m_ts);

            console.Call("DOFPostEffect", "add", new string[] { "DOFBlurY" });

            ts = new TorqueSingleton("PostEffect", "DOFBlurX");
            ts.Props.Add("shader", "PFX_DOFBlurXShader");
            ts.Props.Add("stateBlock", "PFX_DOFBlurStateBlock");
            ts.PropsAddString("texture[0]", "$inTex");
            ts.PropsAddString("target", "#largeBlur");
            ts.Create(m_ts);

            console.Call("DOFPostEffect", "add", new string[] { "DOFBlurX" });

            ts = new TorqueSingleton("PostEffect", "DOFCalcCoC");
            ts.Props.Add("shader", "PFX_DOFCalcCoCShader");
            ts.Props.Add("stateBlock", "PFX_DOFCalcCoCStateBlock");
            ts.PropsAddString("texture[0]", "#shrunk");
            ts.PropsAddString("texture[1]", "#largeBlur");
            ts.PropsAddString("target", "$outTex");
            ts.Create(m_ts);

            console.Call("DOFPostEffect", "add", new string[] { "DOFCalcCoC" });

            ts = new TorqueSingleton("PostEffect", "DOFSmallBlur");
            ts.Props.Add("shader", "PFX_DOFSmallBlurShader");
            ts.Props.Add("stateBlock", "PFX_DefaultDOFStateBlock");
            ts.PropsAddString("texture[0]", "$inTex");
            ts.PropsAddString("target", "$outTex");
            ts.Create(m_ts);

            console.Call("DOFPostEffect", "add", new string[] { "DOFSmallBlur" });

            ts = new TorqueSingleton("PostEffect", "DOFFinalPFX");
            ts.Props.Add("shader", "PFX_DOFFinalShader");
            ts.Props.Add("stateBlock", "PFX_DOFFinalStateBlock");
            ts.PropsAddString("texture[0]", "$backBuffer");
            ts.PropsAddString("texture[1]", "$inTex");
            ts.PropsAddString("texture[2]", "#largeBlur");
            ts.PropsAddString("texture[3]", "#prepass");
            ts.PropsAddString("target", "$backBuffer");
            ts.Create(m_ts);

            console.Call("DOFPostEffect", "add", new string[] { "DOFFinalPFX" });
            }
        //-----------------------------------------------------------------------------
        // Scripts
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "DOFPostEffect", "setShaderConsts", "this", 1, 92700, false)]
        public void DOFPostEffectsetShaderConsts(string thisobj)
            {
            if (console.GetVarBool(thisobj + ".autoFocusEnabled"))
                DOFPostEffectautoFocus(thisobj);
                //console.Call(thisobj, "autoFocus");


            double fd = console.GetVarFloat(thisobj + ".focalDist") / console.GetVarFloat("$Param::FarDist");
            double range = (double)Util.mLerp(console.GetVarFloat(thisobj + ".minRange"), console.GetVarFloat(thisobj + ".maxRange"), (float)fd) / console.GetVarFloat("$Param::FarDist") * .5;

            // We work in "depth" space rather than real-world units for the
            // rest of this method...

            // Given the focal distance and the range around it we want in focus
            // we can determine the near-end-distance and far-start-distance

            double ned = Util.getMax((float)fd - (float)range, (float)0.0);
            double fsd = Util.getMin((float)fd + (float)range, (float)1.0);
            // near slope
            float nsl = console.GetVarFloat(thisobj + ".nearSlope");
            // Given slope of near blur equation and the near end dist and amount (x2,y2)
            // solve for the y-intercept
            // y = mx + b
            // so...
            // y - mx = b

            float b = (float)0.0 - (float)nsl * (float)ned;
            string eqNear = nsl.AsString() + " " + b.AsString() + " 0.0";

            // Do the same for the far blur equation...

            float fsl = console.GetVarFloat(thisobj + ".farSlope");
            b = (float)0.0 - (float)fsl * (float)fsd;

            string eqFar = fsl.AsString() + " " + b.AsString() + " " + "1.0";

            PostEffect.setShaderConst(thisobj, "$dofEqWorld", eqNear);
            PostEffect.setShaderConst("DOFFinalPFX", "$dofEqFar", eqFar);
            PostEffect.setShaderConst(thisobj, "$maxWorldCoC", console.GetVarString(thisobj + ".nearBlurMax"));
            PostEffect.setShaderConst("DOFFinalPFX", "$maxFarCoC", console.GetVarString(thisobj + ".farBlurMax"));
            PostEffect.setShaderConst("DOFFinalPFX", "$dofLerpScale", console.GetVarString(thisobj + ".lerpScale"));
            PostEffect.setShaderConst("DOFFinalPFX", "$dofLerpBias", console.GetVarString(thisobj + ".lerpBias"));

            //Thread.Sleep(5);
            }
        [Torque_Decorations.TorqueCallBack("", "DOFPostEffect", "autoFocus", "this", 1, 92800, false)]
        public void DOFPostEffectautoFocus(string thisobj)
            {
            if (!console.isObject("ServerConnection") || !console.isObject(console.Call("ServerConnection", "getCameraObject")))
                return;

            uint mask = (uint)(WinterLeaf.Enums.SceneObjectTypesAsUint.StaticObjectType | WinterLeaf.Enums.SceneObjectTypesAsUint.TerrainObjectType);

            string control = GameConnection.getCameraObject("ServerConnection");  //console.Call("ServerConnection", "getCameraObject");
            Point3F fvec = ShapeBase.getEyeVector(control);// console.Call(control, "getEyeVector");
            Point3F start = ShapeBase.getEyePoint(control);// console.Call(control, "getEyePoint");

            Point3F end = Util.VectorAdd(start, Util.VectorScale(fvec, console.GetVarFloat("$Param::FarDist")));

            string result = Util.containerRayCast(start,end, mask, control, true);

            string hitPos = Util.getWords(result, 1, 3);
            string focDist = console.GetVarString("$Param::FarDist");
            if (hitPos != "")
                focDist = Util.VectorDist(new Point3F(hitPos), start).AsString();

            console.SetVar(thisobj + ".focalDist", focDist);
            }
        }
    }
