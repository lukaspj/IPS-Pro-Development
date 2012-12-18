﻿/*
 * DotNetTorque 
 
    Copyright (C) 2012 Winterleaf Entertainment LLC.

    Please visit http://www.winterleafentertainment.com for more information
    about the project and latest updates.
 */

#region

using System;
using System.Globalization;
using System.Reflection;
using System.Text;

#endregion

namespace WinterLeaf.Classes
{
 sealed   public class Torque_Decorations
    {
        #region Nested type: TorqueCallBack

        /// <summary>
        ///   This class is the actual decoration used by the user to flag a function for export into the Torque 3D dll.
        /// </summary>
        [AttributeUsage(AttributeTargets.Method)]
        public class TorqueCallBack : Attribute
        {
            private int _mMaxArgs;

            /// <summary>
            /// Registers the function as a callback function for T3D
            /// </summary>
            /// <param name="torquePackageName">The package name to be assigned inside of Torque</param>
            /// <param name="torqueClassOrNamespace">A classname or namespace inside of Torque</param>
            /// <param name="torqueFunction">The function name to be assigned inside of Torque</param>
            /// <param name="torqueUsage">A string for error messages for usage</param>
            /// <param name="maxargs">Max number of arguments that can be passed</param>
            /// <param name="functionweight">The weight used in loading functions into the Torque Compiler</param>
            /// <param name="executeaftercompile">Flag to indicate that you want the function executed after it is loaded into Torque.</param>
            public TorqueCallBack(string torquePackageName, string torqueClassOrNamespace, string torqueFunction,
                                  string torqueUsage,
                                  int maxargs, int functionweight, bool executeaftercompile = false)
            {
                if (_mMaxArgs > 10)
                    Console.WriteLine("Max args exceeded");

                _mMaxArgs = maxargs;
                TorqueClassname = torqueClassOrNamespace;
                TorqueFunction = torqueFunction;
                TorqueUsage = torqueUsage;
                FunctionWeight = functionweight;
                TorquePackageName = torquePackageName;
                ExecuteAfterCompile = executeaftercompile;
            }

            /// <summary>
            ///   The weight of the function, used to determine load order into the Torque DLL.
            /// </summary>
            public int FunctionWeight { get; set; }

            /// <summary>
            ///   Flag to tell DotNetTorque to execute the function after injecting it.
            /// </summary>
            public bool ExecuteAfterCompile { get; set; }

            /// <summary>
            ///   This function is a member of a Package.
            /// </summary>
            public string TorquePackageName { get; set; }

            /// <summary>
            ///   This function is a member of a class.
            /// </summary>
            public string TorqueClassname { get; set; }

            /// <summary>
            ///   The name of the function.
            /// </summary>
            public string TorqueFunction { get; set; }

            /// <summary>
            ///   The usage of the function.
            /// </summary>
            public string TorqueUsage { get; set; }

            /// <summary>
            ///   The max number of args which can be passed to the function. Hard cap of 20, which is default in Torque.
            /// </summary>
            public int TorqueMaxArgs
            {
                get { return _mMaxArgs; }
                set
                {
                    if (value > 20) throw new Exception("Max # of Args is 20");
                    _mMaxArgs = value;
                }
            }
        }

        #endregion

        #region Nested type: TorqueCallBackInfo

        /// <summary>
        ///   A helper class.
        /// </summary>
        internal class TorqueCallBackInfo
        {
            private int _mMaxArgs;
            private string _mTorqueClassname;
            private string _mTorqueFunction;
            private int _mTorqueFunctionID;

            public TorqueCallBackInfo(TorqueCallBack tcb, MethodInfo mi, ParameterInfo[] pi)
            {
                _mMaxArgs = tcb.TorqueMaxArgs;
                _mTorqueClassname = tcb.TorqueClassname;
                _mTorqueFunction = tcb.TorqueFunction;
                TorqueUsage = tcb.TorqueUsage;
                FunctionInvoke = mi;
                Functionweight = tcb.FunctionWeight;
                TorquePackageName = tcb.TorquePackageName;
                ExecuteAfterCompile = tcb.ExecuteAfterCompile;
                FunctionParams = pi;
            }

            public bool ExecuteAfterCompile { get; set; }

            public ScriptType TorqueScriptType { get; set; }

            public int Functionweight { get; set; }

            public MethodInfo FunctionInvoke { get; set; }
            public ParameterInfo[] FunctionParams { get; set; }

            public string TorquePackageName { get; set; }


            public string TorqueClassname
            {
                get { return _mTorqueClassname; }
                set { _mTorqueClassname = value.Trim().ToLower(); }
            }

            public string TorqueFunction
            {
                get { return _mTorqueFunction; }
                set { _mTorqueFunction = value.Trim().ToLower(); }
            }

            public string TorqueUsage { get; set; }


            public int TorqueMaxArgs
            {
                get { return _mMaxArgs; }
                set
                {
                    if (value > 20) throw new Exception("Max # of Args is 20");
                    _mMaxArgs = value;
                }
            }

            public int TorqueFunctionID
            {
                get { return _mTorqueFunctionID; }
                set { _mTorqueFunctionID = value; }
            }

            public override string ToString()
            {
                return TorquePackageName + "|" + _mTorqueClassname + "|" + _mTorqueFunction + "|";
            }


            public string GetScript(ScriptType st, bool debug)
            {
                StringBuilder body = new StringBuilder();

                if (TorqueClassname.Trim() != "")
                {
                    body.AppendFormat("function {0}::{1}(", TorqueClassname, TorqueFunction);
                }
                else
                {
                    body.AppendFormat("function {0}(", TorqueFunction);
                }
                bool firstloop = true;
                for (int i = 0; i < TorqueMaxArgs; i++)
                {
                    if (firstloop)
                    {
                        body.AppendFormat("%a{0}", i.ToString(CultureInfo.InvariantCulture));
                        firstloop = false;
                    }
                    else
                        body.AppendFormat(", %a{0}", i.ToString(CultureInfo.InvariantCulture));
                }
                body.Append(")\r\n");
                body.Append("{\r\n");
                if (debug)
                    body.AppendFormat(@"error(""---------------------------------->function called: {0}::{1}{2});",
                                      TorqueClassname, TorqueFunction, '"');
                body.Append("\r\n");

                body.Append("return dnEval_Direct_New(" + TorqueFunctionID);
                for (int i = 0; i < TorqueMaxArgs; i++)
                {
                    body.Append(",%a" + i.AsString());
                }


                body.Append(");\r\n");


                //body.Append("return %result;\r\n");
                body.Append("}");

                return body.ToString();
            }
        }

        #endregion
    }
}