﻿using System;
using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // Message Hud
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "chathud_load", "()",  0, 4000, true)]
        public string ChathudLoad()
            {
            // chat hud sizes in lines
            console.SetVar("$outerChatLenY[1]", 4);
            console.SetVar("$outerChatLenY[2]", 9);
            console.SetVar("$outerChatLenY[3]", 13);
            // Only play sound files that are <= 5000ms in length.
            console.SetVar("$MaxMessageWavLength", 5000);
            new Torque_Class_Helper("MessageVector", "HudMessageVector").Create(m_ts);
            console.SetVar("$LastHudTarget", 0);
            return string.Empty;
            }

        // Helper function to play a sound file if the message indicates.
        // Returns starting position of wave file indicator.
        [Torque_Decorations.TorqueCallBack("", "", "playMessageSound", "(%message, %voice, %pitch)",  3, 4000, false)]
        public string PlayMessageSound(string message, string voice, string pitch)
            {
            int wavstart = Util.strstr(message, "~w");
            if (wavstart == -1)
                return "-1";

            string wav = message.Substring(wavstart + 2, 1000);

            string wavFile = voice != "" ? string.Format("art/sound/voice/{0}/{1}", voice, wav) : string.Format("art/sound/{0}", wav);

            string wavSource = "";

            // XXX This only expands to a single filepath, of course; it
            // would be nice to support checking in each mod path if we
            // have multiple mods active.

            if (Util.strstr(wavFile, ".wav") != wavFile.Length - 4)
                wavFile = wavFile + ".wav";

            //wavSource = con.Call("ExpandFilename", new[] {wavFile});
            wavSource = Util._expandFilename(wavFile);

            if (console.isObject(wavSource))
                {
                    {
                    int wavLengthMs = console.Call(wavSource, "getDuration").AsInt()*pitch.AsInt();

                    if (wavLengthMs == 0)
                        console.error(string.Format("** WAV file \"{0}\" is nonexistent or sound is zero-length **", wavFile));
                    else if (wavLengthMs <= console.GetVarInt("$MaxMessageWavLength"))
                        {
                        if (console.isObject("$ClientChatHandle[0]"))
                            console.Call("$ClientChatHandle[0]", "delete");

                        console.SetVar("$ClientChatHandle[0]", wavSource);

                        if (pitch.AsInt() != 1)
                            console.Call("$ClientChatHandle[0]", "setPitch", new[] {pitch});

                        console.Call("$ClientChatHandle[0]", "play");
                        }
                    else
                        {
                        console.error(string.Format("** WAV file \"{0}\" is too long **", wavFile));
                        }
                    }
                }
            else
                {
                console.error(string.Format("** Unable to load WAV file : \"{0}\" **", wavFile));
                }
            return wavstart.AsString();
            }

        [Torque_Decorations.TorqueCallBack("", "", "onChatMessage", "(%message, %voice, %pitch)",  3, 4000, false)]
        public string OnChatMessage(string message, string voice, string pitch)
            {
            // XXX Client objects on the server must have voiceTag and voicePitch
            // fields for values to be passed in for %voice and %pitch... in
            // this example mod, they don't have those fields.

            // Clients are not allowed to trigger general game sounds with their
            // chat messages... a voice directory MUST be specified.

            if (voice == "")
                voice = "default";

            // See if there's a sound at the end of the message, and play it.
            string wavStart = PlayMessageSound(message, voice, pitch);
            if (wavStart.AsInt() != -1)
                message = message.Substring(0, wavStart.AsInt());

            if (message.Trim() != "")
                ChatHudAddLine("ChatHud", message);


            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "onServerMessage", "(%message)",  1, 4000, false)]
        public string OnServerMessage(string message)
            {
            string wavStart = PlayMessageSound(message, "", "");
            if (wavStart.AsInt() != -1)
                message = message.Substring(0, wavStart.AsInt());

            if (message.Trim() != "")
                ChatHudAddLine("ChatHud", message);

            return string.Empty;
            }


        //-----------------------------------------------------------------------------
        // MainChatHud methods
        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "MainChatHud", "onServerMessage", "(%this)",  1, 4000, false)]
        public string MainChatHudOnServerMessage(string thisobj)
            {
            // set the chat hud to the users pref
            MainChatHudSetChatHudLength(thisobj, console.GetVarString("$Pref::ChatHudLength"));
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "MainChatHud", "setChatHudLength", "( %this, %length )",  2, 4000, false)]
        public string MainChatHudSetChatHudLength(string thisobj, string length)
            {
            int textheight = console.GetVarInt("ChatHud.Profile.fontSize") + console.GetVarInt("ChatHud.lineSpacing");
            if (textheight <= 0)
                {
                textheight = 12;
                }

            int lengthInPixels = console.GetVarInt("$outerChatLenY[" + length + "]")*textheight;
            int chatMargin = Util.getWord(console.GetVarString("OuterChatHud.extent"), 1).AsInt() -
                             Util.getWord(console.GetVarString("ChatScrollHud.Extent"), 1).AsInt() + 2*console.GetVarInt("ChatScrollHud.profile.borderThickness");

            
            console.Call("OuterChatHud", "setExtent", new[] {Util.firstWord(console.GetVarString("OuterChatHud.extent")), (lengthInPixels + chatMargin).AsString()});
            console.Call("ChatScrollHud", "scrollToBottom");
            GuiControl.setVisible("ChatPageDown",false);
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "MainChatHud", "nextChatHudLen", "( %this)",  1, 4000, false)]
        public string MainChatHudNextChatHudLen(string thisobj)
            {
            int len = console.GetVarInt("$pref::ChatHudLength");
            len++;
            if (len == 4)
                len = 1;
            console.Call(thisobj, "setChatHudLength", new[] {len.AsString()});
            console.SetVar("$pref::ChatHudLength", len);
            return string.Empty;
            }

        //-----------------------------------------------------------------------------
        // ChatHud methods
        // This is the actual message vector/text control which is part of
        // the MainChatHud dialog
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "ChatHud", "addLine", "(%this,%text)",  2, 4000, false)]
        public string ChatHudAddLine(string thisobj, string text)
            {
            int textHeight = console.GetVarInt(string.Format("{0}.profile.fontSize", thisobj)) + console.GetVarInt(string.Format("{0}.lineSpacing", thisobj));
            if (textHeight <= 0)
                textHeight = 12;

            string scrollBox = console.Call(thisobj, "getGroup");
            //See if the two are eqaul, and if they are use the split instead of the getword.
            //int chatScrollHeight = Util.getWord(con.GetVarString(string.Format("{0}.extent", scrollBox)), 1).AsInt() -
            //                       2 * con.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));

            int chatScrollHeight = console.GetVarString(string.Format("{0}.extent", scrollBox)).Split(' ')[1].AsInt() -
                                   2*console.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));

            int chatPosition = Util.getWord(console.GetVarString(string.Format("{0}.extent", thisobj)), 1).AsInt() - chatScrollHeight +
                               Util.getWord(console.GetVarString(string.Format("{0}.position", thisobj)), 1).AsInt() -
                               console.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));

            int linesToScroll = (int) Math.Floor(((chatPosition)/((double) textHeight) + .5));

            string origPosition = "";
            if (linesToScroll > 0)
                origPosition = console.GetVarString(string.Format("{0}.position", thisobj));

            //remove old messages from the top only if scrolled down all the way
            while (!console.Call("chatPageDown", "isVisible").AsBool() && console.Call("HudMessageVector", "getNumLines").AsBool() &&
                   (console.Call("HudMessageVector", "getNumLines").AsInt() >= console.GetVarInt("$pref::HudMessageLogSize")))
                {



                string tag = MessageVector.getLineTag("HudMessageVector", 0).AsString(); 
                if (tag.AsInt() != 0)
                    console.Call(tag, "delete");
                MessageVector.popFrontLine("HudMessageVector");

                }
            //add the message...
            MessageVector.pushBackLine("HudMessageVector", text, console.GetVarInt("$LastHudTarget"));
            console.SetVar("$LastHudTarget", 0);
            //now that we've added the message, see if we need to reset the position
            if (linesToScroll > 0)
                {
                GuiControl.setVisible("chatPageDown",true);
                console.SetVar(thisobj + ".position", origPosition);
                }
            else
                GuiControl.setVisible("chatPageDown",false);
            return string.Empty;
            }

        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "ChatHud", "pageUp", "(%this)",  1, 4000, false)]
        public string ChatHudPageUp(string thisobj)
            {
            int textHeight = console.GetVarInt(string.Format("{0}.profile.fontSize", thisobj)) + console.GetVarInt(string.Format("{0}.lineSpacing", thisobj));
            if (textHeight <= 0)
                textHeight = 12;

            string scrollBox = console.Call(thisobj, "getGroup");

            //int chatScrollHeight = Util.getWord(con.GetVarString(string.Format("{0}.extent", scrollBox)), 1).AsInt() -
            //                       2 * con.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));
            int chatScrollHeight = console.GetVarString(string.Format("{0}.extent", scrollBox)).Split(' ')[1].AsInt() -
                                   2*console.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));
            if (chatScrollHeight <= 0)
                return string.Empty;

            int pageLines = (int) Math.Floor(chatScrollHeight/(double) textHeight) - 1;
            if (pageLines <= 0)
                pageLines = 1;

            // See how many lines we actually can scroll up:
            int chatPosition = -1*console.GetVarString(string.Format("{0}.position", thisobj)).Split('0')[1].AsInt() -
                               console.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));
            int linesToScroll = (int) (Math.Floor(chatPosition/(double) textHeight) + .5);
            if (linesToScroll <= 0)
                return string.Empty;

            int scrollLines = 0;
            scrollLines = linesToScroll > pageLines ? pageLines : linesToScroll;

            string position = console.GetVarString(thisobj + ".position");

            // Now set the position
            console.SetVar(string.Format("{0}.position", thisobj),
                       string.Format("{0} {1} {2}", position.Split(' ')[0], position.Split(' ')[1], (scrollLines*textHeight).AsString()));
            // Display the pageup icon
            GuiControl.setVisible("chatPageDown",true);
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "ChatHud", "pageDown", "(%this)",  1, 4000, false)]
        public string ChatHudPageDown(string thisobj)
            {
            int textHeight = console.GetVarInt(string.Format("{0}.profile.fontSize", thisobj)) + console.GetVarInt(string.Format("{0}.lineSpacing", thisobj));
            if (textHeight <= 0)
                textHeight = 12;

            string scrollBox = console.Call(thisobj, "getGroup");

            //int chatScrollHeight = Util.getWord(con.GetVarString(string.Format("{0}.extent", scrollBox)), 1).AsInt() -
            //                       2 * con.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));
            int chatScrollHeight = console.GetVarString(string.Format("{0}.extent", scrollBox)).Split(' ')[1].AsInt() -
                                   2*console.GetVarInt(string.Format("{0}.profile.borderThickness", scrollBox));
            if (chatScrollHeight <= 0)
                return string.Empty;

            int pageLines = (int) Math.Floor(chatScrollHeight/(double) textHeight) - 1;
            if (pageLines <= 0)
                pageLines = 1;

            int chatPosition = console.GetVarString(thisobj + ".extent").Split(' ')[1].AsInt() - chatScrollHeight +
                               console.GetVarString(thisobj + ".position").Split(' ')[1].AsInt() - console.GetVarInt(scrollBox + ".profile.borderThickness");

            int linesToScroll = (int) (Math.Floor(chatScrollHeight/(double) textHeight) + .5);
            if (linesToScroll <= 0)
                return string.Empty;

            int scrollLines = linesToScroll > pageLines ? pageLines : linesToScroll;

            // Now set the position
            string pos = console.GetVarString(thisobj + ".position");
            console.SetVar(string.Format("{0}.position", thisobj),
                       string.Format("{0} {1}", pos.Split(' ')[0], (pos.Split(' ')[1].AsInt() - (scrollLines*textHeight)).AsString()));

            // See if we have should (still) display the pagedown icon
            GuiControl.setVisible("chatPageDown",scrollLines < linesToScroll ? true : false);
            return string.Empty;
            }

        //-----------------------------------------------------------------------------
        // Support functions
        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "pageUpMessageHud", "()",  0, 4000, false)]
        public string PageUpMessageHud()
            {
            ChatHudPageUp("ChatHud");
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "pageDownMessageHud", "()",  0, 4000, false)]
        public string PageDownMessageHud()
            {
            ChatHudPageDown("ChatHud");
            return string.Empty;
            }

        [Torque_Decorations.TorqueCallBack("", "", "cycleMessageHudSize", "()",  0, 4000, false)]
        public string CycleMessageHudSize()
            {
            MainChatHudNextChatHudLen("MainChatHud");
            return string.Empty;
            }
        }
    }