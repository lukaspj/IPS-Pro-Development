using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IPSAuthoringTool.Utility
{
    public class Model
    {
        private string Path;
        public string materialsCS = null;

        public Model()
        {
        }

        public string getPath() { return Path; }

        public bool setPath(string path)
        {
            if (!System.IO.File.Exists(path))
                return false;
            Path = path;
            string materialscs = System.IO.Path.GetDirectoryName(path).Replace('\\', '/') + "/materials.cs";
            if (System.IO.File.Exists(materialscs))
            {
                materialsCS = materialscs;
                return true;
            }
            materialsCS = null;
            return false;
        }

        public override string ToString()
        {
            return System.IO.Path.GetFileNameWithoutExtension(Path);
        }

        public string AsString()
        {
            return Path + "|" + materialsCS;
        }

        public static Model FromString(string s)
        {
            Model retMod = new Model();
            string[] split = s.Trim().Split('|');
            if(split.Length != 2)
                throw new NotImplementedException();
            retMod.setPath(split[0]);
            if (System.IO.File.Exists(split[1]))
                retMod.materialsCS = split[1];
            return retMod;
        }
    }
}
