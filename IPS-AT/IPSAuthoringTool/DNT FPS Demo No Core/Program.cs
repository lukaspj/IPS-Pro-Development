using System;
using System.Drawing;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using WinterLeaf;
using System.Diagnostics;
using IPSAuthoringTool;


namespace DNT_FPS_Demo
    {
    internal static class Program
        {
        /// <summary>
        ///   The main entry point for the application.
        /// </summary>
        /// 

        [STAThread]
        private static void Main()
            {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            //If we are running dedicated, there is no reason to show a form.

            //dnt_torque = new dnTorque(Process.GetCurrentProcess().Handle);

            IPSAuthoringTool.MainWindow Window = new MainWindow();
            try
            {
                Window.ShowDialog();
            }
            catch (Exception e)
            {
            }
            Application.Exit();
            }
        }
    }