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
        [Torque_Decorations.TorqueCallBack("", "", "Initialize_AdvanceLight_LightViz4", "", 0, 62931, true)]
        public void Initialize_AdvanceLight_LightViz4()
            {
            Torque_Class_Helper tch = new Torque_Class_Helper("ShaderData", "AL_ShadowVisualizeShader");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/guiMaterialV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/lighting/advanced/dbgShadowVisualizeP.hlsl");

            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/guiMaterialV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/lighting/advanced/gl/dbgShadowVisualizeP.glsl");

            tch.Props.Add("pixVersion", "2.0");
            tch.Create(m_ts);

            tch = new Torque_Class_Helper("CustomMaterial", "AL_ShadowVisualizeMaterial");
            tch.Props.Add("shader", "AL_ShadowVisualizeShader");
            tch.Props.Add("stateBlock", "AL_DepthVisualizeState");

            tch.PropsAddString(@"sampler[""shadowMap""]", "#AL_ShadowVizTexture");
            tch.PropsAddString(@"sampler[""depthViz""]", "depthviz");

            tch.Props.Add("pixVersion", "2.0");
            tch.Create(m_ts);

            TorqueSingleton ts = new TorqueSingleton("GuiControlProfile", "AL_ShadowLabelTextProfile");
            ts.PropsAddString("fontColor", "0 0 0");
            ts.Props.Add("autoSizeWidth", "true");
            ts.Props.Add("autoSizeHeight", "true");
            ts.PropsAddString("justify", "left");
            ts.Props.Add("fontSize", "14");
            ts.Create(m_ts);
            }

        [Torque_Decorations.TorqueCallBack("", "", "toggleShadowViz", "", 0, 62932, false)]
        public void toggleShadowViz()
            {
            if (console.Call("AL_ShadowVizOverlayCtrl", "isAwake").AsBool())
                {
                Util._setShadowVizLight("0");
                GuiCanvas.popDialog("Canvas", "AL_ShadowVizOverlayCtrl");
                }
            else
                {
                GuiCanvas.pushDialog("Canvas", "AL_ShadowVizOverlayCtrl", "100");
                _setShadowVizLight(console.Call("EWorldEditor", "getSelectedObject", new string[] { "0" }), false);
                }
            }
        [Torque_Decorations.TorqueCallBack("", "", "_setShadowVizLight", "%light, %force", 2, 62933, false)]
        public void _setShadowVizLight(string light, bool force)
            {
            
            if (!console.Call("AL_ShadowVizOverlayCtrl", "isAwake").AsBool())
                return;
            if (console.Call("AL_ShadowVizOverlayCtrl", "isLocked").AsBool() && !force)
                return;

            string sizeAndAspect = "";
            if (console.isObject(light))
                {
                string clientLight = serverToClientObject(light);  //console.Call("serverToClientObject", new string[] { light });
                sizeAndAspect = Util._setShadowVizLight(clientLight);

                }
            console.Call(SimSet.findObjectByInternalName("AL_ShadowVizOverlayCtrl", "MatCtrl", true), "setMaterial", new string[] { "AL_ShadowVisualizeMaterial" });
            string text = "ShadowViz";
            if (console.isObject(light))
                text = text + ":" + sizeAndAspect.Split(' ')[0] + " x " + sizeAndAspect.Split(' ')[1];

            console.SetVar(SimSet.findObjectByInternalName("AL_ShadowVizOverlayCtrl", "WindowCtrl", true), text);
            }




        [Torque_Decorations.TorqueCallBack("", "", "showShadowVizForLight", "%light", 1, 62934, false)]
        public void showShadowVizForLight(string light)
            {
            if (!console.Call("AL_ShadowVizOverlayCtrl", "isAwake").AsBool())
                GuiCanvas.pushDialog("Canvas", "AL_ShadowVizOverlayCtrl", "100");

            _setShadowVizLight(light, true);
            }

        // Prevent shadowViz from changing lights in response to editor selection
        // events until unlock is called. The only way a vis light will change while locked
        // is if showShadowVizForLight is explicitly called by the user.
        [Torque_Decorations.TorqueCallBack("", "", "lockShadowViz", "", 0, 62934, false)]
        public void lockShadowViz()
            {
            console.SetVar("AL_ShadowVizOverlayCtrl.islocked", true);
            }
        [Torque_Decorations.TorqueCallBack("", "", "unlockShadowViz", "", 0, 62935, false)]
        public void unlockShadowViz()
            {
            console.SetVar("AL_ShadowVizOverlayCtrl.islocked", false);
            }
        }
    }
