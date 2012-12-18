/*
 * DotNetTorque 
 
    Copyright (C) 2012 Winterleaf Entertainment LLC.

    Please visit http://www.winterleafentertainment.com for more information
    about the project and latest updates.
 */

#region

using System.Collections.Generic;
using System.Text;

#endregion

namespace WinterLeaf.Classes
{
    /// <summary>
    /// A helper class to streamline creation of Singletons.
    /// </summary>
  sealed  public class TorqueSingleton
    {
        private Dictionary<string, string> _mParams = new Dictionary<string, string>();

        public TorqueSingleton(string className, string datablockName)
        {
            LClassName = className;
            LDatablockName = datablockName;
        }

        /// <summary>
        /// The name of the instance of the object
        /// </summary>
        public string LDatablockName { get; set; }

        /// <summary>
        /// The name of the TorqueScript class.
        /// </summary>
        public string LClassName { get; set; }

        /// <summary>
        /// The properties in the script block/datablock
        /// </summary>
        public Dictionary<string, string> Props
        {
            get { return _mParams; }
            set { _mParams = value; }
        }

        /// <summary>
        /// Addds the property as a string, i.e. key = "value" versus key=value
        /// </summary>
        /// <param name="key"></param>
        /// <param name="str"></param>
        public void PropsAddString(string key, string str)
        {
            _mParams.Add(key, '"' + str + '"');
        }

        /// <summary>
        /// This will return the TorqueScript code that will be executed when Create is called.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            StringBuilder result = new StringBuilder();
            result.Append(" singleton ");
            result.Append(LClassName);
            result.Append("(");
            result.Append(LDatablockName);
            result.Append(")\r\n{");
            foreach (KeyValuePair<string, string> ele in _mParams)
            {
                result.Append(ele.Key);
                result.Append(" = ");
                result.Append(ele.Value.Trim() != "" ? ele.Value : @"""""");
                result.Append(";\r\n");
            }
            result.Append("};");
            return (result.ToString());
        }

        /// <summary>
        /// Passes the ToString() TorqueScript to the Eval statement capturing the id of the object created
        /// </summary>
        /// <param name="m_ts">A reference to the dnTorque Class(you can just type in m_ts)</param>
        /// <returns></returns>
        public void Create(dnTorque m_ts)
        {
            m_ts.Evaluate(ToString(), true);
        }
    }
}