using System;
using System.Globalization;
using System.Windows.Controls;
using System.Windows.Data;

namespace MahApps.Metro.Converters
{
    public class PanoramaGroupWidthConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            var itemWidth = double.Parse(values[0].ToString(), culture);
            var itemHeight = double.Parse(values[1].ToString(), culture);
            var groupHeight = double.Parse(values[2].ToString(), culture);

            double ratio = groupHeight / itemHeight;
            var list = (ListBox)values[3];

            double width = Math.Ceiling(list.Items.Count / ratio) + 1;
            width *= itemWidth;
            return width < itemWidth ? itemWidth : width;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
