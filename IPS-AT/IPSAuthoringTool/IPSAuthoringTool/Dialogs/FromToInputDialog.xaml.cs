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
using System.Threading;
using System.Windows.Threading;
using System.Globalization;

namespace IPSAuthoringTool.Dialogs
{
    /// <summary>
    /// Interaction logic for StringInputDialog.xaml
    /// </summary>
    public partial class FromToInputDialog : UserControl
    {
        public FromToInputDialog()
        {
            InitializeComponent();
            Visibility = Visibility.Hidden;
            _grid.DataContext = this;
        }

        private bool _hideRequest = false;
        private int[] _result = null;
        private UIElement _parent;

        public void SetParent(UIElement parent)
        {
            _parent = parent;
        }

        #region Message

        public string theMessage
        {
            get { return (string)GetValue(FromToInputDialog.MessageProperty); }
            set { SetValue(FromToInputDialog.MessageProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Message.
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty MessageProperty =
            DependencyProperty.Register(
                "Message", typeof(string), typeof(FromToInputDialog),
                new FrameworkPropertyMetadata("Message"));

        #endregion
        
        public int[] ShowHandlerDialog(string message, string from = "", string to = "")
        {
            theMessage = message;
            Visibility = Visibility.Visible;
            fromControl.Text = from;
            toControl.Text = to;

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
            int[] retInts = new int[2];
            int.TryParse(fromControl.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out retInts[0]);
            int.TryParse(toControl.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out retInts[1]);
            _result = retInts;
            HideHandlerDialog();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            _result = null;
            HideHandlerDialog();
        }
    }
}
