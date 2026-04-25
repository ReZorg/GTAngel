using System.Globalization;
using System.Windows;
using System.Windows.Media;
using GTAngel.Converters;
using Xunit;

#pragma warning disable CS8625 // null parameter for non-nullable CultureInfo
namespace GTAngel.Tests.Converters;

/// <summary>
/// Tests for WPF IValueConverter implementations.
/// These tests run on the default STA thread context provided by xunit.
/// </summary>
public class ValueConverterTests
{
    // ── BoolToVisibilityConverter ──────────────────────────────────────────

    [Fact]
    public void BoolToVisibility_True_ReturnsVisible()
    {
        var conv = new BoolToVisibilityConverter();
        var result = conv.Convert(true, typeof(Visibility), null, CultureInfo.InvariantCulture);
        Assert.Equal(Visibility.Visible, result);
    }

    [Fact]
    public void BoolToVisibility_False_ReturnsCollapsed()
    {
        var conv = new BoolToVisibilityConverter();
        var result = conv.Convert(false, typeof(Visibility), null, CultureInfo.InvariantCulture);
        Assert.Equal(Visibility.Collapsed, result);
    }

    [Fact]
    public void BoolToVisibility_ConvertBack_VisibleReturnsTrue()
    {
        var conv = new BoolToVisibilityConverter();
        var result = conv.ConvertBack(Visibility.Visible, typeof(bool), null, CultureInfo.InvariantCulture);
        Assert.Equal(true, result);
    }

    [Fact]
    public void BoolToVisibility_ConvertBack_CollapsedReturnsFalse()
    {
        var conv = new BoolToVisibilityConverter();
        var result = conv.ConvertBack(Visibility.Collapsed, typeof(bool), null, CultureInfo.InvariantCulture);
        Assert.Equal(false, result);
    }

    // ── InverseBoolToVisibilityConverter ──────────────────────────────────

    [Fact]
    public void InverseBoolToVisibility_True_ReturnsCollapsed()
    {
        var conv = new InverseBoolToVisibilityConverter();
        var result = conv.Convert(true, typeof(Visibility), null, CultureInfo.InvariantCulture);
        Assert.Equal(Visibility.Collapsed, result);
    }

    [Fact]
    public void InverseBoolToVisibility_False_ReturnsVisible()
    {
        var conv = new InverseBoolToVisibilityConverter();
        var result = conv.Convert(false, typeof(Visibility), null, CultureInfo.InvariantCulture);
        Assert.Equal(Visibility.Visible, result);
    }

    // ── InverseBoolConverter ──────────────────────────────────────────────

    [Fact]
    public void InverseBool_True_ReturnsFalse()
    {
        var conv = new InverseBoolConverter();
        Assert.Equal(false, conv.Convert(true, typeof(bool), null, CultureInfo.InvariantCulture));
    }

    [Fact]
    public void InverseBool_False_ReturnsTrue()
    {
        var conv = new InverseBoolConverter();
        Assert.Equal(true, conv.Convert(false, typeof(bool), null, CultureInfo.InvariantCulture));
    }

    [Fact]
    public void InverseBool_ConvertBack_Inverts()
    {
        var conv = new InverseBoolConverter();
        Assert.Equal(true, conv.ConvertBack(false, typeof(bool), null, CultureInfo.InvariantCulture));
        Assert.Equal(false, conv.ConvertBack(true, typeof(bool), null, CultureInfo.InvariantCulture));
    }

    [Fact]
    public void InverseBool_NonBoolInput_ReturnsTrue()
    {
        var conv = new InverseBoolConverter();
        Assert.Equal(true, conv.Convert("not a bool", typeof(bool), null, CultureInfo.InvariantCulture));
    }

    // ── ScoreToColorConverter ─────────────────────────────────────────────

    [Theory]
    [InlineData(0.9, 76, 175, 80)]   // Green
    [InlineData(0.8, 76, 175, 80)]   // Green boundary
    [InlineData(0.7, 255, 193, 7)]   // Amber
    [InlineData(0.6, 255, 193, 7)]   // Amber boundary
    [InlineData(0.5, 255, 152, 0)]   // Orange
    [InlineData(0.4, 255, 152, 0)]   // Orange boundary
    [InlineData(0.3, 244, 67, 54)]   // Red
    [InlineData(0.0, 244, 67, 54)]   // Red (zero)
    public void ScoreToColor_ScoreRange_ReturnsCorrectColor(double score, byte r, byte g, byte b)
    {
        var conv = new ScoreToColorConverter();
        var result = conv.Convert(score, typeof(SolidColorBrush), null, CultureInfo.InvariantCulture);
        var brush = Assert.IsType<SolidColorBrush>(result);
        Assert.Equal(r, brush.Color.R);
        Assert.Equal(g, brush.Color.G);
        Assert.Equal(b, brush.Color.B);
    }

    [Fact]
    public void ScoreToColor_NonDoubleInput_ReturnsGray()
    {
        var conv = new ScoreToColorConverter();
        var result = conv.Convert("not-a-double", typeof(SolidColorBrush), null, CultureInfo.InvariantCulture);
        var brush = Assert.IsType<SolidColorBrush>(result);
        Assert.Equal(Colors.Gray, brush.Color);
    }

    // ── StatusToColorConverter ────────────────────────────────────────────

    [Theory]
    [InlineData("Keep", 76, 175, 80)]
    [InlineData("Discard", 244, 67, 54)]
    [InlineData("Baseline", 33, 150, 243)]
    [InlineData("Running", 255, 193, 7)]
    [InlineData("Crash", 156, 39, 176)]
    public void StatusToColor_KnownStatus_ReturnsCorrectColor(string status, byte r, byte g, byte b)
    {
        var conv = new StatusToColorConverter();
        var result = conv.Convert(status, typeof(SolidColorBrush), null, CultureInfo.InvariantCulture);
        var brush = Assert.IsType<SolidColorBrush>(result);
        Assert.Equal(r, brush.Color.R);
        Assert.Equal(g, brush.Color.G);
        Assert.Equal(b, brush.Color.B);
    }

    [Fact]
    public void StatusToColor_UnknownStatus_ReturnsGray()
    {
        var conv = new StatusToColorConverter();
        var result = conv.Convert("UnknownStatus", typeof(SolidColorBrush), null, CultureInfo.InvariantCulture);
        var brush = Assert.IsType<SolidColorBrush>(result);
        Assert.Equal(Colors.Gray, brush.Color);
    }

    [Fact]
    public void StatusToColor_NullInput_ReturnsGray()
    {
        var conv = new StatusToColorConverter();
        var result = conv.Convert(null!, typeof(SolidColorBrush), null, CultureInfo.InvariantCulture);
        var brush = Assert.IsType<SolidColorBrush>(result);
        Assert.Equal(Colors.Gray, brush.Color);
    }

    // ── PercentageToWidthConverter ────────────────────────────────────────

    [Fact]
    public void PercentageToWidth_WithMaxParam_ScalesCorrectly()
    {
        var conv = new PercentageToWidthConverter();
        var result = conv.Convert(0.5, typeof(double), "200", CultureInfo.InvariantCulture);
        Assert.Equal(100.0, result);
    }

    [Fact]
    public void PercentageToWidth_NoParam_UsesDefault200()
    {
        var conv = new PercentageToWidthConverter();
        var result = conv.Convert(0.5, typeof(double), null, CultureInfo.InvariantCulture);
        Assert.Equal(100.0, result);
    }

    [Fact]
    public void PercentageToWidth_ZeroValue_ReturnsZero()
    {
        var conv = new PercentageToWidthConverter();
        var result = conv.Convert(0.0, typeof(double), "300", CultureInfo.InvariantCulture);
        Assert.Equal(0.0, result);
    }

    [Fact]
    public void PercentageToWidth_NonDoubleInput_ReturnsZero()
    {
        var conv = new PercentageToWidthConverter();
        var result = conv.Convert("bad", typeof(double), "100", CultureInfo.InvariantCulture);
        Assert.Equal(0.0, result);
    }

    // ── FloatToHeightConverter ────────────────────────────────────────────

    [Fact]
    public void FloatToHeight_OneFloat_ReturnsMaxHeight()
    {
        var conv = new FloatToHeightConverter();
        var result = conv.Convert(1.0f, typeof(double), "80", CultureInfo.InvariantCulture);
        Assert.Equal(80.0, result);
    }

    [Fact]
    public void FloatToHeight_ZeroFloat_ReturnsOne()
    {
        // Clamped to minimum of 1 pixel
        var conv = new FloatToHeightConverter();
        var result = conv.Convert(0.0f, typeof(double), "80", CultureInfo.InvariantCulture);
        Assert.Equal(1.0, result);
    }

    [Fact]
    public void FloatToHeight_HalfFloat_ReturnsHalfMax()
    {
        var conv = new FloatToHeightConverter();
        var result = conv.Convert(0.5f, typeof(double), "80", CultureInfo.InvariantCulture);
        Assert.Equal(40.0, result);
    }

    [Fact]
    public void FloatToHeight_DoubleInput_WorksAsWellAsFloat()
    {
        var conv = new FloatToHeightConverter();
        var result = conv.Convert(0.5d, typeof(double), "100", CultureInfo.InvariantCulture);
        Assert.Equal(50.0, result);
    }

    [Fact]
    public void FloatToHeight_OverOneFloat_ClampsToMax()
    {
        var conv = new FloatToHeightConverter();
        var result = conv.Convert(2.0f, typeof(double), "80", CultureInfo.InvariantCulture);
        Assert.Equal(80.0, result);
    }

    [Fact]
    public void FloatToHeight_NegativeFloat_ClampsToOne()
    {
        var conv = new FloatToHeightConverter();
        var result = conv.Convert(-0.5f, typeof(double), "80", CultureInfo.InvariantCulture);
        Assert.Equal(1.0, result);
    }

    // ── BoolToGreenGrayConverter ──────────────────────────────────────────

    [Fact]
    public void BoolToGreenGray_True_ReturnsGreen()
    {
        var conv = new BoolToGreenGrayConverter();
        var result = conv.Convert(true, typeof(SolidColorBrush), null, CultureInfo.InvariantCulture);
        var brush = Assert.IsType<SolidColorBrush>(result);
        Assert.Equal(Color.FromRgb(76, 175, 80), brush.Color);
    }

    [Fact]
    public void BoolToGreenGray_False_ReturnsGray()
    {
        var conv = new BoolToGreenGrayConverter();
        var result = conv.Convert(false, typeof(SolidColorBrush), null, CultureInfo.InvariantCulture);
        var brush = Assert.IsType<SolidColorBrush>(result);
        Assert.Equal(Color.FromRgb(100, 100, 120), brush.Color);
    }

    // ── ProgressToWidthConverter (IMultiValueConverter) ──────────────────

    [Fact]
    public void ProgressToWidth_HalfProgressFullWidth_ReturnsHalfWidth()
    {
        var conv = new ProgressToWidthConverter();
        var values = new object[] { 0.5, 240.0 };
        var result = conv.Convert(values, typeof(double), null, CultureInfo.InvariantCulture);
        Assert.Equal(120.0, result);
    }

    [Fact]
    public void ProgressToWidth_FullProgress_ReturnsFullWidth()
    {
        var conv = new ProgressToWidthConverter();
        var values = new object[] { 1.0, 200.0 };
        var result = conv.Convert(values, typeof(double), null, CultureInfo.InvariantCulture);
        Assert.Equal(200.0, result);
    }

    [Fact]
    public void ProgressToWidth_ZeroProgress_ReturnsZero()
    {
        var conv = new ProgressToWidthConverter();
        var values = new object[] { 0.0, 200.0 };
        var result = conv.Convert(values, typeof(double), null, CultureInfo.InvariantCulture);
        Assert.Equal(0.0, result);
    }

    [Fact]
    public void ProgressToWidth_OverOneProgress_ClampsToWidth()
    {
        var conv = new ProgressToWidthConverter();
        var values = new object[] { 2.0, 200.0 };
        var result = conv.Convert(values, typeof(double), null, CultureInfo.InvariantCulture);
        Assert.Equal(200.0, result);
    }

    [Fact]
    public void ProgressToWidth_NegativeProgress_ClampsToZero()
    {
        var conv = new ProgressToWidthConverter();
        var values = new object[] { -1.0, 200.0 };
        var result = conv.Convert(values, typeof(double), null, CultureInfo.InvariantCulture);
        Assert.Equal(0.0, result);
    }

    [Fact]
    public void ProgressToWidth_EmptyValues_ReturnsZero()
    {
        var conv = new ProgressToWidthConverter();
        var result = conv.Convert(Array.Empty<object>(), typeof(double), null, CultureInfo.InvariantCulture);
        Assert.Equal(0.0, result);
    }
}
