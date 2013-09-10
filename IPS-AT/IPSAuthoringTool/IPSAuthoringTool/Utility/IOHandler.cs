using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
            effect.WriteToFile("creations/ParticleEffects/" + effect.Name + ".pEffect", dia);
        }

        public static void WriteAndExec(UtilObject Util, ParticleEffect effect, Dialogs.AlertDialog dia, Windows.SceneSetupEditor SSE)
        {
            effect.WriteToFile("creations/ParticleEffects/" + effect.Name + ".pEffect", dia);
            if(Util != null)
                Util.exec("creations/ParticleEffects.cs", false, false);
            WriteStaticShapeScript(SSE);
            Util.exec("creations/StaticShapes.cs", false, false);
        }

        public static void writeDatablockScript(Collection<ParticleEffect> effects)
        {
            StreamWriter SW = new StreamWriter("creations/ParticleEffects.cs");
            foreach (ParticleEffect PE in effects)
            {
                SW.WriteLine("datablock ParticleEffectData(" + PE.Name + ")");
                SW.WriteLine("{");
                SW.WriteLine("\tpEffect = \"./ParticleEffects/" + PE.Name + ".pEffect\";");
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

        public static void WriteSceneFile(Windows.SceneSetupEditor SSE)
        {
            StreamWriter SW = new StreamWriter("scene.dat");
            SW.WriteLine(SSE.SelectedModel);
            SW.WriteLine("Seqs Begin---");
            foreach (Utility.Sequence seq in SSE.SequenceDict.Values)
                if(seq != null)
                    SW.WriteLine(seq.ToString());
            SW.WriteLine("---Seqs End");
            SW.WriteLine("SelSeqs Begin---");
            List<Sequence> selSeqs = SSE.GetSelectedSequences();
            if(selSeqs != null)
            {
                foreach (Utility.Sequence seq in selSeqs)
                    SW.WriteLine(seq.File);
            }
            SW.WriteLine("---SelSeqs End");
            SW.WriteLine("Models Begin---");
            foreach (Utility.Model s in SSE.Models)
                SW.WriteLine(s.AsString());
            SW.WriteLine("---Models End");
            SW.WriteLine(SSE.SelectedScene);
            SW.Close();
        }

        public static void ReadSceneFile(Windows.SceneSetupEditor SSE)
        {
            if (!File.Exists("scene.dat"))
                return;
            StreamReader SR = new StreamReader("scene.dat");
            string selModPath = Path.GetFileNameWithoutExtension(SR.ReadLine());
            string line = SR.ReadLine();
            if (!line.Equals("Seqs Begin---"))
                return;
            SSE.Sequences.Clear();
            line = SR.ReadLine();
            while (!line.Equals("---Seqs End"))
            {
                SSE.Sequences.Add(Utility.Sequence.FromString(line));
                line = SR.ReadLine();
            }
           line = SR.ReadLine();
           if (!line.Equals("SelSeqs Begin---"))
                return;
            line = SR.ReadLine();
            while (!line.Equals("---SelSeqs End"))
            {
                SSE.SelectSequence(line);
                line = SR.ReadLine();
            }
            line = SR.ReadLine();
            if (!line.Equals("Models Begin---"))
                return;
            line = SR.ReadLine();
            while (!line.Equals("---Models End"))
            {
                Model model = Model.FromString(line);
                if (model.getPath() == selModPath)
                    SSE.SelectedModel = model;
                SSE.Models.Add(model);
                line = SR.ReadLine();
            }
            SSE.SelectedScene = SR.ReadLine();
            SR.Close();
        }

        public static void WriteStaticShapeScript(Windows.SceneSetupEditor SSE)
        {
            if (SSE.SelectedModel == null)
                return;
            StreamWriter SW = new StreamWriter("creations/StaticShapes.cs");
            SW.Write(
                "singleton TSShapeConstructor(modeldts)\n"+
                "{\n"+
                    "\tbaseShape = \"" + SSE.SelectedModel.getPath().Replace('\\', '/') + "\";\n" +/*
                    "\tloadLights = \"0\";\n" +
                    "\tunit = \"1\";\n" +
                    "\tupAxis = \"DEFAULT\";\n" +
                    "\tlodType = \"TrailingNumber\";\n" +
                    "\tignoreNodeScale = \"0\";\n" +
                    "\tadjustCenter = \"0\";\n" +
                    "\tadjustFloor = \"0\";\n" +
                    "\tforceUpdateMaterials = \"0\";\n" +*/
                "};\n"+
                "function modeldts::onLoad(%this)\n"+
                "{\n"
            );
            int i = 0;
            foreach (Utility.Sequence seq in SSE.GetSelectedSequences())
            {
                if(seq.id != null)
                    SW.WriteLine(String.Format("\t%this.addSequence(\"{0}\", \"Seq{1}\", \"{2}\", \"{3}\");", seq.File.Replace('\\', '/') + " " + seq.id, i, seq.start, seq.end));
                else
                    SW.WriteLine(String.Format("\t%this.addSequence(\"{0} \", \"Seq{1}\", \"{2}\", \"{3}\");", seq.File.Replace('\\', '/'), i, seq.start, seq.end));
                i++;
            }
            SW.Write(
                "}\n"+
                "datablock StaticShapeData(model)\n"+
                "{\n"+
                    "\tcategory = \"none\";\n" +
                    "\tshapeFile = \"" + SSE.SelectedModel.getPath().Replace('\\', '/') + "\";\n" +
                "};\n"
                );
            if (File.Exists(SSE.SelectedModel.materialsCS))
                SW.WriteLine("exec(\"" + SSE.SelectedModel.materialsCS + "\");");

            SW.Close();
        }
    }
}
