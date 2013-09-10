using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace IPSAuthoringTool.Utility
{
    public class Sequence
    {
        public int start = int.MinValue;
        public int end = int.MaxValue;
        private string file;
        private string name;
        public string id = null;

        public string File
        {
            get { return file; }
            set { file = value; }
        }

        public string Name
        {
            get
            {
                if (name == "" || name == null)
                    return Path.GetFileNameWithoutExtension(file);
                return name;
            }
            set { name = value; }
        }

        public override string ToString()
        {
            return name+"|"+file+"|"+start.ToString()+"|"+end.ToString()+"|"+id;
        }

        public bool HasName()
        {
            if (name == "" || name == null)
                return false;
            return true;
        }

        public static Sequence FromString(string s)
        {
            Sequence ret = new Sequence();
            string[] split = s.Trim().Split('|');
            if (split.Length != 5)
                throw new NotImplementedException();
            ret.name = split[0];
            ret.file = split[1];
            if (!int.TryParse(split[2], out ret.start))
                throw new ArithmeticException();
            if (!int.TryParse(split[3], out ret.end))
                throw new ArithmeticException();
            ret.id = split[4] == "" ? null : split[4];
            return ret;
        }
    }
}
