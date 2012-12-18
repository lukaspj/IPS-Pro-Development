using WinterLeaf.Classes;
using System.IO;
using System.Windows.Forms;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        //------------------------------------------------------------------------------
        // Loading info is text displayed on the client side while the mission
        // is being loaded.  This information is extracted from the mission file
        // and sent to each the client as it joins.
        //------------------------------------------------------------------------------

        //------------------------------------------------------------------------------
        // clearLoadInfo
        //
        // Clears the mission info stored
        //------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "clearLoadInfo", "()", 0, 12200, false)]
        public void ClearLoadInfo()
            {
            if (console.isObject("theLevelInfo"))
                console.Call("theLevelInfo", "delete");
            }

        //------------------------------------------------------------------------------
        // buildLoadInfo
        //
        // Extract the map description from the .mis file
        //------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "buildLoadInfo", "(%mission)", 1, 12200, false)]
        public void BuildLoadInfo(string mission)
            {
            //Replaced the torque file stuff w/ csharp, less stuff inside of torque.
            ClearLoadInfo();
            string missionpath = Path.GetDirectoryName(Application.ExecutablePath) + "\\" + mission.Replace("/", "\\");
            if (File.Exists(missionpath))
                {
                string infoObject = "";
                using (StreamReader sr = new StreamReader(missionpath))
                    {

                    bool inInfoBlock = false;
                    while (sr.Peek() >= 0)
                        {
                        string line = sr.ReadLine();
                        if (line.Trim().StartsWith("new ScriptObject(MissionInfo) {"))
                            inInfoBlock = true;
                        if (line.Trim().StartsWith("new LevelInfo(theLevelInfo) {"))
                            inInfoBlock = true;
                        else if (inInfoBlock && line.Trim().StartsWith("};"))
                            {
                            inInfoBlock = false;
                            infoObject += line;
                            break;
                            }
                        if (inInfoBlock)
                            infoObject += line + " ";

                        }

                    }
                console.Eval(infoObject);
                }
            else
                {
                console.error(string.Format("Level File {0} not found.", mission));
                }
            }
        //------------------------------------------------------------------------------
        // dumpLoadInfo
        //
        // Echo the mission information to the console
        //------------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "dumpLoadInfo", "()", 0, 12200, false)]
        public void DumpLoadInfo()
            {
            console.print(string.Format("Level Name: {0}", console.GetVarString("theLevelInfo.name")));
            console.print("Level Description:");
            for (int i = 0; console.GetVarString(string.Format("theLevelInfo.desc[{0}]", i)) != ""; i++)
                console.print("     " + console.GetVarString(string.Format("theLevelInfo.desc[{0}]", i)));
            }
     
        }
    }