#include "Widgets/Components/SNexusModsTextBox.h"

#include "InputCoreTypes.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SOverlay.h"
#include "NexusModsStyle.h"

void SNexusModsTextBox::Construct(const FArguments& InArgs) {
    ActualText = InArgs._Text.Get();
    OnTextChanged = InArgs._OnTextChanged;
    OnTextCommitted = InArgs._OnTextCommitted;

    bIsReadOnly = InArgs._IsReadOnly;
    IsPassword = InArgs._IsPassword;
    bAllowMultiLine = InArgs._AllowMultiLine;
    bAutoWrapText = InArgs._AutoWrapText;
    Padding = InArgs._Padding;

    NormalBackgroundColor = InArgs._NormalBackgroundColor;
    HoverBackgroundColor = InArgs._HoverBackgroundColor;
    FocusedBackgroundColor = InArgs._FocusedBackgroundColor;
    ReadOnlyBackgroundColor = InArgs._ReadOnlyBackgroundColor;

    NormalBorderColor = InArgs._NormalBorderColor;
    HoverBorderColor = InArgs._HoverBorderColor;
    FocusedBorderColor = InArgs._FocusedBorderColor;
    ReadOnlyBorderColor = InArgs._ReadOnlyBorderColor;

    SetupTextBoxImageBrushes();

    TSharedRef<SScrollBar> VerticalScrollBar =
        SNew(SScrollBar)
            .Orientation(Orient_Vertical)
            .Thickness(FVector2D(8.0f, 8.0f))
            .Style(&FNexusModsStyle::Get().GetWidgetStyle<FScrollBarStyle>("NexusMods.ScrollBar"));

    ChildSlot [
        SNew(SOverlay)
        + SOverlay::Slot() [
            SNew(SImage)
                .Image(&FillBrush)
                .ColorAndOpacity(this, &SNexusModsTextBox::GetTextBoxBackgroundColor)
        ]
        + SOverlay::Slot() [
            SNew(SImage)
                .Image(&BorderBrush)
                .ColorAndOpacity(this, &SNexusModsTextBox::GetTextBoxBorderColor)
        ]
        + SOverlay::Slot().Padding(Padding) [
            SAssignNew(TextBox, SMultiLineEditableTextBox)
                .Text(this, &SNexusModsTextBox::GetDisplayText)
                .HintText(InArgs._HintText)
                .IsReadOnly(this, &SNexusModsTextBox::IsTextBoxReadOnly)
                .OnTextChanged(this, &SNexusModsTextBox::OnInternalTextChanged)
                .OnTextCommitted(this, &SNexusModsTextBox::OnInternalTextCommitted)
                .AllowMultiLine(bAllowMultiLine)
                .AutoWrapText(bAutoWrapText)
                .VScrollBar(VerticalScrollBar)
                .Style(&FNexusModsStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NexusMods.TextBox.Inner"))
                .Cursor(EMouseCursor::Hand)
        ]
    ];
}

void SNexusModsTextBox::SetupTextBoxImageBrushes() {
    const FSlateBrush* Border = FNexusModsStyle::Get().GetBrush("NexusMods.BaseBorder");
    const FSlateBrush* Fill = FNexusModsStyle::Get().GetBrush("NexusMods.BaseFill");

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

ENexusModsTextBoxState SNexusModsTextBox::GetTextBoxState() const {
    if (IsTextBoxReadOnly()) {
        return ENexusModsTextBoxState::ReadOnly;
    }

    if (TextBox.IsValid() && TextBox->HasKeyboardFocus()) {
        return ENexusModsTextBoxState::Focused;
    }

    if (IsHovered()) {
        return ENexusModsTextBoxState::Hovered;
    }

    return ENexusModsTextBoxState::Normal;
}

FSlateColor SNexusModsTextBox::GetTextBoxBackgroundColor() const {
    switch (GetTextBoxState()) {
    case ENexusModsTextBoxState::ReadOnly:
        return ReadOnlyBackgroundColor;
    case ENexusModsTextBoxState::Focused:
        return FocusedBackgroundColor;
    case ENexusModsTextBoxState::Hovered:
        return HoverBackgroundColor;
    default:
        return NormalBackgroundColor;
    }
}

FSlateColor SNexusModsTextBox::GetTextBoxBorderColor() const {
    switch (GetTextBoxState()) {
    case ENexusModsTextBoxState::ReadOnly:
        return ReadOnlyBorderColor;
    case ENexusModsTextBoxState::Focused:
        return FocusedBorderColor;
    case ENexusModsTextBoxState::Hovered:
        return HoverBorderColor;
    default:
        return NormalBorderColor;
    }
}

FText SNexusModsTextBox::GetDisplayText() const {
    if (!IsProtectedTextHidden()) {
        return ActualText;
    }

    return FText::FromString(MakeMaskedText(ActualText.ToString()));
}

bool SNexusModsTextBox::IsProtectedTextHidden() const {
    return IsPassword.Get(false);
}

bool SNexusModsTextBox::IsTextBoxReadOnly() const {
    return bIsReadOnly || IsProtectedTextHidden();
}

FString SNexusModsTextBox::MakeMaskedText(const FString& InText) const {
    FString MaskedText;
    MaskedText.Reserve(InText.Len());

    for (int32 CharacterIndex = 0; CharacterIndex < InText.Len(); ++CharacterIndex) {
        MaskedText.AppendChar(TEXT('*'));
    }

    return MaskedText;
}

void SNexusModsTextBox::OnInternalTextChanged(const FText& InText) {
    if (bIsRefreshingDisplayText || IsProtectedTextHidden()) {
        return;
    }

    ActualText = InText;
    OnTextChanged.ExecuteIfBound(ActualText);
}

void SNexusModsTextBox::OnInternalTextCommitted(const FText& InText, ETextCommit::Type CommitType) {
    if (!IsProtectedTextHidden()) {
        ActualText = InText;
    }

    OnTextCommitted.ExecuteIfBound(ActualText, CommitType);
}

FText SNexusModsTextBox::GetText() const {
    return ActualText;
}

void SNexusModsTextBox::SetText(const FText& InText) {
    ActualText = InText;
    RefreshDisplayText();
}

void SNexusModsTextBox::RefreshDisplayText() {
    if (TextBox.IsValid()) {
        bIsRefreshingDisplayText = true;
        TextBox->SetText(GetDisplayText());
        bIsRefreshingDisplayText = false;
        ScrollToEnd();
    }
}

void SNexusModsTextBox::ScrollToEnd() {
    if (!TextBox.IsValid()) return;
    const FString Text = GetText().ToString();
    TArray<FString> Lines;
    Text.ParseIntoArrayLines(Lines, false);
    const int32 LastLineIndex = FMath::Max(0, Lines.Num() - 1);
    TextBox->ScrollTo(FTextLocation(LastLineIndex, 0));
}
