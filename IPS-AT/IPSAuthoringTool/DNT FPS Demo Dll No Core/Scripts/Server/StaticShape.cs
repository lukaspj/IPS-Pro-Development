using System;
using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "StaticShapeData", "create", "%data",  1, 0,false)]
        public string StaticShapeDataCreate(string data)
            {
            Torque_Class_Helper tch = new Torque_Class_Helper("StaticShape");
            tch.Props.Add("dataBlock",data);
            string obj = tch.Create(m_ts).AsString();
            return obj;
            }

        }
    }
