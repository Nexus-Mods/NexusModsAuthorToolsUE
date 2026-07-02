#include "Widgets/Components/SNexusModsButton.h"

#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/AppStyle.h"
#include "NexusModsStyle.h"

void SNexusModsButton::Construct(const FArguments& InArgs) {
    IconBrushName = InArgs._IconBrushName;
    IconSize = InArgs._IconSize;
    Text = InArgs._Text;
    IconPosition = InArgs._IconPosition;
    Spacing = InArgs._Spacing;
    Padding = InArgs._Padding;
    bTransparentBackground = InArgs._bTransparentBackground;
    HoverStyle = InArgs._HoverStyle;
    IsSelected = InArgs._IsSelected;
    // colorings:
    NormalIconColor = InArgs._NormalIconColor;
    HoverIconColor = InArgs._HoverIconColor;
    PressedIconColor = InArgs._PressedIconColor;
    SelectedIconColor = InArgs._SelectedIconColor;
    NormalTextColor = InArgs._NormalTextColor;
    HoverTextColor = InArgs._HoverTextColor;
    PressedTextColor = InArgs._PressedTextColor;
    SelectedTextColor = InArgs._SelectedTextColor;
    NormalBackgroundColor = InArgs._NormalBackgroundColor;
    HoverBackgroundColor = InArgs._HoverBackgroundColor;
    PressedBackgroundColor = InArgs._PressedBackgroundColor;
    SelectedBackgroundColor = InArgs._SelectedBackgroundColor;
    NormalBorderColor = InArgs._NormalBorderColor;
    HoverBorderColor = InArgs._HoverBorderColor;
    PressedBorderColor = InArgs._PressedBorderColor;
    SelectedBorderColor = InArgs._SelectedBorderColor;

    BorderBrushName = InArgs._BorderBrushName;
    FillBrushName = InArgs._FillBrushName;

    // callbacks:
    OnClicked = InArgs._OnClicked;

    if (IconBrushName.IsNone()) {
        Spacing = 0.0f;
    }

    SetupButtonImageBrushes();

    ChildSlot [
        SAssignNew(Button, SButton)
            //.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
            .ButtonStyle(&FButtonStyle::GetDefault())
            .ContentPadding(FMargin(0.0f))
            .ToolTipText(InArgs._ToolTipText)
            .ForegroundColor(this, &SNexusModsButton::GetButtonForegroundColor)
            .ButtonColorAndOpacity(this, &SNexusModsButton::GetButtonBackgroundColor)
            .OnClicked(OnClicked) [
                SNew(SOverlay)
                + SOverlay::Slot() [
                    SNew(SImage).Image(&FillBrush).ColorAndOpacity(this, &SNexusModsButton::GetButtonBackgroundColor)
                ]
                + SOverlay::Slot() [
                    SNew(SImage).Image(&BorderBrush).ColorAndOpacity(this, &SNexusModsButton::GetButtonBorderColor)
                ]
                + SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(Padding) [
                    MakeButtonContent()
                ]
            ]
    ];
}

void SNexusModsButton::SetupButtonImageBrushes() {
    const FSlateBrush* Border = FNexusModsStyle::Get().GetBrush(BorderBrushName);
    const FSlateBrush* Fill = FNexusModsStyle::Get().GetBrush(FillBrushName);

    if (Border) {
        BorderBrush = *Border;
        BorderBrush.DrawAs = ESlateBrushDrawType::Box;
        BorderBrush.Margin = FMargin(0.5f);
    }
    if (Fill) {
        FillBrush = *Fill;
        FillBrush.DrawAs = ESlateBrushDrawType::Box;
        FillBrush.Margin = FMargin(0.5f);
    }
}

TSharedRef<SWidget> SNexusModsButton::MakeButtonContent() {
    const FMargin LeftTextPadding = FMargin(Spacing, 0.0f, 0.0f, 0.0f);
    const FMargin RightTextPadding = FMargin(0.0f, 0.0f, Spacing, 0.0f);

    if (IconPosition == ENexusModsButtonIconPosition::Right) {
        return
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(RightTextPadding) [
                MakeTextWidget()
            ]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
                MakeIconWidget()
            ];
    }

    return
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
            MakeIconWidget()
        ]
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(LeftTextPadding) [
            MakeTextWidget()
        ];
}

TSharedRef<SWidget> SNexusModsButton::MakeIconWidget() {
    return
        SNew(SBox)
        .WidthOverride(IconSize.X)
        .HeightOverride(IconSize.Y)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center) [
            SAssignNew(IconImage, SImage)
            .Image(FNexusModsStyle::Get().GetBrush(IconBrushName))
            .ColorAndOpacity(this, &SNexusModsButton::GetIconColor)
            .Visibility(this, &SNexusModsButton::GetIconVisibility)
        ];
}

TSharedRef<SWidget> SNexusModsButton::MakeTextWidget() {
    return
        SAssignNew(TextBlock, STextBlock)
        .Text(Text)
        .Font(FAppStyle::GetFontStyle("BoldFont"))
        .ColorAndOpacity(this, &SNexusModsButton::GetTextColor)
        .Visibility(this, &SNexusModsButton::GetTextVisibility);
}

ENexusModsButtonState SNexusModsButton::GetButtonState() const {
    if (!Button.IsValid()) return ENexusModsButtonState::Normal;
    if (Button->IsPressed()) return ENexusModsButtonState::Pressed;
    if (Button->IsHovered()) return ENexusModsButtonState::Hovered;
    return ENexusModsButtonState::Normal;
}

bool SNexusModsButton::HasIcon() const {
    return !IconBrushName.IsNone();
}

bool SNexusModsButton::HasText() const {
    return !Text.Get(FText::GetEmpty()).IsEmpty();
}

bool SNexusModsButton::ShouldHighlightIconOrText() const {
    return
        HoverStyle == ENexusModsButtonHoverStyle::Icon ||
        HoverStyle == ENexusModsButtonHoverStyle::Both;
}

bool SNexusModsButton::ShouldHighlightBackground() const {
    return
        HoverStyle == ENexusModsButtonHoverStyle::Background ||
        HoverStyle == ENexusModsButtonHoverStyle::Both;
}

EVisibility SNexusModsButton::GetIconVisibility() const {
    return HasIcon() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SNexusModsButton::GetTextVisibility() const {
    return HasText() ? EVisibility::Visible : EVisibility::Collapsed;
}

FSlateColor SNexusModsButton::GetButtonForegroundColor() const {
    return GetIconColor();
}

FSlateColor SNexusModsButton::GetIconColor() const {
    if (IsSelected.Get(false)) {
        return SelectedIconColor;
    }
    if (!ShouldHighlightIconOrText()) {
        return NormalIconColor;
    }
    switch (GetButtonState()) {
    case ENexusModsButtonState::Pressed:
        return PressedIconColor;
    case ENexusModsButtonState::Hovered:
        return HoverIconColor;
    default:
        return NormalIconColor;
    }
}

FSlateColor SNexusModsButton::GetTextColor() const {
    if (IsSelected.Get(false)) {
        return SelectedTextColor;
    }
    if (!ShouldHighlightIconOrText()) {
        return NormalTextColor;
    }
    switch (GetButtonState()) {
    case ENexusModsButtonState::Pressed:
        return PressedTextColor;
    case ENexusModsButtonState::Hovered:
        return HoverTextColor;
    default:
        return NormalTextColor;
    }
}


FSlateColor SNexusModsButton::GetButtonBackgroundColor() const {
    if (IsSelected.Get(false)) {
        return SelectedBackgroundColor;
    }
    FLinearColor DefaultColor = bTransparentBackground ? FLinearColor::Transparent : NormalBackgroundColor;
    if (!ShouldHighlightBackground()) {
        return DefaultColor;
    }
    switch (GetButtonState()) {
        case ENexusModsButtonState::Pressed:
            return PressedBackgroundColor;
        case ENexusModsButtonState::Hovered:
            return HoverBackgroundColor;
        default:
            return DefaultColor;
    }
}


FSlateColor SNexusModsButton::GetButtonBorderColor() const {
    if (IsSelected.Get(false)) {
        return SelectedBorderColor;
    }
    FLinearColor DefaultColor = bTransparentBackground ? FLinearColor::Transparent : NormalBorderColor;
    if (!ShouldHighlightBackground()) {
        return DefaultColor;
    }
    switch (GetButtonState()) {
    case ENexusModsButtonState::Pressed:
        return PressedBorderColor;
    case ENexusModsButtonState::Hovered:
        return HoverBorderColor;
    default:
        return DefaultColor;
    }
}
