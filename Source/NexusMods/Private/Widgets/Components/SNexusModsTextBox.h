#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "NexusModsStyle.h"

/**
 * Helper for easily getting current state of the text box chrome.
 */
enum class ENexusModsTextBoxState : uint8 {
    Normal,
    Hovered,
    Focused,
    ReadOnly
};

class SMultiLineEditableTextBox;

/**
 * Nexus Mods styled editable text box.
 *
 * This intentionally uses SMultiLineEditableTextBox with AllowMultiLine(false)
 * rather than SEditableTextBox, because the multiline editable text path honours
 * the selected background colour from the text style without requiring engine patches.
 *
 * Password/protected mode is implemented by masking the text shown in the multiline
 * widget while keeping the real value inside this wrapper. That preserves the
 * custom selection background behaviour from SMultiLineEditableTextBox.
 */
class SNexusModsTextBox : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsTextBox)
        : _Text(FText::GetEmpty())
        , _HintText(FText::GetEmpty())
        , _IsReadOnly(false)
        , _IsPassword(false)
        , _AllowMultiLine(false)
        , _AutoWrapText(false)
        , _Padding(FNexusModsStyle::FormRowLabelPadding)
        , _NormalBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.31f))
        , _HoverBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.08f))
        , _FocusedBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.08f))
        , _ReadOnlyBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.18f))
        , _NormalBorderColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.69f))
        , _HoverBorderColor(FNexusModsStyle::NexusOrange)
        , _FocusedBorderColor(FNexusModsStyle::NexusOrange)
        , _ReadOnlyBorderColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f))
    {}
        SLATE_ATTRIBUTE(FText, Text)
        SLATE_ATTRIBUTE(FText, HintText)
        SLATE_ARGUMENT(bool, IsReadOnly)
        SLATE_ATTRIBUTE(bool, IsPassword)
        SLATE_ARGUMENT(bool, AllowMultiLine)
        SLATE_ARGUMENT(bool, AutoWrapText)
        SLATE_ARGUMENT(FMargin, Padding)

        SLATE_ARGUMENT(FLinearColor, NormalBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, HoverBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, FocusedBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, ReadOnlyBackgroundColor)

        SLATE_ARGUMENT(FLinearColor, NormalBorderColor)
        SLATE_ARGUMENT(FLinearColor, HoverBorderColor)
        SLATE_ARGUMENT(FLinearColor, FocusedBorderColor)
        SLATE_ARGUMENT(FLinearColor, ReadOnlyBorderColor)

        SLATE_EVENT(FOnTextChanged, OnTextChanged)
        SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    FText GetText() const;
    void SetText(const FText& InText);
    void RefreshDisplayText();
    void ScrollToEnd();

private:
    ENexusModsTextBoxState GetTextBoxState() const;
    FSlateColor GetTextBoxBackgroundColor() const;
    FSlateColor GetTextBoxBorderColor() const;
    FText GetDisplayText() const;
    bool IsProtectedTextHidden() const;
    bool IsTextBoxReadOnly() const;
    FString MakeMaskedText(const FString& InText) const;
    void OnInternalTextChanged(const FText& InText);
    void OnInternalTextCommitted(const FText& InText, ETextCommit::Type CommitType);
    void SetupTextBoxImageBrushes();

private:
    TSharedPtr<SMultiLineEditableTextBox> TextBox;

    FSlateBrush BorderBrush;
    FSlateBrush FillBrush;

    FText ActualText;
    FOnTextChanged OnTextChanged;
    FOnTextCommitted OnTextCommitted;

    bool bIsReadOnly = false;
    bool bAllowMultiLine = false;
    bool bAutoWrapText = false;
    bool bIsRefreshingDisplayText = false;
    FMargin Padding;
    TAttribute<bool> IsPassword;

    FLinearColor NormalBackgroundColor;
    FLinearColor HoverBackgroundColor;
    FLinearColor FocusedBackgroundColor;
    FLinearColor ReadOnlyBackgroundColor;

    FLinearColor NormalBorderColor;
    FLinearColor HoverBorderColor;
    FLinearColor FocusedBorderColor;
    FLinearColor ReadOnlyBorderColor;
};
