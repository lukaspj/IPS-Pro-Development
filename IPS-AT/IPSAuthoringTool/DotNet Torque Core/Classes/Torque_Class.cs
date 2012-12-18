/*
 * DotNetTorque 
 
    Copyright (C) 2012 Winterleaf Entertainment LLC.

    Please visit http://www.winterleafentertainment.com for more information
    about the project and latest updates.
 */

#region

using System;
using System.Collections.Generic;
using System.Text;

#endregion

namespace WinterLeaf.Classes
{
    /// <summary>
    /// This is a helper class to attempt to clean up the code
    /// when creating a object inside of torque.
    /// </summary>
   sealed public class Torque_Class_Helper
    {
        /// <summary>
        /// Object Properties.
        /// </summary>
        private Dictionary<string, string> _mParams = new Dictionary<string, string>();

        /// <summary>
        /// Constructor Creates an object to template a torquescript class with.
        /// </summary>
        /// <param name="className"></param>
        public Torque_Class_Helper(string className)
        {
            LClassName = className;
            LInstanceName = "";
        }

        /// <summary>
        ///   Creates an object to template a torquescript class with.
        /// </summary>
        /// <param name="className"> The name of the object type </param>
        /// <param name="instanceName"> The name of this instance of the object </param>
        public Torque_Class_Helper(string className, string instanceName)
        {
            LClassName = className;
            LInstanceName = instanceName;
        }

        /// <summary>
        ///   The name of the instance of the object
        /// </summary>
        public string LInstanceName { get; set; }

        /// <summary>
        ///   The name of the TorqueScript class.
        /// </summary>
        public string LClassName { get; set; }

        /// <summary>
        ///   The properties in the script block/datablock
        /// </summary>
        public Dictionary<string, string> Props
        {
            get { return _mParams; }
            set { _mParams = value; }
        }

        public void PropsAddString(string key, string str)
        {
            _mParams.Add(key, '"' + str + '"');
        }

        /// <summary>
        ///   This will return the TorqueScript code that will be executed when Create is called.
        /// </summary>
        /// <returns> </returns>
        public override string ToString()
        {
            StringBuilder result = new StringBuilder();
            result.Append(" new ");
            result.Append(LClassName);
            result.Append("(");
            result.Append(LInstanceName);
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
        ///   Passes the ToString() TorqueScript to the Eval statement capturing the id of the object created
        /// </summary>
        /// <param name="m_ts"> A reference to the dnTorque Class(you can just type in m_ts) </param>
        /// <returns> </returns>
        public UInt32 Create(dnTorque m_ts)
        {
            UInt32 r;
            string varnam = "$IReallyUniqueForthisWorkAround123654";
            m_ts.SetVar(varnam, "0");

            m_ts.Evaluate(varnam + " = " + ToString(), true);
            string id = m_ts.GetVar(varnam);
            return !UInt32.TryParse(id, out r) ? 0 : r;
        }

        /// <summary>
        /// Takes the passed TorqueScript and evaluates it in the dll.
        /// </summary>
        /// <param name="m_ts"></param>
        public void CreateDB(dnTorque m_ts)
        {
            m_ts.Evaluate(ToString(), true);
        }
    }
}