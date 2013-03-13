﻿using System;
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
    public partial class VectorInputDialog : UserControl
    {
        public VectorInputDialog()
        {
            InitializeComponent();
            Visibility = Visibility.Hidden;
            _grid.DataContext = this;
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
            get { return (string)GetValue(VectorInputDialog.MessageProperty); }
            set { SetValue(VectorInputDialog.MessageProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Message.
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty MessageProperty =
            DependencyProperty.Register(
                "Message", typeof(string), typeof(VectorInputDialog),
                new FrameworkPropertyMetadata("Message"));

        #endregion

        public float[] ShowHandlerDialog(string message)
        {
            theMessage = message;
            Visibility = Visibility.Visible;
            xControl.Text = "0";
            yControl.Text = "0";
            zControl.Text = "0";

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
            float[] retFloats = new float[3];
            float.TryParse(xControl.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out retFloats[0]);
            float.TryParse(yControl.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out retFloats[1]);
            float.TryParse(zControl.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out retFloats[2]);
            _result = retFloats;
            HideHandlerDialog();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            _result = null;
            HideHandlerDialog();
        }
    }
}
