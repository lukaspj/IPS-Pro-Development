using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Windows;
using System.Globalization;

namespace IPSAuthoringTool.Utility
{
    [Serializable]
    public class ParticleEffect
    {
        private List<Emitter> emitters = new List<Emitter>();

        public List<Emitter> Emitters
        {
            get { return emitters; }
            set { emitters = value; }
        }
        private string _path;

        public string path
        {
            get { return _path; }
            set { _path = value; }
        }
        private string _name = "NoName";

        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        #region Compiler

        public void WriteToFile(string destination, Dialogs.AlertDialog alertDialog)
        {
            path = destination;
            Name = Path.GetFileNameWithoutExtension(destination);
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
                    writer.WriteAttributeString("x", emi.x.ToString(CultureInfo.InvariantCulture));
                    writer.WriteAttributeString("y", emi.y.ToString(CultureInfo.InvariantCulture));
                    writer.WriteAttributeString("z", emi.z.ToString(CultureInfo.InvariantCulture));
                    writer.WriteAttributeString("datablock", emi.datablock);
                    writer.WriteAttributeString("emitter", emi.emitter);
                    writer.WriteAttributeString("Start", emi.Start.ToString());
                    writer.WriteAttributeString("End", emi.End.ToString());
                    writer.WriteAttributeString("Tag", emi.Tag);
                    foreach (Emitter.value val in emi.Values)
                    {
                        writer.WriteStartElement("Value");
                        writer.WriteAttributeString("Ease", val.Ease.ToString());
                        writer.WriteAttributeString("Name", val.valueName);
                        writer.WriteAttributeString("DeltaValue", val.deltaValue.ToString(CultureInfo.InvariantCulture));
                        writer.WriteAttributeString("setTime", val.setTime.ToString(CultureInfo.InvariantCulture));
                        foreach (Emitter.PointOnValue p in val.points)
                        {
                            writer.WriteStartElement("Point");
                            writer.WriteAttributeString("X", p.point.X.ToString(CultureInfo.InvariantCulture));
                            writer.WriteAttributeString("Y", p.point.Y.ToString(CultureInfo.InvariantCulture));
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
                if (alertDialog != null)
                    alertDialog.ShowHandlerDialog("Successfully saved: " + Name);
            }
            catch
            {
                if (alertDialog != null)
                    alertDialog.ShowHandlerDialog("Error saving: " + Name);
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
                    Name = Path.GetFileNameWithoutExtension(file);
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
                    Name = Path.GetFileNameWithoutExtension(file);
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
            float.TryParse(reader.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out returnEmit.x);
            reader.MoveToAttribute("y");
            float.TryParse(reader.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out returnEmit.y);
            reader.MoveToAttribute("z");
            float.TryParse(reader.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out returnEmit.z);
            reader.MoveToAttribute("datablock");
            returnEmit.datablock = reader.Value;
            reader.MoveToAttribute("emitter");
            returnEmit.emitter = reader.Value;
            reader.MoveToAttribute("Start");
            float.TryParse(reader.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out returnEmit.Start);
            reader.MoveToAttribute("End");
            float.TryParse(reader.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out returnEmit.End);
            if (reader.MoveToAttribute("Tag"))
                returnEmit.Tag = reader.Value != "" ? reader.Value : null;
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
                float.TryParse(subReader.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out val.deltaValue);
                subReader.MoveToAttribute("setTime");
                float.TryParse(subReader.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out val.setTime);
                subReader.MoveToAttribute("Ease");
                val.Ease = bool.Parse(subReader.Value);
                subReader.MoveToAttribute("DeltaValue");
                subReader.MoveToElement();
                string innerXML2 = subReader.ReadInnerXml();
                XmlReader subReader2 = XmlReader.Create(new StringReader("<root>"+innerXML2+"</root>"));
                while (subReader2.ReadToFollowing("Point"))
                {
                    Emitter.PointOnValue pt = new Emitter.PointOnValue();
                    if(subReader2.MoveToAttribute("X"))
                        float.TryParse(subReader2.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out pt.point.X);
                    if(subReader2.MoveToAttribute("Y"))
                        float.TryParse(subReader2.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out pt.point.Y);
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

        public static Collection<ParticleEffect> loadLatestFile()
        {
            Collection<ParticleEffect> effects = new Collection<ParticleEffect>();
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

        public static void writeLatestFile(Collection<ParticleEffect> theEffects)
        {
            StreamWriter SW = new StreamWriter("LatestEmitters.dat");
            foreach (ParticleEffect eff in theEffects)
            {
                SW.WriteLine(eff.path);
            }
            SW.Close();
        }

        #endregion

        public override string ToString()
        {
            return Name;
        }
    }
}
