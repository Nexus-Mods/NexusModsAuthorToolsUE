#include "Widgets/Components/SNexusModsIconToggleButton.h"

#include "Widgets/Layout/SBox.h"

void SNexusModsIconToggleButton::Construct(const FArguments& InArgs) {
    IsOn = InArgs._IsOn;
    OnIconBrushName = InArgs._OnIconBrushName;
    OffIconBrushName = InArgs._OffIconBrushName;
    IconSize = InArgs._IconSize;
    Padding = InArgs._Padding;
    bTransparentBackground = InArgs._bTransparentBackground;
    HoverStyle = InArgs._HoverStyle;
    ToolTipText = InArgs._ToolTipText;
    OnClicked = InArgs._OnClicked;

    ChildSlot [
        SAssignNew(ButtonContainer, SBox) [
            MakeButton()
        ]
    ];
}

TSharedRef<SWidget> SNexusModsIconToggleButton::MakeButton() const {
    return
        SNew(SNexusModsButton)
            .IconBrushName(GetCurrentIconBrushName())
            .IconSize(IconSize)
            .Padding(Padding)
            .bTransparentBackground(bTransparentBackground)
            .HoverStyle(HoverStyle)
            .ToolTipText(ToolTipText)
            .OnClicked(const_cast<SNexusModsIconToggleButton*>(this), &SNexusModsIconToggleButton::OnButtonClicked);
}

FReply SNexusModsIconToggleButton::OnButtonClicked() {
    FReply Reply = FReply::Handled();

    if (OnClicked.IsBound()) {
        Reply = OnClicked.Execute();
    }

    RefreshButton();

    return Reply;
}

FName SNexusModsIconToggleButton::GetCurrentIconBrushName() const {
    return IsOn.Get(false) ? OnIconBrushName : OffIconBrushName;
}

void SNexusModsIconToggleButton::RefreshButton() {
    if (ButtonContainer.IsValid()) {
        ButtonContainer->SetContent(MakeButton());
    }
}
