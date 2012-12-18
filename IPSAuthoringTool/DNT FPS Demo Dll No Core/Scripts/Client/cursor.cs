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
        [Torque_Decorations.TorqueCallBack("", "", "Initialize_Cursor", "", 0, 23000, true)]
        public void Initialize_Cursor()
            {
            console.SetVar("$cursorControlled", true);
            }
        [Torque_Decorations.TorqueCallBack("", "", "showCursor", "", 0, 23000, false)]
        public void showCursor()
            {
            if (console.GetVarBool("$cursorControlled"))
                Util._lockMouse("false");

            GuiCanvas.cursorOn("Canvas");
            }
        [Torque_Decorations.TorqueCallBack("", "", "hideCursor", "", 0, 23000, false)]
        public void hideCursor()
            {
            if (console.GetVarBool("$cursorControlled"))
                Util._lockMouse("true");
                
            GuiCanvas.cursorOff("Canvas");
            }
        //---------------------------------------------------------------------------------------------
        // In the CanvasCursor package we add some additional functionality to the built-in GuiCanvas
        // class, of which the global Canvas object is an instance. In this case, the behavior we want
        // is for the cursor to automatically display, except when the only guis visible want no
        // cursor - usually the in game interface.
        //---------------------------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("CanvasCursorPackage", "GuiCanvas", "checkCursor", "%this", 1, 23000, false)]
        public void GuiCanvascheckCursor(string thisobj)
            {
            int count = SimSet.getCount(thisobj);
            for (uint i = 0; i < count; i++)
                {
                string control = SimSet.getObject(thisobj, i);
                if ((console.GetVarString(control + ".noCursor") != "") && console.GetVarBool(control + ".noCursor")) continue;
                showCursor();
                return;
                }
            hideCursor();
            }
        [Torque_Decorations.TorqueCallBack("CanvasCursorPackage", "GuiCanvas", "setContent", "%this,%ctrl", 2, 23000, false)]
        public void GuiCanvasSetContent(string thisobj, string ctrl)
            {
            //since the parent execute is actually calling the c++ function and not a script function we do not want it to pass
            //depth since it will be too many params
            console.ParentExecute(thisobj, "setContent", 1, new string[] { thisobj, ctrl }, true);
            GuiCanvascheckCursor(thisobj);
            }
        [Torque_Decorations.TorqueCallBack("CanvasCursorPackage", "GuiCanvas", "pushDialog", "(%this, %ctrl, %layer, %center)", 4, 23000, false)]
        public void GuiCanvaspushDialog(string thisobj, string ctrl, string layer, string center)
            {
            //since the parent execute is actually calling the c++ function and not a script function we do not want it to pass
            //depth since it will be too many params
            //console.ParentExecute(thisobj, "pushDialog", 1, new string[] { thisobj, ctrl, layer, center }, true);
            GuiCanvas.pushDialog(thisobj, ctrl, layer, center);
            GuiCanvascheckCursor(thisobj);
            }
        [Torque_Decorations.TorqueCallBack("CanvasCursorPackage", "GuiCanvas", "popDialog", "(%this, %ctrl)", 2, 23000, false)]
        public void GuiCanvaspopDialog(string thisobj, string ctrl)
            {
            //since the parent execute is actually calling the c++ function and not a script function we do not want it to pass
            //depth since it will be too many params
            //console.ParentExecute(thisobj, "popDialog", 1, new string[] { thisobj, ctrl }, true);
            GuiCanvas.popDialog(thisobj, ctrl);
            GuiCanvascheckCursor(thisobj);
            }
        [Torque_Decorations.TorqueCallBack("CanvasCursorPackage", "GuiCanvas", "popLayer", "(%this, %layer)", 2, 23000, false)]
        public void GuiCanvaspopLayer(string thisobj, string layer)
            {
            //since the parent execute is actually calling the c++ function and not a script function we do not want it to pass
            //depth since it will be too many params
            //console.ParentExecute(thisobj, "popLayer", 1, new string[] { thisobj, layer }, true);
            GuiCanvas.popLayer(thisobj, layer);
            GuiCanvascheckCursor(thisobj);
            }
        [Torque_Decorations.TorqueCallBack("", "", "cursorActivatePackage", "()", 0, 23001, true)]
        public void CursorActivatePackage()
            {
            //I want a little control of when it activates the package, or I could of just set the activate on compile flag in
            //the decoration.
            Util.activatePackage("CanvasCursorPackage");

            }




        [Torque_Decorations.TorqueCallBack("", "", "Initialize_ArtGuiCursors", "", 0, 25000, true)]
        public void Initialize_ArtGuiCursors()
            {
            if (console.GetVarString("$platform") == "macos")
                {
                Torque_Class_Helper tch = new Torque_Class_Helper("GuiCursor", "DefaultCursor");
                tch.PropsAddString("hotSpot", "4 4");
                tch.PropsAddString("renderOffset", " 0 0");
                tch.PropsAddString("bitmapName", "./core/art/gui/images/macCursor");
                tch.CreateDB(m_ts);
                }
            else
                {
                Torque_Class_Helper tch = new Torque_Class_Helper("GuiCursor", "DefaultCursor");
                tch.PropsAddString("hotSpot", "1 1");
                tch.PropsAddString("renderOffset", " 0 0");
                tch.PropsAddString("bitmapName", "./core/art/gui/images/defaultCursor");
                tch.CreateDB(m_ts);
                }
            }
        }
    }
