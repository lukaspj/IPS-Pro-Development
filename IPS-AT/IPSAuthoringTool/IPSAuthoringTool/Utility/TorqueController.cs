using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;

using System.IO;
using System.Threading;
using System.Windows.Forms;
using System.Diagnostics;
using System.Drawing;
using IPSAuthoringTool;

namespace IPSAuthoringTool.Utility
{
    public partial class TorqueController : TorqueScriptTemplate
    {
        private dnTorque dnt;
        private bool dntWindowOpen = false;

        public bool DNTWindowOpen
        {
            get { return dnt != null && dnt.IsRunning && dntWindowOpen; }
            set { dntWindowOpen = value; }
        }

        public dnTorque Dnt
        {
            get { return dnt; }
            set { dnt = value; }
        }
        public TorqueController(ref dnTorque c)
            : base(ref c)
            {
            dnt = c;
            }

        public void Open()
        {
            if (dnt == null)
                return;
            dnt.onShutDown += dnt_onShutDown;

            string TorqueDLL = "IPS-AT.dll";
#if DEBUG
            TorqueDLL = "IPS-AT_DEBUG.dll";
#endif
            //Initialize Torque, pass a handle to this form into T3D so it knows where to rendor the screen to.
            //If you don't do this, you can't pass the mouse and key strokes, w/out the mouse and keystrokes
            //being redirected the application will hang intermittently.
            dnt.InitializeTorque(new[] { "" },
                                        "DNT_FPS_Demo_Game_Dll.Scripts.Server.Main",
                                        "DNT_FPS_Demo_Game_Dll.Scripts.Client.Main",
                                        "DNT_FPS_Demo_Game_Dll.Scripts.Main", "", "", "",
                                        Path.GetDirectoryName(Application.ExecutablePath) + "\\",
                                        @"IPS-AT DLL.dll", TorqueDLL);
            //Let's prepare the T3D display,

            dnt.WindowIcon = Icon.ExtractAssociatedIcon(Application.ExecutablePath);
            //Turn verbose debugging off.
            dnt.Debugging = false;
            dntWindowOpen = true;
        }

        void dnt_onShutDown(object sender, EventArgs e)
        {
            dntWindowOpen = false;
            if (dnt != null && dnt.IsRunning)
                dnt.Stop();
        }

        public void ProperShutdown()
        {
            if (dnt == null)
                return;
            try
            {
                if (dnt.IsRunning == true)
                {
                    dnt.Stop();
                    //console.Call("quit");
                }
            }
            catch (NullReferenceException e)
            {
                //DNT was destroyed even tho it was set to IsRunning.
                console.print("Null reference exception IPSAT-0x01");
            }
            while (dnt.IsRunning)
                Thread.Sleep(100);
            dnt = null;
            dntWindowOpen = false;
        }

        public ConsoleObject getConsole() { return dnt != null && dnt.IsRunning && dntWindowOpen ? console : null; }
        public UtilObject getUtil() { return dnt != null && dnt.IsRunning && dntWindowOpen ? Util : null; }

        string currentEmitter;
        public void RemoveEmitter()
        {
            if(currentEmitter != "" && currentEmitter != null && currentEmitter != null)
                console.Call(currentEmitter, "delete");
            currentEmitter = null;
        }

        public void SpawnEffect(string datablock)
        {
            if (!DNTWindowOpen)
                return;
            lock (dnt.tick)
            {
                RemoveEmitter();
                Torque_Class_Helper TCH = new Torque_Class_Helper("ParticleEffect", "theEmitter");
                TCH.Props.Add("datablock", datablock);
                TCH.PropsAddString("position", "0 0 0");
                currentEmitter = TCH.Create(dnt).AsString();
            }
        }

        public string[] getDatablocks(string type)
        {
            string dats = Util._getSpecificDatablocks(type).Trim();
            return dats.Split(' ');
        }
    }
}
