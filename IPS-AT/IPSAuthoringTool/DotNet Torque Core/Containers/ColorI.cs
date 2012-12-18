/*
 * DotNetTorque 
 
    Copyright (C) 2012 Winterleaf Entertainment LLC.

    Please visit http://www.winterleafentertainment.com for more information
    about the project and latest updates.
 */

#region

using WinterLeaf.Classes;

#endregion

namespace WinterLeaf.Containers
{
    /// <summary>
    /// Used to hold the CSharp Equiv of the ColorI torque class.
    /// </summary>
 sealed   public class ColorI
    {
        public ColorI(byte r, byte g, byte b, byte a)
        {
            Red = r;
            Green = g;
            Blue = b;
            a = Alpha;
        }

        public ColorI(string val)
        {
            string[] v = val.Split(' ');
            if (v.GetUpperBound(0) < 3) return;
            Red = v[0].AsByte();
            Green = v[1].AsByte();
            Blue = v[2].AsByte();
            Alpha = v[3].AsByte();
        }

        public byte Red { get; set; }
        public byte Green { get; set; }
        public byte Blue { get; set; }
        public byte Alpha { get; set; }

        public string AsString()
        {
            return string.Format("{0} {1} {2} {3} ", Red, Green, Blue, Alpha);
        }

        public override string ToString()
        {
            return string.Format("{0} {1} {2} {3} ", Red, Green, Blue, Alpha);
        }
    }
}