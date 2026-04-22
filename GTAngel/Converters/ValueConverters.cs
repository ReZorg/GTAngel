using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;

namespace GTA3DE.Wpf.Converters;

public class BoolToVisibilityConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        => value is true ? Visibility.Visible : Visibility.Collapsed;

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => value is Visibility.Visible;
}

public class InverseBoolToVisibilityConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        => value is true ? Visibility.Collapsed : Visibility.Visible;

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => value is Visibility.Collapsed;
}

public class PercentageToWidthConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is double d && parameter is string maxStr && double.TryParse(maxStr, out double max))
            return d * max;
        if (value is double d2)
            return d2 * 200;
        return 0.0;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => throw new NotImplementedException();
}

public class ScoreToColorConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is double score)
        {
            if (score >= 0.8) return new SolidColorBrush(Color.FromRgb(76, 175, 80));   // Green
            if (score >= 0.6) return new SolidColorBrush(Color.FromRgb(255, 193, 7));   // Amber
            if (score >= 0.4) return new SolidColorBrush(Color.FromRgb(255, 152, 0));   // Orange
            return new SolidColorBrush(Color.FromRgb(244, 67, 54));                      // Red
        }
        return new SolidColorBrush(Colors.Gray);
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => throw new NotImplementedException();
}

/// <summary>Converts bool to Green (true) or Gray (false) brush — for status indicator dots.</summary>
public class BoolToGreenGrayConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        => value is true
            ? new SolidColorBrush(Color.FromRgb(76, 175, 80))   // Green
            : new SolidColorBrush(Color.FromRgb(100, 100, 120)); // Gray
    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => throw new NotImplementedException();
}

/// <summary>Converts a float [0,1] to a pixel height (default max=80). Parameter overrides max.</summary>
public class FloatToHeightConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        double max = 80.0;
        if (parameter is string s && double.TryParse(s, out double p)) max = p;
        double raw = value is float f ? (double)f : value is double d ? d : 0.0;
        // Clamp to [0,1] then scale to max pixels
        double clamped = Math.Max(0.0, Math.Min(1.0, raw));
        return Math.Max(1.0, clamped * max);
    }
    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => throw new NotImplementedException();
}

/// <summary>Converts bool to bool (inverse) — for IsEnabled bindings.</summary>
public class InverseBoolConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        => value is bool b ? !b : true;
    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => value is bool b ? !b : false;
}

/// <summary>
/// IMultiValueConverter: takes (progress [0..1], containerWidth) → pixel width.
/// Used for the UE5 launch pipeline progress bar.
/// </summary>
public class ProgressToWidthConverter : IMultiValueConverter
{
    public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
    {
        if (values.Length < 2) return 0.0;
        double progress = values[0] is double d ? d : values[0] is float f ? (double)f : 0.0;
        double width    = values[1] is double w ? w : 240.0;
        return Math.Max(0.0, Math.Min(width, progress * width));
    }
    public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        => throw new NotImplementedException();
}

public class StatusToColorConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        return value?.ToString() switch
        {
            "Keep" => new SolidColorBrush(Color.FromRgb(76, 175, 80)),
            "Discard" => new SolidColorBrush(Color.FromRgb(244, 67, 54)),
            "Baseline" => new SolidColorBrush(Color.FromRgb(33, 150, 243)),
            "Running" => new SolidColorBrush(Color.FromRgb(255, 193, 7)),
            "Crash" => new SolidColorBrush(Color.FromRgb(156, 39, 176)),
            _ => new SolidColorBrush(Colors.Gray)
        };
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => throw new NotImplementedException();
}
