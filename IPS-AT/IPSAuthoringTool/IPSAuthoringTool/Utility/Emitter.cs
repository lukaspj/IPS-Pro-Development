using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace IPSAuthoringTool.Utility
{
    [Serializable] 
    public class Emitter : INotifyPropertyChanged
    {
        public List<value> Values = new List<value>();
        public EmitterType Type = 0;
        public float x = 0;
        public float y = 0;
        public float z = 0;
        public string datablock = "";
        public string emitter = "";
        public float Start = 0;
        public float End = 0;
        private string tag = null;

        public string Tag
        {
            get { return tag; }
            set { tag = value; NotifyPropertyChanged("Tag"); }
        }

        #region INotifyPropertyChanged interface
        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(String propertyName = "")
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion

        [Serializable] 
        public class value
        {
            public string valueName;
            public float deltaValue;
            public string startEasing;
            public List<PointOnValue> points;
            public bool Ease;
            public float setTime;
            public value()
            {
                points = new List<PointOnValue>();
            }
            public override string ToString()
            {
                return valueName;
            }
        }

        [Serializable] 
        public struct PointF
        {
            public float X;
            public float Y;
        }

        [Serializable] 
        public class PointOnValue
        {
            public PointF point;
            public string Easing;
            public bool EaseIn;
            public bool EaseOut;

            public PointOnValue()
            {
                point = new PointF();
                point.X = 0;
                point.Y = 0;
                Easing = "Linear";
                EaseIn = true;
                EaseOut = true;
            }
        }

        public enum EmitterType
        {
            SphereEmitter = 0,
            GraphEmitter,
            GroundEmitter,
            MaskEmitter,
            Error
        }

        public string getDatablockType()
        {
            switch (Type)
            {
                case EmitterType.SphereEmitter:
                    return "SphereEmitterData";
                case EmitterType.GraphEmitter:
                    return "GraphEmitterData";
                case EmitterType.GroundEmitter:
                    return "GroundEmitterData";
                case EmitterType.MaskEmitter:
                    return "MaskEmitterData";
            }
            return "";
        }

        public static string getDatablockType(EmitterType Type)
        {
            switch (Type)
            {
                case EmitterType.SphereEmitter:
                    return "SphereEmitterData";
                case EmitterType.GraphEmitter:
                    return "GraphEmitterData";
                case EmitterType.GroundEmitter:
                    return "GroundEmitterData";
                case EmitterType.MaskEmitter:
                    return "MaskEmitterData";
            }
            return "";
        }

        public string getNodeDatablockType()
        {
            switch (Type)
            {
                case EmitterType.SphereEmitter:
                    return "SphereEmitterNodeData";
                case EmitterType.GraphEmitter:
                    return "GraphEmitterNodeData";
                case EmitterType.GroundEmitter:
                    return "GroundEmitterNodeData";
                case EmitterType.MaskEmitter:
                    return "MaskEmitterNodeData";
            }
            return "";
        }

        public static string getNodeDatablockType(EmitterType Type)
        {
            switch (Type)
            {
                case EmitterType.SphereEmitter:
                    return "SphereEmitterNodeData";
                case EmitterType.GraphEmitter:
                    return "GraphEmitterNodeData";
                case EmitterType.GroundEmitter:
                    return "GroundEmitterNodeData";
                case EmitterType.MaskEmitter:
                    return "MaskEmitterNodeData";
            }
            return "";
        }

        public static EmitterType stringToEnum(string s)
        {
            switch (s)
            {
                case "SphereEmitter":
                    return EmitterType.SphereEmitter;
                case "GraphEmitter":
                    return EmitterType.GraphEmitter;
                case "GroundEmitter":
                    return EmitterType.GroundEmitter;
                case "MaskEmitter":
                    return EmitterType.MaskEmitter;
                default:
                    return EmitterType.Error;
            }
        }
        public static int EnumToInt(EmitterType s)
        {
            switch (s)
            {
                case EmitterType.SphereEmitter:
                    return 0;
                case EmitterType.GraphEmitter:
                    return 1;
                case EmitterType.GroundEmitter:
                    return 2;
                case EmitterType.MaskEmitter:
                    return 3;
                default:
                    return -1;
            }
        }

        public class PointXSorter : IComparer<PointOnValue>
        {
            public int Compare(PointOnValue obj1, PointOnValue obj2)
            {
                return obj1.point.X.CompareTo(obj2.point.X);
            }
        }

        public static List<string> getFields(EmitterType _type)
        {
            List<string> retList = new List<string>();
            retList.AddRange(new string[] {"xPosition","yPosition", "zPosition", "EjectionPeriod", "EjectionOffset"});
            switch (_type)
            {
                case EmitterType.SphereEmitter:
                    break;
                case EmitterType.GraphEmitter:
                    retList.AddRange(new string[] { "UpperBoundary", "LowerBoundary", "TimeScale"});
                    break;
                case EmitterType.GroundEmitter:
                    retList.AddRange(new string[] { "Radius" });
                    break;
                case EmitterType.MaskEmitter:
                    retList.AddRange(new string[] { "Scale" });
                    break;
            }
            return retList;
        }

        public override string ToString()
        {
            return Tag == null ? Type.ToString() : Tag;
        }

        public string asString()
        {
            switch (Type)
            {
                case EmitterType.SphereEmitter:
                    return "SphereEmitter";
                case EmitterType.GraphEmitter:
                    return "GraphEmitter";
                case EmitterType.GroundEmitter:
                    return "GroundEmitter";
                case EmitterType.MaskEmitter:
                    return "MaskEmitter";
                default:
                    return "Unidentified Emitter";
            }
        }

        public static List<object> getEmitterLabels()
        {
            return new List<object>() { "SphereEmitter", "GraphEmitter", "GroundEmitter", "MaskEmitter" };
        }

        /*
    public Emitter Clone()
    {
        List<value> clonedList = new List<value>();
        foreach (value v in Values)
            clonedList.Add(v.Clone());
        return new Emitter() {
            datablock = (string)datablock.Clone(),
            emitter = (string)emitter.Clone(),
            End = End,
            Start = Start,
            tag = (string)tag.Clone(),
            Type = Type,
            Values = clonedList,
            x = x, y = y, z = z
        };
    }*/
    }
}
