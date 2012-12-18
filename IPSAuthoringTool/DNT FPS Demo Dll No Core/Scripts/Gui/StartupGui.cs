using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
using System.Windows.Forms;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {

    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // StartupGui is the splash screen that initially shows when the game is loaded
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "loadStartup", "()",  0, 4950, false)]
        public void loadStartup()
            {
            // The index of the current splash screen
            console.SetVar("$StartupIdx", 0);
            // A list of the splash screens and logos
            // to cycle through. Note that they have to
            // be in consecutive numerical order
            console.SetVar("StartupGui.bitmap[0]", "art/gui/background");
            console.SetVar("StartupGui.logo[0]", "art/gui/Torque-3D-logo");
            console.SetVar("StartupGui.logoPos[0]", "178 251");
            console.SetVar("StartupGui.logoExtent[0]", "443 139");
            // Call the next() function to set our firt
            // splash screen
            console.Call("StartupGui", "next");
            // Play our startup sound
            //SFXPlayOnce(AudioGui, "art/sound/gui/startup");//SFXPlay(startsnd);
            //MessageBox.Show("test" + m_ts._hwndhndl.ToInt32());
            
            }
        [Torque_Decorations.TorqueCallBack("", "StartupGui", "click", "(this)",  1, 4950, false)]
        public void StartupGuiClick(string thisobj)
            {
            console.SetVar(thisobj+".done",true);
            console.Call(thisobj, "onDone");
            }
        [Torque_Decorations.TorqueCallBack("", "StartupGui", "next", "(this)",  1, 4950, false)]
        public void StartupGuiNext(string thisobj)
            {
           
            
            // Set us to a blank screen while we load the next one
            GuiCanvas.setContent("Canvas","BlankGui");
            // Set our bitmap and reset the done variable
            console.Call(thisobj, "setBitmap", new string[] {console.GetVarString(thisobj + ".bitmap[" + console.GetVarString("$StartupIdx") +"]")});

            console.SetVar(thisobj+".done",false);
            // If we have a logo then set it


            string stl = SimSet.findObjectByInternalName(thisobj, "StartupLogo", false);
            if (console.isObject(stl))
                {
                if (console.GetVarString(thisobj + ".logo[" + console.GetVarString("$StartupIdx") +"]") != "")
                    {
                    console.Call(stl, "setBitmap", new string[] { console.GetVarString(thisobj + ".logo" + console.GetVarString("$StartupIdx")) });

                    if (console.GetVarString(thisobj + ".logoPos[" + console.GetVarString("$StartupIdx") +"]")!="")
                        {
                        string pos = console.GetVarString(thisobj + ".logoPos[" + console.GetVarString("$StartupIdx") +"]");
                        console.Call(stl, "setPosition", new string[] { pos.Split(' ')[0], pos.Split(' ')[1] });
                        }
                    if (console.GetVarString(thisobj + ".logoExtent[" + console.GetVarString("$StartupIdx") +"]")!="")
                        {
                        console.Call(stl, "setExtent",
                                 new string[] {console.GetVarString(thisobj + ".logoExtent[" + console.GetVarString("$StartupIdx") +"]")});
                        }
                    console.Call(stl, "setVisible", new string[] { "True" });
                    }
                else
                    {
                    console.Call(stl, "setVisible", new string[] { "false" });
                    }

                }


            string st2 = SimSet.findObjectByInternalName(thisobj, "StartupLogoSecondary", false);
            if (console.isObject(st2))
                {
                if (console.GetVarString(thisobj + ".seclogo[" + console.GetVarString("$StartupIdx") +"]") != "")
                    {
                    console.Call(st2, "setBitmap", new string[] { console.GetVarString(thisobj + ".seclogo[" + console.GetVarString("$StartupIdx")+"]") });

                    if (console.GetVarString(thisobj + ".seclogoPos[" + console.GetVarString("$StartupIdx")+"]") != "")
                        {
                        string pos = console.GetVarString(thisobj + ".seclogoPos[" + console.GetVarString("$StartupIdx")+"]");
                        console.Call(st2, "setPosition", new string[] { pos.Split(' ')[0], pos.Split(' ')[1] });
                        }
                    if (console.GetVarString(thisobj + ".seclogoExtent[" + console.GetVarString("$StartupIdx")+"]") != "")
                        {
                        console.Call(st2, "setExtent",
                                 new string[] { console.GetVarString(thisobj + ".seclogoExtent[" + console.GetVarString("$StartupIdx")+"]") });
                        }
                    console.Call(st2, "setVisible", new string[] { "True" });
                    }
                else
                    {
                    console.Call(st2, "setVisible", new string[] { "false" });
                    }

                }

            console.SetVar("$StartupIdx", console.GetVarInt("$StartupIdx")+1);
            GuiCanvas.setContent("Canvas",thisobj);
            }
        [Torque_Decorations.TorqueCallBack("", "StartupGui", "onDone", "(this)",  1, 4950, false)]
        public void StartupGuionDone(string thisobj)
            {
            if (!console.GetVarBool(thisobj + ".done")) return;
            if (console.GetVarString(thisobj +".bitmap[" + console.GetVarString("$StartupIdx")+"]")=="")
                {
                console.SetVar(thisobj+".done",true);
                console.Call("loadMainMenu");
                }
            else
                {
                console.Call(thisobj, "next");
                }
            }
        }
    }
