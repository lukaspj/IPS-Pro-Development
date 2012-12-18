﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
namespace DNT_FPS_Demo_Game_Dll.Scripts
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "pushFront", "%list, %token, %delim", 3, 1800, false)]
        public string pushFront(string list, string token, string delim)
            {
            if (list != "")
                return token + delim + list;
            return token;
            }
        [Torque_Decorations.TorqueCallBack("", "", "pushBack", "%list, %token, %delim", 3, 1810, false)]
        public string pushBack(string list, string token, string delim)
            {
            if (list != "")
                return list + delim + token;
            return token;
            }

        [Torque_Decorations.TorqueCallBack("", "", "popFront", "%list,  %delim", 2, 1820, false)]
        public string popFront(string list, string delim)
            {
            string unused = "";
            return Util._nextToken(list, unused, delim);
            }
        [Torque_Decorations.TorqueCallBack("", "", "defaultParseArgs", "", 0, 1830, false)]
        public void defaultParseArgs()
            {
            int argc = console.GetVarInt("$Game::argc");

            for (int i = 1; i < argc; i++)
                {
                string arg = console.GetVarString("$Game::argv[" + i.AsString() + "]").Trim().ToLower();
                string nextArg = console.GetVarString("$Game::argv[" + (i + 1).AsString() + "]").Trim().ToLower();
                bool hasNextArg = argc - i > 1;
                bool logModeSpecified = false;

                // Check for dedicated run
                if (arg == "-dedicated")
                    {
                    console.SetVar("$userDirs", console.GetVarString("$defaultGame"));
                    console.SetVar("$dirCount", 1);
                    console.SetVar("$isDedicated", true);
                    }
                switch (arg)
                    {
                    case "-log":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            // Turn on console logging
                            if (nextArg.AsInt() != 0)
                                nextArg = (nextArg.AsInt() + 5).AsString();
                            Util.setLogMode(nextArg.AsInt());
                            logModeSpecified = true;
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -log <Mode: 0,1,2>");
                            }
                        break;
                    case "-dir":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            // Append the mod to the end of the current list
                            console.SetVar("$userDirs", Util.strreplace(console.GetVarString("$userDirs"), nextArg, ""));
                            console.SetVar("$userDirs", pushFront(console.GetVarString("$useDirs"), nextArg, ";"));
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            console.SetVar("$dirCount", console.GetVarInt("$dirCount") + 1);
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -dir <dir_name>");
                            }
                        break;
                    //--------------------
                    // changed the default behavior of this command line arg. It now
                    // defaults to ONLY loading the game, not tools 
                    // default auto-run already loads in tools --SRZ 11/29/07
                    case "-game":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            // Set the selected dir --NOTE: we no longer allow tools with this argument
                            /* 
                        if( $isDedicated )
                        {
                           $userDirs = $nextArg;
                           $dirCount = 1;
                        }
                        else
                        {
                           $userDirs = "tools;" @ $nextArg;
                           $dirCount = 2;
                        }
                        */
                            console.SetVar("$userDirs", nextArg);
                            console.SetVar("$dirCount", 1);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            console.error(console.GetVarString("$userDirs"));
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -game <game_name>");
                            }
                        break;
                    case "-console":
                        console.Call("enableWinConsole", new string[] { "true" });
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-jSave":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.print("Saving event log to journal: " + nextArg);
                            console.Call("saveJournal", new string[] { nextArg });
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -jSave <journal_name>");
                            }
                        break;
                    case "-jPlay":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.Call("playJournal", new string[] { nextArg, "false" });
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -jPlay <journal_name>");
                            }
                        break;
                    case "-jPlayToVideo":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$VideoCapture::journalName", nextArg);
                            console.SetVar("$VideoCapture::captureFromJournal", true);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -jPlayToVideo <journal_name>");
                            }
                        break;
                    case "-vidCapFile":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$VideoCapture::fileName", nextArg);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -vidCapFile <ouput_video_name>");
                            }
                        break;
                    case "-vidCapFPS":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$VideoCapture::fps", nextArg);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -vidCapFPS <ouput_video_framerate>");
                            }
                        break;
                    case "-vidCapEncoder":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$VideoCapture::encoder", nextArg);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -vidCapEncoder <ouput_video_encoder>");
                            }
                        break;
                    case "-vidCapWidth":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$videoCapture::width", nextArg);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -vidCapWidth <ouput_video_width>");
                            }
                        break;
                    case "-vidCapHeight":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$videoCapture::height", nextArg);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -vidCapHeight <ouput_video_height>");
                            }
                        break;
                    case "-jDebug":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.Call("playJournal", new string[] { nextArg, "true" });
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -jDebug <journal_name>");
                            }
                        break;
                    case "-level":
                        string levelToLoad = "";
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            int hasExt = Util.strpos(nextArg, ".mis", 0);
                            if (hasExt == -1)
                                {
                                levelToLoad = nextArg + " ";
                                for (int ii = i + 2; ii < argc; ii++)
                                    {
                                    arg = console.GetVarString("$Game::argv[" + ii.AsString() + "]");
                                    hasExt = Util.strpos(arg, ".mis", 0);
                                    if (hasExt == -1)
                                        {
                                        levelToLoad = levelToLoad + arg + " ";
                                        }
                                    else
                                        {
                                        levelToLoad = levelToLoad + arg;
                                        break;
                                        }
                                    }
                                }
                            else
                                {
                                levelToLoad = nextArg;
                                }
                            console.SetVar("$levelToLoad", levelToLoad);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -level <level file name (no path), with or without extension>");
                            }
                        break;
                    case "-worldeditor":
                        console.SetVar("$startWorldEditor", true);
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-guieditor":
                        console.SetVar("$startGUIEditor", true);
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-help":
                        console.SetVar("$displayHelp", true);
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-compileAll":
                        console.SetVar("$compileAll", true);
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-compileTools":
                        console.SetVar("$compileTools", true);
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-genScript":
                        console.SetVar("$genScript", true);
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    //------------------------
                    case "-fullscreen":
                        console.Call("setFullScreen", new string[] { "true" });
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-windowed":
                        console.Call("setFullScreen", new string[] { "false" });
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-openGL":
                        console.SetVar("$pref::Video::displayDevice", "OpenGL");
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-directX":
                        console.SetVar("$pref::Video::displayDevice", "D3D");
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-voodoo2":
                        console.SetVar("$pref::Video::displayDevice", "Voodoo2");
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-autoVideo":
                        console.SetVar("$pref::Video::displayDevice", "");
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-prefs":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            Util.exec(nextArg, true, true);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -prefs <path/script.cs>");
                            }
                        break;
                    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                    case "-dedicated":
                        console.SetVar("$Server::Dedicated", true);
                        console.Call("enableWinConsole", new string[] { "true" });
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        break;
                    case "-mission":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$missionArg", nextArg);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -mission <filename>");
                            }
                        break;
                    case "-connect":
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);
                        if (hasNextArg)
                            {
                            console.SetVar("$JoinGameAddress", nextArg);
                            console.SetVar("$argUsed[" + (i + 1).AsString() + "]", console.GetVarInt("$argUsed[" + (i + 1).AsString() + "]") + 1);
                            i++;
                            }
                        else
                            {
                            console.error("Error: Missing Command Line argument. Usage: -connect <ip_address>");
                            }
                        break;
                    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                    //------------------------
                    default:
                        console.SetVar("$argUsed[" + i.AsString() + "]", console.GetVarInt("$argUsed[" + i.AsString() + "]") + 1);

                        if (console.GetVarString("$userDirs") == "")
                            console.SetVar("$userDirs", arg);
                        break;
                    }
                }
            //-----------------------------------------------
            // Play journal to video file? 
            if (!console.GetVarBool("$VideoCapture::captureFromJournal") || console.GetVarString("$VideoCapture::journalName") == "") return;
            if (console.GetVarString("$VideoCapture::fileName") == "")
                console.SetVar("$VideoCapture::fileName", console.GetVarString("$VideoCapture::journalName"));

            if (console.GetVarString("$VideoCapture::encoder") == "")
                console.SetVar("$VideoCapture::encoder", "THEORA");

            if (console.GetVarString("$VideoCapture::fps") == "")
                console.SetVar("$VideoCapture::fps", 30);
            if (console.GetVarString("$videoCapture::width") == "")
                console.SetVar("$videoCapture::width", 0);
            if (console.GetVarString("$videoCapture::height") == "")
                console.SetVar("$videoCapture::height", 0);

            Util.playJournalToVideo(console.GetVarString("$VideoCapture::journalName"), console.GetVarString("$VideoCapture::fileName"), console.GetVarString("$VideoCapture::encoder"),
                                    console.GetVarFloat("$VideoCapture::fps"), new Point2I(console.GetVarString("$videoCapture::width") + " " + console.GetVarString("$videoCapture::height")));
            }
        }
    }
