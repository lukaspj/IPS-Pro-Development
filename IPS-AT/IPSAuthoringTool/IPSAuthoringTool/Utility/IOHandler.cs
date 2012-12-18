using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

using WinterLeaf;
using WinterLeaf.Enums;
using WinterLeaf.Containers;
using WinterLeaf.Classes;

namespace IPSAuthoringTool.Utility
{
    public partial class IOHandler : TorqueScriptTemplate
    {
        public static void Write(ParticleEffect effect, Dialogs.AlertDialog dia)
        {
            effect.WriteToFile("creations/ParticleEffects/" + effect.name + ".pEffect", dia);
        }

        public static void WriteAndExec(UtilObject Util, ParticleEffect effect, Dialogs.AlertDialog dia)
        {
            effect.WriteToFile("creations/ParticleEffects/" + effect.name + ".pEffect", dia);
            if(Util != null)
                Util.exec("creations/ParticleEffects.cs", false, false);
        }

        public static void writeDatablockScript(List<ParticleEffect> effects)
        {
            StreamWriter SW = new StreamWriter("creations/ParticleEffects.cs");
            foreach (ParticleEffect PE in effects)
            {
                SW.WriteLine("datablock ParticleEffectData(" + PE.name + ")");
                SW.WriteLine("{");
                SW.WriteLine("\tpEffect = \"./ParticleEffects/" + PE.name + ".pEffect\";");
                SW.WriteLine("\tlifeTimeMS = 5000;");
                SW.WriteLine("};");
                SW.WriteLine();
            }
            SW.Close();
        }

        public static List<string> readStringDataFile(string path)
        {
            List<string> ret = new List<string>();
            StreamReader SR = new StreamReader(path);
            while (!SR.EndOfStream)
            {
                ret.Add(SR.ReadLine());
            }
            SR.Close();
            return ret;
        }

        public static void writeStringDataFile(Dictionary<string, List<string>> dict, string path)
        {
            StreamWriter SW = new StreamWriter(path);
            foreach (string s in dict.Keys)
            {
                foreach (string ss in dict[s])
                {
                    SW.WriteLine(s + "|" + ss);
                }
            }
            SW.Close();
        }
    }
}
