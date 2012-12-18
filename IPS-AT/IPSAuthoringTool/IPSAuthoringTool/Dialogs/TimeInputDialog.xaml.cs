using System;
using System.Collections.Generic;
using System.Linq;
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
using System.Windows.Threading;
using System.Threading;
using System.Globalization;

namespace IPSAuthoringTool.Dialogs
{
    /// <summary>
    /// Interaction logic for TimeInputDialog.xaml
    /// </summary>
    public partial class TimeInputDialog : UserControl
    {
        public TimeInputDialog()
        {
            InitializeComponent();
            Visibility = Visibility.Hidden;
        }

        private bool _hideRequest = false;
        private float[] _result = null;
        private UIElement _parent;

        public void SetParent(UIElement parent)
        {
            _parent = parent;
        }

        #region Message

        public string theMessage
        {
            get { return (string)GetValue(TimeInputDialog.MessageProperty); }
            set { SetValue(TimeInputDialog.MessageProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Message.
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty MessageProperty =
            DependencyProperty.Register(
                "Message", typeof(string), typeof(TimeInputDialog),
                new FrameworkPropertyMetadata("Message"));

        #endregion

        public float[] ShowHandlerDialog(string message)
        {
            theMessage = message;
            Visibility = Visibility.Visible;
            RangeSlider.RangeStartSelected = RangeSlider.RangeStart;
            RangeSlider.RangeStopSelected = RangeSlider.RangeStop;

            _parent.IsEnabled = false;

            _hideRequest = false;
            while (!_hideRequest)
            {
                // HACK: Stop the thread if the application is about to close
                if (this.Dispatcher.HasShutdownStarted ||
                    this.Dispatcher.HasShutdownFinished)
                {
                    break;
                }

                // HACK: Simulate "DoEvents"
                this.Dispatcher.Invoke(
                    DispatcherPriority.Background,
                    new ThreadStart(delegate { }));
                Thread.Sleep(20);
            }

            return _result;
        }

        public float[] ShowHandlerDialog(string message, int start, int stop)
        {
            theMessage = message;
            Visibility = Visibility.Visible;
            RangeSlider.RangeStartSelected = start;
            RangeSlider.RangeStopSelected = stop;

            _parent.IsEnabled = false;

            _hideRequest = false;
            while (!_hideRequest)
            {
                // HACK: Stop the thread if the application is about to close
                if (this.Dispatcher.HasShutdownStarted ||
                    this.Dispatcher.HasShutdownFinished)
                {
                    break;
                }

                // HACK: Simulate "DoEvents"
                this.Dispatcher.Invoke(
                    DispatcherPriority.Background,
                    new ThreadStart(delegate { }));
                Thread.Sleep(20);
            }

            return _result;
        }

        private void HideHandlerDialog()
        {
            _hideRequest = true;
            Visibility = Visibility.Hidden;
            _parent.IsEnabled = true;
        }

        private void OkButton_Click(object sender, RoutedEventArgs e)
        {
            _result = new float[2] {RangeSlider.RangeStartSelected, RangeSlider.RangeStopSelected};
            HideHandlerDialog();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            _result = null;
            HideHandlerDialog();
        }

        private void RangeSlider_RangeSelectionChanged_1(object sender, MahApps.Metro.Controls.RangeSelectionChangedEventArgs e)
        {
            MahApps.Metro.Controls.RangeSlider rs = (MahApps.Metro.Controls.RangeSlider)sender;
            RangeLabel.Content = (float)rs.RangeStartSelected / 1000 + " to " + (float)rs.RangeStopSelected / 1000;
        }
    }
}
