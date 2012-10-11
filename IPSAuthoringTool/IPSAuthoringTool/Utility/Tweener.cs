using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IPSAuthoringTool.Utility
{
    class Tweener
    {
        // c = Delta
        // t = Time
        // d = Duration
        // b = Start

        /**
         * No easing, no acceleration 
         */
        public static double LinearTween(double Time, double Start, double Delta, double Duration)
        {
            return Delta * Time / Duration + Start;
        }

        /**
         * Accelerating from zero velocity 
         */
        public static double EaseInQuad(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            return Delta * Time * Time + Start;
        }

        /**
         * Decelerating to zero velocity 
         */
        public static double EaseOutQuad(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            return -Duration * Time * (Time - 2) + Start;
        }

        /**
         * Acceleration until halfway, then deceleration
         */
        public static double EaseInOutQuad(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration / 2;
            if (Time < 1)
                return Delta / 2 * Time * Time + Start;
            Time--;
            return (-1*Delta) / 2 * (Time * (Time - 2) - 1) + Start;
        }

        /**
         * Accelerating from zero velocity 
         */
        public static double EaseInCubic(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            return Delta * Time * Time * Time + Start;
        }

        /**
         * Decelerating to zero velocity 
         */
        public static double EaseOutCubic(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            Time--;
            return Delta * (Time * Time * Time + 1) + Start;
        }

        /**
         * Acceleration until halfway, then deceleration 
         */
        public static double EaseInOutCubic(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration / 2;
            if (Time < 1)
                return Delta / 2 * Time * Time * Time + Start;
            Time -= 2;
            return Delta / 2 * (Time * Time * Time + 2) + Start;
        }

        /**
         * Accelerating from zero velocity 
         */
        public static double EaseInQuart(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            return Delta * Time * Time * Time * Time + Start;
        }

        /**
         * Decelerating to zero velocity 
         */
        public static double EaseOutQuart(double Time, double Start, double Delta, double Duration)
        {
            Time /= Delta;
            Time--;
            return -Delta * (Time * Time * Time * Time - 1) + Start;
        }

        /**
         * Acceleration until halfway, then deceleration 
         */
        public static double EaseInOutQuart(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration / 2;
            if (Time < 1) return Delta / 2 * Time * Time * Time * Time + Start;
            Time -= 2;
            return -Delta / 2 * (Time * Time * Time * Time - 2) + Start;
        }

        /**
         * Accelerating from zero velocity 
         */
        public static double EaseInQuint(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            return Delta * Time * Time * Time * Time * Time + Start;
        }

        /**
         * Decelerating to zero velocity 
         */
        public static double EaseOutQuint(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            Time--;
            return Delta * (Time * Time * Time * Time * Time + 1) + Start;
        }

        /**
         * Acceleration until halfway, then deceleration 
         */
        public static double EaseInOutQuint(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration / 2;
            if (Time < 1) return Delta / 2 * Time * Time * Time * Time * Time + Start;
            Time -= 2;
            return Delta / 2 * (Time * Time * Time * Time * Time + 2) + Start;
        }

        /**
         * Accelerating from zero velocity 
         */
        public static double EaseInSine(double Time, double Start, double Delta, double Duration)
        {
            return -Delta * (double)Math.Cos(Time / Duration * (Math.PI / 2)) + Delta + Start;
        }

        /**
         * Decelerating to zero velocity 
         */
        public static double EaseOutSine(double Time, double Start, double Delta, double Duration)
        {
            return Delta * (double)Math.Sin(Time / Duration * (Math.PI / 2)) + Start;
        }

        /**
         * Acceleration until halfway, then deceleration 
         */
        public static double EaseInOutSine(double Time, double Start, double Delta, double Duration)
        {
            return -Delta / 2 * ((double)Math.Cos(Math.PI * Time / Duration) - 1) + Start;
        }

        /**
        * Accelerating from zero velocity 
        */
        public static double EaseInExpo(double Time, double Start, double Delta, double Duration)
        {
            return Delta * (double)Math.Pow(2, 10 * (Time / Duration - 1)) + Start;
        }

        /**
         * Decelerating to zero velocity 
         */
        public static double EaseOutExpo(double Time, double Start, double Delta, double Duration)
        {
            return Delta * ((double)-Math.Pow(2, -10 * Time / Duration)) + Start;
        }

        /**
         * Acceleration until halfway, then deceleration 
         */
        public static double EaseInOutExpo(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration / 2;
            if (Time < 1) return Delta / 2 * (double)Math.Pow(2, 10 * (Time - 1)) + Start;
            Time--;
            return Delta / 2 * ((double)-Math.Pow(2, -10 * Time) + 2) + Start;
        }

        /**
        * Accelerating from zero velocity 
        */
        public static double EaseInCirc(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            return -Delta * ((double)Math.Sqrt(1 - Time * Time) - 1) + Start;
        }

        /**
         * Decelerating to zero velocity 
         */
        public static double EaseOutCirc(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration;
            Time--;
            return Delta * (double)Math.Sqrt(1 - Time * Time) + Start;
        }

        /**
         * Acceleration until halfway, then deceleration 
         */
        public static double EaseInOutCirc(double Time, double Start, double Delta, double Duration)
        {
            Time /= Duration / 2;
            if (Time < 1) return -Delta / 2 * ((double)Math.Sqrt(1 - Time * Time) - 1) + Start;
            Time -= 2;
            return Delta / 2 * ((double)Math.Sqrt(1 - Time * Time) + 1) + Start;
        }
        //http://gizma.com/easing/#l

        public static double EaseFromString(double Time, double Start, double Delta, double Duration, string Method, bool In, bool Out)
        {
            switch (Method)
            {
                case "Linear":
                    return LinearTween(Time, Start, Delta, Duration);
                case "Quadratic":
                    if (In && Out)
                        return EaseInOutQuad(Time, Start, Delta, Duration);
                    else if (In)
                        return EaseInQuad(Time, Start, Delta, Duration);
                    else if (Out)
                        return EaseOutQuad(Time, Start, Delta, Duration);
                    else return LinearTween(Time, Start, Delta, Duration);
                case "Cubic":
                    if (In && Out)
                        return EaseInOutCubic(Time, Start, Delta, Duration);
                    else if (In)
                        return EaseInCubic(Time, Start, Delta, Duration);
                    else if (Out)
                        return EaseOutCubic(Time, Start, Delta, Duration);
                    else return LinearTween(Time, Start, Delta, Duration);
                case "Quartic":
                    if(In && Out)
                        return EaseInOutQuart(Time, Start, Delta, Duration);
                    else if(In)
                        return EaseInQuart(Time, Start, Delta, Duration);
                    else if(Out)
                        return EaseOutQuart(Time, Start, Delta, Duration);
                    else return LinearTween(Time, Start, Delta, Duration);
                case "Quintic":
                    if(In && Out)
                        return EaseInOutQuint(Time, Start, Delta, Duration);
                    else if(In)
                        return EaseInQuint(Time, Start, Delta, Duration);
                    else if(Out)
                        return EaseOutQuint(Time, Start, Delta, Duration);
                    else return LinearTween(Time, Start, Delta, Duration);
                case "Sinusoidal":
                    if(In && Out)
                        return EaseInOutSine(Time, Start, Delta, Duration);
                    else if(In)
                        return EaseInSine(Time, Start, Delta, Duration);
                    else if(Out)
                        return EaseOutSine(Time, Start, Delta, Duration);
                    else return LinearTween(Time, Start, Delta, Duration);
                case "Exponential":
                    if(In && Out)
                        return EaseInOutExpo(Time, Start, Delta, Duration);
                    else if(In)
                        return EaseInExpo(Time, Start, Delta, Duration);
                    else if(Out)
                        return EaseOutExpo(Time, Start, Delta, Duration);
                    else return LinearTween(Time, Start, Delta, Duration);
                case "Circular":
                    if(In && Out)
                        return EaseInOutCirc(Time, Start, Delta, Duration);
                    else if(In)
                        return EaseInCirc(Time, Start, Delta, Duration);
                    else if(Out)
                        return EaseOutCirc(Time, Start, Delta, Duration);
                    else return LinearTween(Time, Start, Delta, Duration);
                default:
                    return LinearTween(Time, Start, Delta, Duration);
            }
        }

        public static List<string> GetEasingMethodNames()
        {
            List<string> retList = new List<string>();
            retList.AddRange(new string[] { "Linear", "Quadratic", "Cubic", "Quartic", "Quintic", "Sinusoidal", "Exponential", "Circular" });
            return retList;
        }
    }
}
