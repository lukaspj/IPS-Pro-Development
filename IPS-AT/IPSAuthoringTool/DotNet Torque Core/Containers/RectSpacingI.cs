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
    /// Used to hold the CSharp equiv of the RectSpacingI torque class.
    /// </summary>
sealed    public class RectSpacingI
    {
        public RectSpacingI(int l, int t, int b, int r)
        {
            left = l;
            top = t;
            bottom = b;
            right = r;
        }

        public RectSpacingI(string val)
        {
            string[] v = val.Split(' ');
            if (v.GetUpperBound(0) < 3) return;
            left = v[0].AsInt();
            top = v[1].AsInt();
            bottom = v[2].AsInt();
            right = v[3].AsInt();
        }

        public int left { get; set; }
        public int top { get; set; }
        public int bottom { get; set; }
        public int right { get; set; }

        public string AsString()
        {
            return string.Format("{0} {1} {2} {3} ", left, top, bottom, right);
        }

        public override string ToString()
        {
            return string.Format("{0} {1} {2} {3} ", left, top, bottom, right);
        }
    }
}