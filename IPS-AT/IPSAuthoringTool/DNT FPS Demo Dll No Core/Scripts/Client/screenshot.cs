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

        //---------------------------------------------------------------------------------------------
        // formatImageNumber
        // Preceeds a number with zeros to make it 6 digits long.
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "formatImageNumber", "(%number)", 1, 31000, false)]
        public string formatImageNumber(string number)
            {
            if (number.AsInt() < 10)
                number = "0" + number;
            if (number.AsInt() < 100)
                number = "0" + number;
            if (number.AsInt() < 1000)
                number = "0" + number;
            if (number.AsInt() < 10000)
                number = "0" + number;
            return number;
            }
        //---------------------------------------------------------------------------------------------
        // formatSessionNumber
        // Preceeds a number with zeros to make it 4 digits long.
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "formatSessionNumber", "(%number)", 1, 31000, false)]
        public string formatSessionNumber(string number)
            {
            if (number.AsInt() < 10)
                number = "0" + number;
            if (number.AsInt() < 100)
                number = "0" + number;
            return number;
            }

        //---------------------------------------------------------------------------------------------
        // recordMovie
        // Records a movie file from the Canvas content using the specified fps.
        // Possible encoder values are "PNG" and "THEORA" (default).
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "recordMovie", "(%movieName, %fps, %encoder)", 3, 31000, false)]
        public void recordMovie(string movieName, string fps, string encoder)
            {
            if (!console.isObject("Canvas"))
                return;
            if (encoder == "")
                encoder = "THEORA";
            string resolution = GuiCanvas.getVideoMode("Canvas");
            Util.startVideoCapture("Canvas", movieName, encoder, fps.AsFloat(), new Point2I(resolution));
            }
        [Torque_Decorations.TorqueCallBack("", "", "stopMovie", "", 0, 31000, false)]
        public void stopMovie()
            {
            Util.stopVideoCapture();
            }
        /// This is bound in initializeCommon() to take
        /// a screenshot on a keypress.
        [Torque_Decorations.TorqueCallBack("", "", "doScreenShot", "%val", 1, 31000, false)]
        public void doScreenShot(string val)
            {
            if (val.AsInt() == 0)
                return;
            console.Call("_screenShot", new string[] { "1" });
            }
        [Torque_Decorations.TorqueCallBack("", "", "screenshotinit", "", 0, 31000, true)]
        public void screenshotinit()
            {
            console.SetVar("$screenshotNumber", 0);
            }
        /// Internal function which generates unique filename
        /// and triggers a screenshot capture.
        [Torque_Decorations.TorqueCallBack("", "", "_screenShot", "%tiles, %overlap", 2, 31000, false)]
        public void _screenShot(string tiles, string overlap)
            {
            if (console.GetVarString("$pref::Video::screenShotSession") == "")
                console.SetVar("$pref::Video::screenShotSession", 0);

            if (console.GetVarInt("$screenshotNumber") == 0)
                console.SetVar("$pref::Video::screenShotSession", console.GetVarInt("$pref::Video::screenShotSession") + 1);

            if (console.GetVarInt("$pref::Video::screenShotSession") > 999)
                console.SetVar("$pref::Video::screenShotSession", 1);

            string name = "screenshot_" + console.Call("formatSessionNumber", new string[] { console.GetVarString("$pref::Video::screenShotSession") }) + "-" +
                          console.Call("formatImageNumber", new string[] { console.GetVarString("$screenshotNumber") });
            
            name = Util._expandFilename(name);
            console.SetVar("$screenshotNumber", console.GetVarString("$screenshotNumber") + 1);

            if ((console.GetVarString("$pref::Video::screenShotFormat") == "JPEG") ||
                (console.GetVarString("$pref::Video::screenShotFormat") == "JPG"))
                Util.screenShot(name, "JPEG", tiles.AsUint(), overlap.AsFloat());
            else
                Util.screenShot(name, "PNG", tiles.AsUint(), overlap.AsFloat());
            }
        /// This will close the console and take a large format
        /// screenshot by tiling the current backbuffer and save
        /// it to the root game folder.
        ///
        /// For instance a tile setting of 4 with a window set to
        /// 800x600 will output a 3200x2400 screenshot.
        [Torque_Decorations.TorqueCallBack("", "", "tiledScreenShot", "%tiles, %overlap", 2, 31000, false)]
        public void tiledScreenShot(string tiles, string overlap)
            {
            GuiCanvas.popDialog("Canvas", "ConsoleDlg");
            
            _screenShot(tiles, overlap);
            }
        }
    }
