using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;

namespace SimpleWPFChart
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window 
    {
        
        
        public List<ChartCurve> lines;  //client provides the data which could be lots of data points

        /// <summary>
        /// Max number of x data points supported by this graph
        /// Depend on the amount of time which mean amount of raw data points, calculate those to get the curve with this fixed number of data points to show on graph.
        /// Increase the value here just make the graph compressed --> Not the right approach --> DO NOT USE THIS APPROACH AGAIN
        /// </summary>
        private int numberOfDataPoints = 20;
        
        private List<ChartCurve> processedLines;    //this is the actual data used to plot

        private double minXValue = 0;
        private double maxXValue = 20;   
        private double minYValue = 0;
        private double maxYValue = 100;  

        public Window1()
        {
            InitializeComponent();
            #region Similator Data                      

            ChartCurve cc1 = new ChartCurve(Brushes.Red, GetFakeData(0) , ChartLineType.PolylineType, "Red Channel");
            ChartCurve cc2 = new ChartCurve(Brushes.Green, GetFakeData(20), ChartLineType.PolylineType, "Green Channel");
            ChartCurve cc3 = new ChartCurve(Brushes.Blue, GetFakeData(-20), ChartLineType.PolylineType, "Blue Channel");

            lines = new List<ChartCurve>();
            lines.Add(cc1);
            lines.Add(cc2);
            lines.Add(cc3);

            ObservableCollection<ChartRequestInfo> ri = new ObservableCollection<ChartRequestInfo>();
            ri.Add(new ChartRequestInfo(Brushes.Red, GetDoubleData(0), ChartLineType.PolylineType, "Red Channel"));
            ri.Add(new ChartRequestInfo(Brushes.Green, GetDoubleData(20), ChartLineType.PolylineType, "Green Channel"));
            ri.Add(new ChartRequestInfo(Brushes.Blue, GetDoubleData(-20), ChartLineType.PolylineType, "Blue Channel"));
            ucGraph.RequestData = ri;
            

            ObservableCollection<ChartRequestInfo> ri2 = new ObservableCollection<ChartRequestInfo>();
            ri2.Add(new ChartRequestInfo(Brushes.Red, GetDoubleData(0), ChartLineType.BezierType, "Module 1"));
            ri2.Add(new ChartRequestInfo(Brushes.Green, GetDoubleData(20), ChartLineType.BezierType, "Module 2"));
            ri2.Add(new ChartRequestInfo(Brushes.Blue, GetDoubleData(-20), ChartLineType.BezierType, "Module 3"));
            ucGraph2.RequestData = ri2;

            ObservableCollection<ChartRequestInfo> ri3 = new ObservableCollection<ChartRequestInfo>();
            ri3.Add(new ChartRequestInfo(Brushes.Red, GetDoubleData(0), ChartLineType.BezierKnotsType, "Red Channel"));
            ri3.Add(new ChartRequestInfo(Brushes.Green, GetDoubleData(20), ChartLineType.BezierKnotsType, "Green Channel"));
            ri3.Add(new ChartRequestInfo(Brushes.Blue, GetDoubleData(-20), ChartLineType.BezierKnotsType, "Blue Channel"));
            ucGraph3.RequestData = ri3;

            ObservableCollection<ChartRequestInfo> ri4 = new ObservableCollection<ChartRequestInfo>();
            ri4.Add(new ChartRequestInfo(Brushes.Red, GetDoubleData(0), ChartLineType.PolylineKnotsType, "Red Channel"));
            ri4.Add(new ChartRequestInfo(Brushes.Green, GetDoubleData(20), ChartLineType.PolylineKnotsType, "Green Channel"));
            ri4.Add(new ChartRequestInfo(Brushes.Blue, GetDoubleData(-20), ChartLineType.PolylineKnotsType, "Blue Channel"));
            ucGraph4.RequestData = ri4;

            

            #endregion Similator Data            
        }

        #region Helpper

        private Point [] GetFakeData(int delta)
        {
            //1st line
            Point[] ccp = new Point[20];
            ccp[0] = new Point(1, 20 + delta);
            ccp[1] = new Point(2, 25 + delta);
            ccp[2] = new Point(3, 30 + delta);
            ccp[3] = new Point(4, 40 + delta);
            ccp[4] = new Point(5, 20 + delta);
            ccp[5] = new Point(6, 30 + delta);
            ccp[6] = new Point(7, 60 + delta);
            ccp[7] = new Point(8, 70 + delta);
            ccp[8] = new Point(9, 20 + delta);
            ccp[9] = new Point(10, 30 + delta);
            ccp[10] = new Point(11, 50 + delta);
            ccp[11] = new Point(12, 20 + delta);
            ccp[12] = new Point(13, 30 + delta);
            ccp[13] = new Point(14, 80 + delta);
            ccp[14] = new Point(15, 90 + delta);
            ccp[15] = new Point(16, 80 + delta);
            ccp[16] = new Point(17, 20 + delta);
            ccp[17] = new Point(18, 30 + delta);
            ccp[18] = new Point(19, 50 + delta);
            ccp[19] = new Point(20, 20 + delta);

            //for (int i = 0; i < 19; i++)
            //{
            //    if (i < 10)
            //        ccp.Add(new Point(i, (i+1)));
            //    else
            //        ccp.Add(new Point(i, (19 - i)));
            //}

            //for (int i = 0; i < 20; i++)
            //{
            //    ccp.Add(new Point(i, (float)Math.Sin(i/5f)));
            //}
            return ccp;
        }

        private ObservableCollection<double> GetDoubleData(int delta)
        {
            ObservableCollection<double> data = new ObservableCollection<double>();
            data.Add(20 + delta);
            data.Add(50 + delta);
            data.Add(30 + delta);
            data.Add(40 + delta);
            data.Add(30 + delta);
            data.Add(50 + delta);
            data.Add(25 + delta);
            data.Add(70 + delta);
            data.Add(20 + delta);
            data.Add(60 + delta);

            data.Add(50 + delta);
            data.Add(20 + delta);
            data.Add(40 + delta);
            data.Add(35 + delta);
            data.Add(20 + delta);
            data.Add(40 + delta);
            data.Add(50 + delta);
            data.Add(20 + delta);
            data.Add(90 + delta);
            data.Add(10 + delta);

            data.Add(20 + delta);
            data.Add(50 + delta);
            data.Add(30 + delta);
            data.Add(40 + delta);
            data.Add(30 + delta);
            data.Add(50 + delta);
            data.Add(25 + delta);
            data.Add(70 + delta);
            data.Add(20 + delta);
            data.Add(60 + delta);

            return data;
        }

        #endregion Helpper

           
    }   
}
