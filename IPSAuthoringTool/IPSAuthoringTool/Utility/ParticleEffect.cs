using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Windows;

namespace IPSAuthoringTool.Utility
{
    public class ParticleEffect
    {
        public List<Emitter> Emitters = new List<Emitter>();
        public string path;
        public string name = "NoName";

        #region Compiler

        public void WriteToFile(string destination)
        {
            path = destination;
            name = Path.GetFileNameWithoutExtension(destination);
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "  ";
            settings.NewLineChars = Environment.NewLine;
            settings.NewLineHandling = NewLineHandling.Replace;
            XmlWriter writer = XmlWriter.Create(destination, settings);
            writer.WriteStartDocument();
            writer.WriteStartElement("ParticleEffect");
            writer.WriteStartElement("Emitters");
            foreach (Emitter emi in Emitters)
            {
                writer.WriteStartElement("Emitter");
                writer.WriteAttributeString("Type", emi.Type.ToString());
                foreach (Emitter.value val in emi.Values)
                {
                    writer.WriteStartElement("Value");
                    writer.WriteAttributeString("Name", val.valueName);
                    writer.WriteAttributeString("DeltaValue", val.deltaValue.ToString());
                    foreach (Emitter.PointOnValue p in val.points)
                    {
                        writer.WriteStartElement("Point");
                        writer.WriteAttributeString("X", p.point.X.ToString());
                        writer.WriteAttributeString("Y", p.point.Y.ToString());
                        writer.WriteAttributeString("Easing", p.Easing);
                        writer.WriteAttributeString("EaseIn", p.EaseIn.ToString());
                        writer.WriteAttributeString("EaseOut", p.EaseOut.ToString());
                        writer.WriteEndElement();
                    }
                    writer.WriteEndElement();
                }
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
            writer.WriteEndElement();
            writer.WriteEndDocument();
            writer.Close();
        }

        public void ReadFromFile(string file)
        {
            if (File.Exists(file))
            {
                path = file;
                XmlReader reader = XmlReader.Create(file);
                if (!reader.ReadToFollowing("ParticleEffect"))
                    return;
                if (!reader.ReadToFollowing("Emitters"))
                    return;
                while (reader.ReadToFollowing("Emitter"))
                    Emitters.Add(readEmitter(reader));
                reader.Close();
                name = Path.GetFileNameWithoutExtension(file);
            }
        }

        private Emitter readEmitter(XmlReader reader)
        {
            Emitter returnEmit = new Emitter();
            reader.MoveToFirstAttribute();
            returnEmit.Type = Emitter.stringToEnum(reader.Value);
            reader.MoveToElement();
            string innerXML = reader.ReadInnerXml();
            XmlReader subReader = XmlReader.Create(new StringReader("<root>"+innerXML+"</root>"));
            while (subReader.ReadToFollowing("Value"))
            {
                subReader.MoveToFirstAttribute();
                Emitter.value val = new Emitter.value();
                val.points = new List<Emitter.PointOnValue>();
                val.valueName = subReader.Value;
                subReader.MoveToNextAttribute();
                val.deltaValue = float.Parse(subReader.Value);
                subReader.MoveToElement();
                string innerXML2 = subReader.ReadInnerXml();
                XmlReader subReader2 = XmlReader.Create(new StringReader("<root>"+innerXML2+"</root>"));
                while (subReader2.ReadToFollowing("Point"))
                {
                    Emitter.PointOnValue pt = new Emitter.PointOnValue();
                    subReader2.MoveToFirstAttribute();
                    pt.point.X = int.Parse(subReader2.Value);
                    subReader2.MoveToNextAttribute();
                    pt.point.Y = int.Parse(subReader2.Value);
                    subReader2.MoveToNextAttribute();
                    pt.Easing = subReader2.Value;
                    subReader2.MoveToNextAttribute();
                    pt.EaseIn = bool.Parse(subReader2.Value);
                    subReader2.MoveToNextAttribute();
                    pt.EaseOut = bool.Parse(subReader2.Value);
                    val.points.Add(pt);
                }
                subReader2.Close();
                returnEmit.Values.Add(val);
            }
            subReader.Close();
            return returnEmit;
        }

        #endregion

        #region Latest

        public static List<ParticleEffect> loadLatestFile()
        {
            List<ParticleEffect> effects = new List<ParticleEffect>();
            if (File.Exists("LatestEmitters.dat"))
            {
                StreamReader SR = new StreamReader("LatestEmitters.dat");
                string line;
                while ((line = SR.ReadLine()) != null)
                {
                    ParticleEffect eff = new ParticleEffect();
                    eff.ReadFromFile(line);
                    effects.Add(eff);
                }
                SR.Close();
            }
            return effects;
        }

        public static void writeLatestFile(List<ParticleEffect> theEffects)
        {
            StreamWriter SW = new StreamWriter("LatestEmitters.dat");
            foreach (ParticleEffect eff in theEffects)
            {
                SW.WriteLine(eff.path);
            }
            SW.Close();
        }

        #endregion
    }
}
