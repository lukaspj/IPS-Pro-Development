using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace IPSAuthoringTool.Utility
{
    public class Emitter
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

        public class value
        {
            public string valueName;
            public float deltaValue;
            public string startEasing;
            public List<PointOnValue> points;
            public bool Ease;
            public float setTime;
        }

        public struct PointF
        {
            public float X;
            public float Y;
        }

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
            StockEmitter = 0,
            GraphEmitter,
            MeshEmitter,
            RadiusMeshEmitter,
            GroundEmitter,
            MaskEmitter,
            GroundMaskEmitter,
            PathEmitter,
            Error
        }

        public static EmitterType stringToEnum(string s)
        {
            switch (s)
            {
                case "StockEmitter":
                    return EmitterType.StockEmitter;
                case "GraphEmitter":
                    return EmitterType.GraphEmitter;
                case "MeshEmitter":
                    return EmitterType.MeshEmitter;
                case "RadiusMeshEmitter":
                    return EmitterType.RadiusMeshEmitter;
                case "GroundEmitter":
                    return EmitterType.GroundEmitter;
                case "MaskEmitter":
                    return EmitterType.MaskEmitter;
                case "GroundMaskEmitter":
                    return EmitterType.GroundMaskEmitter;
                case "PathEmitter":
                    return EmitterType.PathEmitter;
                default:
                    return EmitterType.Error;
            }
        }
        public static int EnumToInt(EmitterType s)
        {
            switch (s)
            {
                case EmitterType.StockEmitter:
                    return 0;
                case EmitterType.GraphEmitter:
                    return 1;
                case EmitterType.MeshEmitter:
                    return 2;
                case EmitterType.RadiusMeshEmitter:
                    return 3;
                case EmitterType.GroundEmitter:
                    return 4;
                case EmitterType.MaskEmitter:
                    return 5;
                case EmitterType.GroundMaskEmitter:
                    return 6;
                case EmitterType.PathEmitter:
                    return 7;
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
                case EmitterType.StockEmitter:
                    break;
                case EmitterType.GraphEmitter:
                    retList.AddRange(new string[] { "UpperBoundary", "LowerBoundary", "TimeScale"});
                    break;
                case EmitterType.MeshEmitter:
                    break;
                case EmitterType.RadiusMeshEmitter:
                    retList.AddRange(new string[] { "Radius" });
                    break;
                case EmitterType.GroundEmitter:
                    retList.AddRange(new string[] { "Radius" });
                    break;
                case EmitterType.MaskEmitter:
                    retList.AddRange(new string[] { "Scale" });
                    break;
                case EmitterType.GroundMaskEmitter:
                    retList.AddRange(new string[] { "Radius" });
                    break;
                case EmitterType.PathEmitter:
                    retList.AddRange(new string[] { "Travelspeed" });
                    break;
            }
            return retList;
        }

        public override string ToString()
        {
            switch (Type)
            {
                case EmitterType.StockEmitter:
                    return "StockEmitter";
                case EmitterType.GraphEmitter:
                    return "GraphEmitter";
                case EmitterType.MeshEmitter:
                    return "MeshEmitter";
                case EmitterType.RadiusMeshEmitter:
                    return "RadiusMeshEmitter";
                case EmitterType.GroundEmitter:
                    return "GroundEmitter";
                case EmitterType.MaskEmitter:
                    return "MaskEmitter";
                case EmitterType.GroundMaskEmitter:
                    return "GroundMaskEmitter";
                case EmitterType.PathEmitter:
                    return "PathEmitter";
                default:
                    return "Unidentified Emitter";
            }
        }
    }
}
