using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Windows;
using System.Globalization;

namespace IPSAuthoringTool.Utility
{
    public class ParticleEffect
    {
        public List<Emitter> Emitters = new List<Emitter>();
        public string path;
        public string name = "NoName";

        #region Compiler

        public void WriteToFile(string destination, Dialogs.AlertDialog alertDialog)
        {
            path = destination;
            name = Path.GetFileNameWithoutExtension(destination);
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "  ";
            settings.NewLineChars = Environment.NewLine;
            settings.NewLineHandling = NewLineHandling.Replace;
            XmlWriter writer = XmlWriter.Create(destination, settings);
            try
            {
                writer.WriteStartDocument();
                writer.WriteStartElement("ParticleEffect");
                writer.WriteStartElement("Emitters");
                foreach (Emitter emi in Emitters)
                {
                    writer.WriteStartElement("Emitter");
                    writer.WriteAttributeString("Type", emi.Type.ToString());
                    writer.WriteAttributeString("x", emi.x.ToString());
                    writer.WriteAttributeString("y", emi.y.ToString());
                    writer.WriteAttributeString("z", emi.z.ToString());
                    writer.WriteAttributeString("datablock", emi.datablock);
                    writer.WriteAttributeString("emitter", emi.emitter);
                    writer.WriteAttributeString("Start", emi.Start.ToString());
                    writer.WriteAttributeString("End", emi.End.ToString());
                    foreach (Emitter.value val in emi.Values)
                    {
                        writer.WriteStartElement("Value");
                        writer.WriteAttributeString("Ease", val.Ease.ToString());
                        writer.WriteAttributeString("Name", val.valueName);
                        writer.WriteAttributeString("DeltaValue", val.deltaValue.ToString());
                        writer.WriteAttributeString("setTime", val.setTime.ToString());
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
                alertDialog.ShowHandlerDialog("Successfully saved: " + name);
            }
            catch
            {
                alertDialog.ShowHandlerDialog("Error saving: " + name);
            }
            writer.Close();
        }

        public void ReadFromFile(string file, Dialogs.AlertDialog alertDialog)
        {
            try
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
                    alertDialog.ShowHandlerDialog("Succesfully loaded effect.");
                }
            }
            catch
            {
                alertDialog.ShowHandlerDialog("Error while reading: " + path);
            }
        }

        public void ReadFromFile(string file)
        {
            try
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
            catch
            {
                MessageBox.Show("Error while reading: " + path);
            }
        }

        private Emitter readEmitter(XmlReader reader)
        {
            Emitter returnEmit = new Emitter();
            reader.MoveToAttribute("Type");
            returnEmit.Type = Emitter.stringToEnum(reader.Value);
            reader.MoveToAttribute("x");
            returnEmit.x = float.Parse(reader.Value, CultureInfo.InvariantCulture);
            reader.MoveToAttribute("y");
            returnEmit.y = float.Parse(reader.Value, CultureInfo.InvariantCulture);
            reader.MoveToAttribute("z");
            returnEmit.z = float.Parse(reader.Value, CultureInfo.InvariantCulture);
            reader.MoveToAttribute("datablock");
            returnEmit.datablock = reader.Value;
            reader.MoveToAttribute("emitter");
            returnEmit.emitter = reader.Value;
            reader.MoveToAttribute("Start");
            returnEmit.Start = float.Parse(reader.Value, CultureInfo.InvariantCulture);
            reader.MoveToAttribute("End");
            returnEmit.End = float.Parse(reader.Value, CultureInfo.InvariantCulture);
            reader.MoveToElement();
            string innerXML = reader.ReadInnerXml();
            XmlReader subReader = XmlReader.Create(new StringReader("<root>"+innerXML+"</root>"));
            while (subReader.ReadToFollowing("Value"))
            {
                Emitter.value val = new Emitter.value();
                val.points = new List<Emitter.PointOnValue>();
                subReader.MoveToAttribute("Name");
                val.valueName = subReader.Value;
                subReader.MoveToAttribute("DeltaValue");
                val.deltaValue = float.Parse(subReader.Value, CultureInfo.InvariantCulture);
                subReader.MoveToAttribute("Ease");
                val.Ease = bool.Parse(subReader.Value);
                subReader.MoveToAttribute("DeltaValue");
                subReader.MoveToElement();
                string innerXML2 = subReader.ReadInnerXml();
                XmlReader subReader2 = XmlReader.Create(new StringReader("<root>"+innerXML2+"</root>"));
                while (subReader2.ReadToFollowing("Point"))
                {
                    Emitter.PointOnValue pt = new Emitter.PointOnValue();
                    subReader2.MoveToAttribute("X");
                    pt.point.X = float.Parse(subReader2.Value, CultureInfo.InvariantCulture);
                    subReader2.MoveToAttribute("Y");
                    pt.point.Y = float.Parse(subReader2.Value, CultureInfo.InvariantCulture);
                    subReader2.MoveToAttribute("Easing");
                    pt.Easing = subReader2.Value;
                    subReader2.MoveToAttribute("EaseIn");
                    pt.EaseIn = bool.Parse(subReader2.Value);
                    subReader2.MoveToAttribute("EaseOut");
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
