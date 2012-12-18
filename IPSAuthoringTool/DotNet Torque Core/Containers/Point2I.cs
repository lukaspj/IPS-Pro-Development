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
    /// Used to hold CSharp equiv of the Point2I Torque Class.
    /// </summary>
sealed    public class Point2I
    {
        public Point2I(int mx, int my)
        {
            x = mx;
            y = my;
        }

        public Point2I(string val)
        {
            string[] v = val.Split(' ');
            if (v.GetUpperBound(0) < 1) return;
            x = v[0].AsInt();
            y = v[1].AsInt();
        }

        public int x { get; set; }
        public int y { get; set; }

        public string AsString()
        {
            return string.Format("{0} {1} ", x, y);
        }

        public override string ToString()
        {
            return string.Format("{0} {1} ", x, y);
        }
    }
}