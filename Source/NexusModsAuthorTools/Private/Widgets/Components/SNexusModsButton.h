#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "NexusModsStyle.h"

class SButton;
class SImage;
class STextBlock;

/**
 * Controls what changes colour when the button is hovered/pressed.
 */
enum class ENexusModsButtonHoverStyle : uint8 {
    None,
    Icon,
    Background,
    Both
};

/**
 * Controls where the icon is placed when both icon and text are shown.
 */
enum class ENexusModsButtonIconPosition : uint8 {
    Left,
    Right
};

/**
 * Helper for easily getting current state of button
 */
enum class ENexusModsButtonState : uint8 {
    Normal,
    Hovered,
    Pressed
};

class SNexusModsButton : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsButton)
        : _IconBrushName(NAME_None)
        , _IconSize(FVector2D(16.0f, 16.0f))
        , _Text(FText::GetEmpty())
        , _IconPosition(ENexusModsButtonIconPosition::Left)
        , _Spacing(6.0f)
        , _Padding(FNexusModsStyle::FormRowLabelPadding)
        , _bTransparentBackground(true)
        , _HoverStyle(ENexusModsButtonHoverStyle::Icon)
        , _IsSelected(false)
        , _NormalIconColor(FLinearColor::White)
        , _HoverIconColor(FNexusModsStyle::NexusOrange)
        , _PressedIconColor(FNexusModsStyle::NexusOrangeLight)
        , _SelectedIconColor(FLinearColor::White)
        , _NormalTextColor(FLinearColor::White)
        , _HoverTextColor(FNexusModsStyle::NexusOrange)
        , _PressedTextColor(FNexusModsStyle::NexusOrangeLight)
        , _SelectedTextColor(FLinearColor::White)
        , _NormalBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.31f))
        , _HoverBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.08f))
        , _PressedBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.14f))
        , _SelectedBackgroundColor(FLinearColor(
            FNexusModsStyle::NexusOrange.R,
            FNexusModsStyle::NexusOrange.G,
            FNexusModsStyle::NexusOrange.B,
            0.25f))
        , _NormalBorderColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.69f))
        , _HoverBorderColor(FNexusModsStyle::NexusOrange)
        , _PressedBorderColor(FNexusModsStyle::NexusOrangeLight)
        , _SelectedBorderColor(FNexusModsStyle::NexusOrange)
        , _BorderBrushName("NexusMods.BaseBorder")
        , _FillBrushName("NexusMods.BaseFill")
        {}
        SLATE_ARGUMENT(FName, IconBrushName)
        SLATE_ARGUMENT(FVector2D, IconSize)
        SLATE_ATTRIBUTE(FText, Text)
        SLATE_ARGUMENT(ENexusModsButtonIconPosition, IconPosition)
        SLATE_ARGUMENT(float, Spacing)
        SLATE_ARGUMENT(FMargin, Padding)
        SLATE_ARGUMENT(bool, bTransparentBackground)
        SLATE_ARGUMENT(ENexusModsButtonHoverStyle, HoverStyle)
        SLATE_ATTRIBUTE(bool, IsSelected)

        SLATE_ARGUMENT(FLinearColor, NormalIconColor)
        SLATE_ARGUMENT(FLinearColor, HoverIconColor)
        SLATE_ARGUMENT(FLinearColor, PressedIconColor)
        SLATE_ARGUMENT(FLinearColor, SelectedIconColor)

        SLATE_ARGUMENT(FLinearColor, NormalTextColor)
        SLATE_ARGUMENT(FLinearColor, HoverTextColor)
        SLATE_ARGUMENT(FLinearColor, PressedTextColor)
        SLATE_ARGUMENT(FLinearColor, SelectedTextColor)

        SLATE_ARGUMENT(FLinearColor, NormalBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, HoverBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, PressedBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, SelectedBackgroundColor)

        SLATE_ARGUMENT(FLinearColor, NormalBorderColor)
        SLATE_ARGUMENT(FLinearColor, HoverBorderColor)
        SLATE_ARGUMENT(FLinearColor, PressedBorderColor)
        SLATE_ARGUMENT(FLinearColor, SelectedBorderColor)

        SLATE_ARGUMENT(FName, BorderBrushName)
        SLATE_ARGUMENT(FName, FillBrushName)

        SLATE_ATTRIBUTE(FText, ToolTipText)
        SLATE_EVENT(FOnClicked, OnClicked)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedRef<SWidget> MakeButtonContent();
    TSharedRef<SWidget> MakeIconWidget();
    TSharedRef<SWidget> MakeTextWidget();

    bool HasIcon() const;
    bool HasText() const;
    bool ShouldHighlightIconOrText() const;
    bool ShouldHighlightBackground() const;

    ENexusModsButtonState GetButtonState() const;

    FSlateColor GetIconColor() const;
    FSlateColor GetTextColor() const;
    FSlateColor GetButtonForegroundColor() const;
    FSlateColor GetButtonBackgroundColor() const;
    FSlateColor GetButtonBorderColor() const;
    EVisibility GetIconVisibility() const;
    EVisibility GetTextVisibility() const;

    void SetupButtonImageBrushes(); 

private:
    FName IconBrushName;
    FVector2D IconSize;
    TAttribute<FText> Text;
    ENexusModsButtonIconPosition IconPosition = ENexusModsButtonIconPosition::Left;
    float Spacing = 6.0f;
    FMargin Padding;

    bool bTransparentBackground = true;
    ENexusModsButtonHoverStyle HoverStyle = ENexusModsButtonHoverStyle::Icon;
    TAttribute<bool> IsSelected;

    FSlateBrush BorderBrush;
    FSlateBrush FillBrush;

    FLinearColor NormalIconColor;
    FLinearColor HoverIconColor;
    FLinearColor PressedIconColor;
    FLinearColor SelectedIconColor;

    FLinearColor NormalTextColor;
    FLinearColor HoverTextColor;
    FLinearColor PressedTextColor;
    FLinearColor SelectedTextColor;

    FLinearColor NormalBackgroundColor;
    FLinearColor HoverBackgroundColor;
    FLinearColor PressedBackgroundColor;
    FLinearColor SelectedBackgroundColor;

    FLinearColor NormalBorderColor;
    FLinearColor HoverBorderColor;
    FLinearColor PressedBorderColor;
    FLinearColor SelectedBorderColor;

    FName BorderBrushName;
    FName FillBrushName;

    FOnClicked OnClicked;

    TSharedPtr<SButton> Button;
    TSharedPtr<SImage> IconImage;
    TSharedPtr<STextBlock> TextBlock;
};
