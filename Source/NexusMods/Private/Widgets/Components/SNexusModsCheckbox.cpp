#include "Widgets/Components/SNexusModsCheckbox.h"

#include "InputCoreTypes.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "NexusModsStyle.h"

void SNexusModsCheckbox::Construct(const FArguments& InArgs) {
    OnCheckStateChanged = InArgs._OnCheckStateChanged;

    bIsCheckboxEnabled = InArgs._CheckboxEnabled;
    CachedCheckedState = InArgs._IsChecked;
    Size = InArgs._Size;

    NormalBackgroundColor = InArgs._NormalBackgroundColor;
    HoverBackgroundColor = InArgs._HoverBackgroundColor;
    CheckedBackgroundColor = InArgs._CheckedBackgroundColor;
    DisabledBackgroundColor = InArgs._DisabledBackgroundColor;

    NormalBorderColor = InArgs._NormalBorderColor;
    HoverBorderColor = InArgs._HoverBorderColor;
    CheckedBorderColor = InArgs._CheckedBorderColor;
    DisabledBorderColor = InArgs._DisabledBorderColor;

    CheckmarkColor = InArgs._CheckmarkColor;
    CheckmarkBrushName = InArgs._CheckmarkBrushName;
    CheckmarkSize = InArgs._CheckmarkSize;
    ToolTipText = InArgs._ToolTipText;

    SetupCheckboxImageBrushes();

    ChildSlot [
        SNew(SBox)
            .ToolTipText(ToolTipText)
            .WidthOverride(Size.X)
            .HeightOverride(Size.Y) [
            SNew(SOverlay)
            + SOverlay::Slot() [
                SNew(SImage)
                    .Image(&FillBrush)
                    .ColorAndOpacity(this, &SNexusModsCheckbox::GetCheckboxBackgroundColor)
            ]
            + SOverlay::Slot() [
                SNew(SImage)
                    .Image(&BorderBrush)
                    .ColorAndOpacity(this, &SNexusModsCheckbox::GetCheckboxBorderColor)
            ]
            + SOverlay::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center) [
                SNew(SBox)
                    .WidthOverride(CheckmarkSize.X)
                    .HeightOverride(CheckmarkSize.Y) [
                    SNew(SImage)
                        .Image(CheckmarkBrush)
                        .ColorAndOpacity(this, &SNexusModsCheckbox::GetCheckmarkColor)
                        .Visibility(this, &SNexusModsCheckbox::GetCheckmarkVisibility)
                ]
            ]
        ]
    ];
}

void SNexusModsCheckbox::SetupCheckboxImageBrushes() {
    const FSlateBrush* Border = FNexusModsStyle::Get().GetBrush("NexusMods.BaseBorder");
    const FSlateBrush* Fill = FNexusModsStyle::Get().GetBrush("NexusMods.BaseFill");
    CheckmarkBrush = FNexusModsStyle::Get().GetBrush(CheckmarkBrushName);
    if (!CheckmarkBrush) {
        CheckmarkBrush = FCoreStyle::Get().GetBrush("WhiteBrush");
    }
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

FReply SNexusModsCheckbox::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
    if (!bIsCheckboxEnabled || MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton) {
        return FReply::Unhandled();
    }

    const ECheckBoxState NewState = IsChecked()
        ? ECheckBoxState::Unchecked
        : ECheckBoxState::Checked;

    SetIsChecked(NewState);
    OnCheckStateChanged.ExecuteIfBound(NewState);

    return FReply::Handled();
}

bool SNexusModsCheckbox::IsChecked() const {
    return GetCheckedState() == ECheckBoxState::Checked;
}

ECheckBoxState SNexusModsCheckbox::GetCheckedState() const {
    return CachedCheckedState;
}

void SNexusModsCheckbox::SetIsChecked(ECheckBoxState InCheckedState) {
    CachedCheckedState = InCheckedState;
}

ENexusModsCheckboxState SNexusModsCheckbox::GetCheckboxState() const {
    if (!bIsCheckboxEnabled) return ENexusModsCheckboxState::Disabled;
    if (IsHovered()) return ENexusModsCheckboxState::Hovered;
    if (IsChecked()) return ENexusModsCheckboxState::Checked;
    return ENexusModsCheckboxState::Normal;
}

EVisibility SNexusModsCheckbox::GetCheckmarkVisibility() const {
    return IsChecked() ? EVisibility::Visible : EVisibility::Collapsed;
}

FSlateColor SNexusModsCheckbox::GetCheckboxBackgroundColor() const {
    ENexusModsCheckboxState CheckboxState = GetCheckboxState();
    switch (CheckboxState) {
    case ENexusModsCheckboxState::Disabled:
        return DisabledBackgroundColor;
    case ENexusModsCheckboxState::Checked:
        return CheckboxState == ENexusModsCheckboxState::Hovered ? HoverBackgroundColor : NormalBackgroundColor;
        //return CheckedBackgroundColor;
    case ENexusModsCheckboxState::Hovered:
        return HoverBackgroundColor;
    default:
        return NormalBackgroundColor;
    }
}

FSlateColor SNexusModsCheckbox::GetCheckboxBorderColor() const {
    ENexusModsCheckboxState CheckboxState = GetCheckboxState();
    switch (CheckboxState) {
    case ENexusModsCheckboxState::Disabled:
        return DisabledBorderColor;
    case ENexusModsCheckboxState::Checked:
        return CheckboxState == ENexusModsCheckboxState::Hovered ? HoverBorderColor : NormalBorderColor;
        //return CheckedBorderColor;
    case ENexusModsCheckboxState::Hovered:
        return HoverBorderColor;
    default:
        return NormalBorderColor;
    }
}

FSlateColor SNexusModsCheckbox::GetCheckmarkColor() const {
    return CheckmarkColor;
}
