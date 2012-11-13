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

namespace IPSAuthoringTool.Dialogs
{
    /// <summary>
    /// Interaction logic for StringInputDialog.xaml
    /// </summary>
    public partial class StringInputDialog : UserControl
    {
        public StringInputDialog()
        {
            InitializeComponent();
            Visibility = Visibility.Hidden;
        }

        private bool _hideRequest = false;
        private string _result = null;
        private UIElement _parent;

        public void SetParent(UIElement parent)
        {
            _parent = parent;
        }

        #region Message

        public string theMessage
        {
            get { return (string)GetValue(StringInputDialog.MessageProperty); }
            set { SetValue(StringInputDialog.MessageProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Message.
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty MessageProperty =
            DependencyProperty.Register(
                "Message", typeof(string), typeof(StringInputDialog),
                new FrameworkPropertyMetadata("Message"));

        #endregion

        public string ShowHandlerDialog(string message)
        {
            theMessage = message;
            Visibility = Visibility.Visible;
            TextControl.Text = "";

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
            _result = TextControl.Text;
            HideHandlerDialog();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            _result = null;
            HideHandlerDialog();
        }
    }
}
