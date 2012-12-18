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
        public bool canvasCreated = false;

        [Torque_Decorations.TorqueCallBack("", "", "Initialize_Canvas", "", 0, 22000, true)]
        public void Initialize_Canvas()
            {
            //console.SetVar("$canvasCreated", false);
            canvasCreated = false;
            }

        [Torque_Decorations.TorqueCallBack("", "", "configureCanvas", "", 0, 22000, false)]
        public void configureCanvas()
            {
            // Setup a good default if we don't have one already.
            if (console.GetVarString("$pref::Video::mode") == "")
                console.SetVar("$pref::Video::mode", "800 600 false 32 60 0");


            string data = console.GetVarString("$pref::Video::mode");
            string resX = Util.getWord(data, console.GetVarInt("$WORD::RES_X"));
            string resY = Util.getWord(data, console.GetVarInt("$WORD::RES_Y"));
            string fs = Util.getWord(data, console.GetVarInt("$WORD::FULLSCREEN"));
            string bpp = Util.getWord(data, console.GetVarInt("$WORD::BITDEPTH"));
            string rate = Util.getWord(data, console.GetVarInt("$WORD::REFRESH"));
            string fsaa = Util.getWord(data, console.GetVarInt("$WORD::AA"));

            console.print("--------------");
            console.print("Attempting to set resolution to \"" + console.GetVarString("$pref::Video::mode") + "\"");

            string deskRes = Util.getDesktopResolution().AsString();
            string deskResX = Util.getWord(deskRes, console.GetVarInt("$WORD::RES_X"));
            string deskResY = Util.getWord(deskRes, console.GetVarInt("$WORD::RES_Y"));
            string deskResBPP = Util.getWord(deskRes, 2);

            // We shouldn't be getting this any more but just in case...
            if (bpp == "Default")
                bpp = deskResBPP;

            // Make sure we are running at a valid resolution
            if (fs == "0" || fs == "false")
                {
                // Windowed mode has to use the same bit depth as the desktop
                bpp = deskResBPP;
                if (resX.AsInt() >= deskResX.AsInt() || resY.AsInt() >= deskResY.AsInt())
                    {
                    console.print("Warning: The requested windowed resolution is equal to or larger than the current desktop resolution. Attempting to find a better resolution");
                    string rescount = GuiCanvas.getModeCount("Canvas").AsString();
                    for (int i = (rescount.AsInt() - 1); i >= 0; i--)
                        {
                        string testRes = GuiCanvas.getMode("Canvas", i);
                        string testResX = Util.getWord(testRes, console.GetVarInt("$WORD::RES_X"));
                        string testResY = Util.getWord(testRes, console.GetVarInt("$WORD::RES_Y"));
                        string testBPP = Util.getWord(testRes, console.GetVarInt("$WORD::BITDEPTH"));

                        if (testBPP != bpp)
                            continue;

                        if ((testResX.AsInt() >= deskResX.AsInt()) || (testResY.AsInt() >= deskResY.AsInt())) continue;
                        // This will work as our new resolution
                        resX = testResX;
                        resY = testResY;

                        console.warn("Warning: Switching to \"" + resX + " " + resY + " " + bpp + "\"");
                        break;
                        }

                    }

                }
            console.SetVar("$pref::Video::mode", resX + " " + resY + " " + fs + " " + bpp + " " + rate + " " + fsaa);

            string fsLabel = "";

            if (fs.AsInt() == 1 || fs == "true")
                fsLabel = "Yes";
            else
                fsLabel = "No";

            console.print(
                "Accept Mode: \n" +
                "--Resolution    : " + resX + " " + resY + "\n" +
                "--Full Screen   : " + fsLabel + "\n" +
                "--Bits Per Pixel: " + bpp + "\n" +
                "--Refresh Rate  : " + rate + "\n" +
                "--FSAA Level    : " + fsaa + "\n" +
                "---------------------------"
                );
            // Actually set the new video mode
            GuiCanvas.setVideoMode("Canvas", resX, resY, fs, bpp, rate, fsaa);
            // FXAA piggybacks on the FSAA setting in $pref::Video::mode.

            if (console.isObject("FXAA_PostEffect"))
                console.SetVar("FXAA_PostEffect.isEnabled", (fsaa.AsInt() > 0 ? "true" : "false"));

            if (console.GetVarBool("$pref::Video::autoDetect"))
                GraphicsQualityAutodetect();
            }
        [Torque_Decorations.TorqueCallBack("", "", "initializeCanvas", "", 0, 22000, false)]
        public void initializeCanvas()
            {
            // Don't duplicate the canvas.
            if (canvasCreated)
                {
                console.error("Cannot instantiate more than one canvas!");
                return;
                }
            if (!console.Call("createCanvas").AsBool())
                {
                console.error("Canvas creation failed. Shutting down.");
               // Stop();
                }
            canvasCreated = true;
            }
        //---------------------------------------------------------------------------------------------
        // resetCanvas
        // Forces the canvas to redraw itself.
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "resetCanvas", "", 0, 22000, false)]
        public void resetCanvas()
            {
            if (console.isObject("Canvas"))
                GuiCanvas.repaint("Canvas", 0);
            }

        //---------------------------------------------------------------------------------------------
        // Callbacks for window events.
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "GuiCanvas", "onLoseFocus", "(%this)", 1, 22000, false)]
        public void GuiCanvasonLoseFocus(string thisobj)
            {

            }

        //---------------------------------------------------------------------------------------------
        // Full screen handling
        //---------------------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "GuiCanvas", "attemptFullscreenToggle", "(%this)", 1, 22000, false)]
        public void attemptFullscreenToggle(string thisobj)
            {
            if (EditorIsActive() && !console.Call(thisobj, "isFullScreen").AsBool())
                {
                console.Call("MessageBoxOk", new string[] { "Windowed Mode Required", "Please exit the Mission Editor to switch to full screen." });
                return;
                }
            if (GuiEditorIsActive() && !console.Call(thisobj, "isFullScreen").AsBool())
                {
                console.Call("MessageBoxOk", new string[] { "Windowed Mode Required", "Please exit the GUI Editor to switch to full screen." });
                return;
                }
            GuiCanvas.toggleFullscreen(thisobj);
            }
        }
    }
