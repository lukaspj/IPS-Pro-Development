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
        [Torque_Decorations.TorqueCallBack("", "", "Initialize_Art_Gui_Profiles", "", 0, 24000, true)]
        public void Initialize_Art_Gui_Profiles()
            {
            for (int i = 0; i < 2; i++)
                {
                if (console.GetVarString("$Gui::fontCacheDirectory") == "")
                    console.SetVar("$Gui::fontCacheDirectory", console.Call("expandFileName", new string[] { "core/fonts" }));

                //---------------------------------------------------------------------------------------------
                // GuiDefaultProfile is a special profile that all other profiles inherit defaults from. It
                // must exist.
                //---------------------------------------------------------------------------------------------

                if (!console.isObject("GuiDefaultProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiDefaultProfile");
                    tch.Props.Add("tab", "false");
                    tch.Props.Add("canKeyFocus", "false");
                    tch.Props.Add("hasBitmapArray", "false");
                    tch.Props.Add("mouseOverSelected", "false");

                    //fill colore
                    tch.Props.Add("opaque", "false");
                    tch.PropsAddString("fillColor", "242 241 240");
                    tch.PropsAddString("fillColorHL", "228 228 235");
                    tch.PropsAddString("fillColorSEL", "98 100 137");
                    tch.PropsAddString("fillColorNA", "255 255 255 ");

                    // border color
                    tch.PropsAddString("border", "0");
                    tch.PropsAddString("borderColor", "100 100 100");
                    tch.PropsAddString("borderColorHL", "50 50 50 50");
                    tch.PropsAddString("borderColorNA", "75 75 75");

                    // font
                    tch.PropsAddString("fontType", "Arial");
                    tch.PropsAddString("fontSize", "14");
                    tch.PropsAddString("fontCharset", "ANSI");

                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "0 0 0");
                    tch.PropsAddString("fontColorSEL", "255 255 255");

                    // bitmap information
                    tch.PropsAddString("bitmap", "");
                    tch.PropsAddString("bitmapBase", "");
                    tch.PropsAddString("textOffset", "0 0");

                    // used by guiTextControl
                    tch.Props.Add("modal", "true");
                    tch.Props.Add("justify", "left");
                    tch.Props.Add("autoSizeWidth", "false");
                    tch.Props.Add("autoSizeHeight", "false");
                    tch.Props.Add("returnTab", "false");
                    tch.Props.Add("numbersOnly", "false");
                    tch.PropsAddString("cursorColor", "0 0 0 255");

                    tch.CreateDB(m_ts);
                    }


                if (!console.isObject("GuiSolidDefaultProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiSolidDefaultProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "true");
                    tch.Props.Add("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTransparentProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTransparentProfile");
                    tch.Props.Add("opaque", "false");
                    tch.Props.Add("border", "false");
                    tch.Props.Add("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTransparentwbProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTransparentwbProfile");
                    tch.Props.Add("opaque", "false");
                    tch.Props.Add("border", "true");
                    tch.Props.Add("borderColor", @"""100 100 100""");
                    tch.Props.Add("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiGroupBorderProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiGroupBorderProfile");
                    tch.Props.Add("border", "false");
                    tch.Props.Add("opaque", "false");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.Props.Add("bitmap", @"""./core/art/gui/images/group-border""");
                    tch.Props.Add("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTabBorderProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTabBorderProfile");
                    tch.Props.Add("border", "false");
                    tch.Props.Add("opaque", "false");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/tab-border");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiGroupTitleProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiGroupTitleProfile");
                    tch.PropsAddString("fillColor", "242 241 240");
                    tch.PropsAddString("fillColorHL", "242 241 240");
                    tch.PropsAddString("fillColorNA", "242 241 240");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiToolTipProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiToolTipProfile");
                    tch.PropsAddString("fillColor", "239 237 222");
                    tch.PropsAddString("borderColor", "138 134 122");

                    tch.PropsAddString("fontType", "Arial");
                    tch.PropsAddString("fontSize", "14");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiModelessDialogProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiModelessDialogProfile");
                    tch.Props.Add("modal", "false");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiFrameSetProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiFrameSetProfile");
                    tch.PropsAddString("fillcolor", "255 255 255");
                    tch.PropsAddString("borderColor", "246 245 244");
                    tch.Props.Add("border", "1");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiWindowProfile"))
                    {
                    if (console.GetVarString("$platform") == "macos")
                        {
                        Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiWindowProfile");
                        tch.Props.Add("opaque", "false");
                        tch.Props.Add("border", "2");
                        tch.PropsAddString("fillColor", "242 241 240");
                        tch.PropsAddString("fillColorHL", "221 221 221");
                        tch.PropsAddString("fillColorNA", "200 200 200");
                        tch.PropsAddString("fontColor", "50 50 50");
                        tch.PropsAddString("fontColorHL", "0 0 0");
                        tch.PropsAddString("bevelColorHL", "255 255 255");
                        tch.PropsAddString("bevelColorLL", "0 0 0");
                        tch.PropsAddString("text", "untitled");
                        tch.PropsAddString("bitmap", "./core/art/gui/images/window");
                        tch.PropsAddString("textOffset", "8 4");
                        tch.Props.Add("hasBitmapArray", "true");
                        tch.PropsAddString("justify", "center");
                        tch.PropsAddString("category", "Core");
                        tch.CreateDB(m_ts);
                        }
                    else
                        {
                        Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiWindowProfile");
                        tch.Props.Add("opaque", "false");
                        tch.Props.Add("border", "2");
                        tch.PropsAddString("fillColor", "242 241 240");
                        tch.PropsAddString("fillColorHL", "221 221 221");
                        tch.PropsAddString("fillColorNA", "200 200 200");
                        tch.PropsAddString("fontColor", "50 50 50");
                        tch.PropsAddString("fontColorHL", "0 0 0");
                        tch.PropsAddString("bevelColorHL", "255 255 255");
                        tch.PropsAddString("bevelColorLL", "0 0 0");
                        tch.PropsAddString("text", "untitled");
                        tch.PropsAddString("bitmap", "./core/art/gui/images/window");
                        tch.PropsAddString("textOffset", "8 4");
                        tch.Props.Add("hasBitmapArray", "true");
                        tch.PropsAddString("justify", "true");
                        tch.PropsAddString("category", "Core");
                        tch.CreateDB(m_ts);
                        }
                    }
                if (!console.isObject("GuiToolbarWindowProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiToolbarWindowProfile : GuiWindowProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/toolbar-window");
                    tch.PropsAddString("text", "");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiToolbarWindowProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiToolbarWindowProfile : GuiWindowProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/toolbar-window");
                    tch.PropsAddString("text", "");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiHToolbarWindowProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiHToolbarWindowProfile : GuiWindowProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/htoolbar-window");
                    tch.PropsAddString("text", "");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiMenubarWindowProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiMenubarWindowProfile : GuiWindowProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/menubar-window");
                    tch.PropsAddString("text", "");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiWindowCollapseProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiWindowCollapseProfile : GuiWindowProfile");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiContentProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiContentProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "255 255 255");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiBlackContentProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiBlackContentProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "0 0 0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiInputCtrlProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiInputCtrlProfile");
                    tch.Props.Add("tab", "True");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextProfile");
                    tch.PropsAddString("justify", "left");
                    tch.PropsAddString("fontColor", "20 20 20");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextBoldProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextBoldProfile : GuiTextProfile");
                    tch.PropsAddString("fontType", "Arial Bold");
                    tch.Props.Add("fontSize", "16");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextBoldCenterProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextBoldCenterProfile : GuiTextProfile");
                    tch.PropsAddString("fontColor", "50 50 50");
                    tch.PropsAddString("fontType", "Arial Bold");
                    tch.Props.Add("fontSize", "16");
                    tch.PropsAddString("justify", "center");
                    tch.PropsAddString("category", "core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextRightProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextRightProfile : GuiTextProfile");
                    tch.PropsAddString("justify", "right");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextCenterProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextCenterProfile : GuiTextProfile");
                    tch.PropsAddString("justify", "center");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextSolidProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextSolidProfile : GuiTextProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "5");
                    tch.PropsAddString("borderColor", "GuiDefaultProfile.fillColor");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("InspectorTitleTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "InspectorTitleTextProfile");
                    tch.PropsAddString("fontColor", "100 100 100");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextProfileLight"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextProfileLight");
                    tch.PropsAddString("fontColor", "220 220 220");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiAutoSizeTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiAutoSizeTextProfile");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.Props.Add("autoSizeWidth", "true");
                    tch.Props.Add("autoSizeHeight", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextRightProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextRightProfile : GuiTextProfile");
                    tch.PropsAddString("justify", "right");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiMediumTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiMediumTextProfile : GuiTextProfile");
                    tch.Props.Add("fontSize", "24");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiBigTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiBigTextProfile : GuiTextProfile");
                    tch.Props.Add("fontSize", "36");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiMLTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiMLTextProfile");
                    tch.PropsAddString("fontColorLink", "100 100 100");
                    tch.PropsAddString("fontColorLinkHL", "255 255 255");
                    tch.Props.Add("autoSizeWidth", "true");
                    tch.Props.Add("autoSizeHeight", "true");
                    tch.Props.Add("border", "false");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextArrayProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextArrayProfile : GuiTextProfile");
                    tch.PropsAddString("fontColor", "50 50 50");
                    tch.PropsAddString("fontColorHL", " 0 0 0");
                    tch.PropsAddString("fontColorSEL", "0 0 0");
                    tch.PropsAddString("fillColor", "200 200 200");
                    tch.PropsAddString("fillColorHL", "228 228 235");
                    tch.PropsAddString("fillColorSEL", "200 200 200");
                    tch.Props.Add("border", "false");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextListProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextListProfile : GuiTextProfile");
                    tch.Props.Add("tab", "true");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiTextEditProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextEditProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/textEdit");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.Props.Add("border", "-2");
                    tch.PropsAddString("fillColor", "242 241 240 0");
                    tch.PropsAddString("fillColorHL", "255 255 255");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "255 255 255");
                    tch.PropsAddString("fontColorSEL", "98 100 137");
                    tch.PropsAddString("fontColorNA", "200 200 200");
                    tch.PropsAddString("textOffset", "4 2");
                    tch.Props.Add("autoSizeWidth", "false");
                    tch.Props.Add("autoSizeHeight", "true");
                    tch.PropsAddString("justify", "left");
                    tch.Props.Add("tab", "true");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiTreeViewRenameCtrlProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTreeViewRenameCtrlProfile : GuiTextEditProfile");
                    tch.Props.Add("returnTab", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextEditProfileNumbersOnly"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextEditProfileNumbersOnly : GuiTextEditProfile");
                    tch.Props.Add("numbersOnly", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextEditNumericProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextEditNumericProfile : GuiTextEditProfile");
                    tch.Props.Add("numbersOnly", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiNumericDropSliderTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiNumericDropSliderTextProfile : GuiTextEditProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/textEditSliderBox");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiTextEditDropSliderNumbersOnly"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextEditDropSliderNumbersOnly :  GuiTextEditProfile");
                    tch.Props.Add("numbersOnly", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/textEditSliderBox");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiProgressProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiProgressProfile");
                    tch.Props.Add("opaque", "false");
                    tch.PropsAddString("fillColor", "0 162 255 200");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("borderColor", "50 50 50 200");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiProgressBitmapProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiProgressBitmapProfile");
                    tch.Props.Add("border", "false");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/loadingbar");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiRLProgressBitmapProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiRLProgressBitmapProfile");
                    tch.Props.Add("border", "false");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/rl-loadingbar");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiProgressTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiProgressTextProfile");
                    tch.Props.Add("fontSize", "14");
                    tch.PropsAddString("fontType", "Arial");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("justify", "center");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiButtonProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiButtonProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("fontColor", "50 50 50");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "200 200 200");
                    tch.Props.Add("fixedExtent", "false");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/button");
                    tch.Props.Add("hasBitmapArray", "false");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiThumbHighlightButtonProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiThumbHighlightButtonProfile : GuiButtonProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/thumbHightlightButton");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("InspectorDynamicFieldButton"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "InspectorDynamicFieldButton : GuiButtonProfile");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiMenuButtonProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiMenuButtonProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "false");
                    tch.Props.Add("fontSize", "18");
                    tch.PropsAddString("fontType", "Arial Bold");
                    tch.PropsAddString("fontColor", "50 50 50");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "200 200 200");
                    tch.Props.Add("fixedExtent", "false");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("canKeyFocus", "false");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/selector-button");
                    tch.Props.Add("hasBitmapArray", "false");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiIconButtonProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiIconButtonProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("fontColor", "50 50 50");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "200 200 200");
                    tch.Props.Add("fixedExtent", "false");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("canKeyFocus", "false");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/iconbutton");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiIconButtonSolidProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiIconButtonSolidProfile : GuiIconButtonProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/iconbuttonsolid");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiIconButtonSmallProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiIconButtonSmallProfile : GuiIconButtonProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/iconbuttonsmall");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiButtonTabProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiButtonTabProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("fontColor", "50 50 50");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "0 0 0");
                    tch.Props.Add("fixedExtent", "false");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("canKeyFocus", "false");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/buttontab");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("EditorTabPage"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "EditorTabPage");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "false");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.Props.Add("fixedExtent", "false");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("canKeyFocus", "false");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/tab");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiCheckBoxProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiCheckBoxProfile");
                    tch.Props.Add("opaque", "false");
                    tch.PropsAddString("fillColor", "232 232 232");
                    tch.Props.Add("border", "false");
                    tch.PropsAddString("borderColor", "100 100 100");
                    tch.Props.Add("fontSize", "14");
                    tch.PropsAddString("fontColor", "20 20 20");
                    tch.PropsAddString("fontColorHL", "80 80 80");
                    tch.PropsAddString("fontColorNA", "200 200 200");
                    tch.Props.Add("fixedExtent", "true");
                    tch.PropsAddString("justify", "left");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/checkbox");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiCheckBoxListProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiCheckBoxListProfile : GuiCheckBoxProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/checkbox-list");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiCheckBoxListFlipedProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiCheckBoxListFlipedProfile : GuiCheckBoxProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/checkbox-list_fliped");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("InspectorCheckBoxTitleProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "InspectorCheckBoxTitleProfile : GuiCheckBoxProfile");
                    tch.PropsAddString("fontColor", "100 100 100");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiRadioProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiRadioProfile");
                    tch.Props.Add("fontSize", "14");
                    tch.PropsAddString("fillColor", "232 232 232");
                    tch.PropsAddString("fontColor", "20 20 20");
                    tch.PropsAddString("fontColorHL", "80 80 80");
                    tch.Props.Add("fixedExtent", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/radioButton");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiScrollProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiScrollProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillcolor", "255 255 255");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "150 150 150");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/scrollBar");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiOverlayProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiOverlayProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillcolor", "255 255 255");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "255 255 255");
                    tch.PropsAddString("fillColor", "0 0 0 100");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTransparentScrollProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTransparentScrollProfile");
                    tch.Props.Add("opaque", "false");
                    tch.PropsAddString("fillColor", "255 255 255");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.Props.Add("border", "false");
                    tch.Props.Add("borderThickness", "2");
                    tch.PropsAddString("borderColor", "100 100 100");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/scrollBar");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiSliderProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiSliderProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/slider");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiSliderBoxProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiSliderBoxProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/slider-w-box");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiPaneProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiPaneProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/popupMenu");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiPopupMenuItemBorder"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiPopupMenuItemBorder : GuiButtonProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "255 255 255");
                    tch.Props.Add("fixedExtent", "false");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("canKeyFocus", "false");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/button");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiPopUpMenuDefault"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiPopUpMenuDefault : GuiDefaultProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("mouseOverSelected", "true");
                    tch.PropsAddString("textOffset", "3 3");
                    tch.Props.Add("border", "0");
                    tch.Props.Add("borderThickness", "0");
                    tch.Props.Add("fixedExtent", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/scrollbar");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.Props.Add("profileForChildren", "GuiPopupMenuItemBorder");
                    tch.PropsAddString("fillColor", "242 241 240 ");
                    tch.PropsAddString("fillColorHL", "228 228 235");
                    tch.PropsAddString("fillColorSEL", "98 100 137");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorSEL", "255 255 255");
                    tch.PropsAddString("borderColor", "100 100 100");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiPopupBackgroundProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiPopupBackgroundProfile");
                    tch.Props.Add("modal", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiPopUpMenuProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiPopUpMenuProfile : GuiPopUpMenuDefault");
                    tch.PropsAddString("textOffset", "6 4");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/dropDown");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.Props.Add("border", "1");
                    tch.Props.Add("profileForChildren", "GuiPopUpMenuDefault");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiPopUpMenuTabProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiPopUpMenuTabProfile : GuiPopUpMenuDefault");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/dropDown-tab");
                    tch.PropsAddString("textOffset", "6 4");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.Props.Add("border", "1");
                    tch.Props.Add("profileForChildren", "GuiPopUpMenuDefault");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiPopUpMenuEditProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiPopUpMenuEditProfile : GuiPopUpMenuDefault");
                    tch.PropsAddString("textOffset", "6 4");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/dropDown");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.Props.Add("border", "1");
                    tch.Props.Add("profileForChildren", "GuiPopUpMenuDefault");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiListBoxProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiListBoxProfile");
                    tch.Props.Add("tab", "true");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTabBookProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTabBookProfile");
                    tch.PropsAddString("fillColorHL", "100 100 100");
                    tch.PropsAddString("fillColorNA", "150 150 150");
                    tch.PropsAddString("fontColor", "30 30 30");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "50 50 50");
                    tch.PropsAddString("fontType", "Arial");
                    tch.Props.Add("fontSize", "14");
                    tch.PropsAddString("justify", "center");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/tab");
                    tch.Props.Add("tabWidth", "64");
                    tch.Props.Add("tabHeight", "24");
                    tch.PropsAddString("tabPosition", "Top");
                    tch.PropsAddString("tabRotation", "Horizontal");
                    tch.PropsAddString("textOffset", "0 -3");
                    tch.Props.Add("tab", "true");
                    tch.Props.Add("cankeyfocus", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTabBookNoBitmapProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTabBookNoBitmapProfile : GuiTabBookProfile");
                    tch.PropsAddString("bitmap", "");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTabPageProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTabPageProfile : GuiDefaultProfile");
                    tch.PropsAddString("fontType", "Arial");
                    tch.Props.Add("fontSize", "10");
                    tch.PropsAddString("justify", "center");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/tab");
                    tch.Props.Add("opaque", "false");
                    tch.PropsAddString("fillColor", "240 239 238");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiMenuBarProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiMenuBarProfile");
                    tch.PropsAddString("fontType", "Arial");
                    tch.Props.Add("fontSize", "14");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "240 239 238");
                    tch.PropsAddString("fillColorHL", "202 201 200");
                    tch.PropsAddString("fillColorSEL", "202 0 0");
                    tch.PropsAddString("borderColorNA", "202 201 200");
                    tch.PropsAddString("borderColorHL", "50 50 50");
                    tch.Props.Add("border", "0");
                    tch.PropsAddString("fontColor", "20 20 20");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorNA", "255 255 255");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("canKeyFocus", "false");
                    tch.Props.Add("mouseOverSelected", "true");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/menu");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiConsoleProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiConsoleProfile");
                    tch.Props.Add("fontType", @"($platform $= ""macos"") ? ""Monaco"" : ""Lucida Console""");
                    tch.Props.Add("fontSize", @"($platform $= ""macos"") ? 13 : 12");
                    tch.PropsAddString("fontColor", "255 255 255");
                    tch.PropsAddString("fontColorHL", "0 255 255");
                    tch.PropsAddString("fontColorNA", "255 0 0");
                    tch.PropsAddString("fontColors[6]", "100 100 100");
                    tch.PropsAddString("fontColors[7]", "100 100 0");
                    tch.PropsAddString("fontColors[8]", "0 0 100");
                    tch.PropsAddString("fontColors[9]", "0 100 0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiConsoleTextEditProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiConsoleTextEditProfile : GuiTextEditProfile");
                    tch.Props.Add("fontType", @"($platform $= ""macos"") ? ""Monaco"" : ""Lucida Console""");
                    tch.Props.Add("fontSize", @"($platform $= ""macos"") ? 13 : 12");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiConsoleTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiConsoleTextProfile");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.Props.Add("autoSizeWidth", "true");
                    tch.Props.Add("autoSizeHeight", "true");
                    tch.PropsAddString("textOffset", "2 2");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "255 255 255");
                    tch.Props.Add("border", "true");
                    tch.Props.Add("borderThickness", "1");
                    tch.PropsAddString("borderColor", "0 0 0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTreeViewProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTreeViewProfile");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/treeView");
                    tch.Props.Add("autoSizeHeight", "true");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.PropsAddString("fillColor", "255 255 255");
                    tch.PropsAddString("fillColorHL", "228 228 235");
                    tch.PropsAddString("fillColorSEL", "98 100 137");
                    tch.PropsAddString("fillColorNA", "255 255 255 ");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "0 0 0");
                    tch.PropsAddString("fontColorSEL", "255 255 255");
                    tch.PropsAddString("fontColorNA", "200 200 200");
                    tch.PropsAddString("borderColor", "128 000 000");
                    tch.PropsAddString("borderColorHL", "255 228 235");
                    tch.Props.Add("fontSize", "14");
                    tch.Props.Add("opaque", "false");
                    tch.Props.Add("border", "false");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiSimpleTreeProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiSimpleTreeProfile : GuiTreeViewProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "255 255 255 255");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("GuiText24Profile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiText24Profile : GuiTextProfile");
                    tch.Props.Add("fontSize", "24");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiRSSFeedMLTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiRSSFeedMLTextProfile");
                    tch.PropsAddString("fontColorLink", "55 55 255");
                    tch.PropsAddString("fontColorLinkHL", "255 55 55");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                console.SetVar("$ConsoleDefaultFillColor", "0 0 0 175");

                if (!console.isObject("ConsoleScrollProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "ConsoleScrollProfile : GuiScrollProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("fillColor", "$ConsoleDefaultFillColor");
                    tch.Props.Add("border", "1");
                    tch.PropsAddString("borderColor", "0 0 0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("ConsoleTextEditProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "ConsoleTextEditProfile : GuiTextEditProfile");
                    tch.PropsAddString("fillColor", "242 241 240 255");
                    tch.PropsAddString("fillColorHL", "255 255 255");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTextPadProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTextPadProfile");
                    tch.Props.Add("fontType", @"($platform $= ""macos"") ? ""Monaco"" : ""Lucida Console""");
                    tch.Props.Add("fontSize", @"($platform $= ""macos"") ? 13 : 12");
                    tch.Props.Add("tab", "true");
                    tch.Props.Add("canKeyFocus", "true");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "255 255 255");
                    tch.Props.Add("Border", "0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiTransparentProfileModeless"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiTransparentProfileModeless : GuiTransparentProfile");
                    tch.Props.Add("modal", "false");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiFormProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiFormProfile : GuiTextProfile");
                    // tch.Props.Add("opaque", "false");
                    tch.Props.Add("border", "5");
                    //tch.PropsAddString("bitmap", "./core/art/gui/images/form");
                    tch.Props.Add("hasBitmapArray", "true");
                    tch.PropsAddString("justify", "center");
                    tch.Props.Add("profileForChildren", "GuiButtonProfile");
                    tch.Props.Add("opaque", "false");
                    tch.PropsAddString("bitmap", "./core/art/gui/images/button");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiNumericTextEditSliderProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiNumericTextEditSliderProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "0 0 0 0");
                    tch.PropsAddString("fillColorHL", "255 255 255");

                    tch.Props.Add("border", "true");
                    tch.Props.Add("tab", "false");
                    tch.Props.Add("canKeyFocus", "true");

                    tch.PropsAddString("fontType", "Arial");
                    tch.Props.Add("fontSize", "14");

                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorSEL", "43 107 206");
                    tch.PropsAddString("fontColorHL", "244 244 244");
                    tch.PropsAddString("fontColorNA", "100 100 100");

                    tch.Props.Add("numbersOnly", "true");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiNumericTextEditSliderBitmapProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiNumericTextEditSliderBitmapProfile");
                    tch.Props.Add("opaque", "true");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("borderColor", "100 100 100");
                    tch.Props.Add("tab", "false");
                    tch.Props.Add("canKeyFocus", "true");

                    tch.PropsAddString("fontType", "Arial");
                    tch.Props.Add("fontSize", "14");

                    tch.PropsAddString("fillColor", "242 241 240");
                    tch.PropsAddString("fillColorHL", "255 255 255");
                    tch.PropsAddString("fontColor", "0 0 0");
                    tch.PropsAddString("fontColorHL", "255 255 255");
                    tch.PropsAddString("fontColorSEL", "98 100 137");
                    tch.PropsAddString("fontColorNA", "200 200 200");

                    tch.Props.Add("numbersOnly", "true");
                    tch.Props.Add("hasBitmapArray", "true");

                    tch.PropsAddString("bitmap", "./core/art/gui/images/numericslider");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiMultiFieldTextEditProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiMultiFieldTextEditProfile : GuiTextEditProfile");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("GuiModalDialogBackgroundProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "GuiModalDialogBackgroundProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "221 221 221 150");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }
                if (!console.isObject("CenterPrintProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "CenterPrintProfile");
                    tch.Props.Add("opaque", "true");
                    tch.PropsAddString("fillColor", "128 128 128");
                    tch.PropsAddString("fontColor", "0 255 0");
                    tch.Props.Add("border", "true");
                    tch.PropsAddString("borderColor", "0 255 0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }

                if (!console.isObject("CenterPrintTextProfile"))
                    {
                    Torque_Class_Helper tch = new Torque_Class_Helper("GuiControlProfile", "CenterPrintTextProfile");
                    tch.Props.Add("opaque", "false");
                    tch.PropsAddString("fontType", "Arial");
                    tch.Props.Add("fontSize", "12");
                    tch.PropsAddString("fontColor", "0 255 0");
                    tch.PropsAddString("category", "Core");
                    tch.CreateDB(m_ts);
                    }




                }
            }
        }
    }
