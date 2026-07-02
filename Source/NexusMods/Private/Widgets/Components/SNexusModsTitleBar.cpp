#include "Widgets/Components/SNexusModsTitleBar.h"

#include "InputCoreTypes.h"
#include "NexusModsStyle.h"
#include "Styling/AppStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Components/SNexusModsButton.h"

void SNexusModsTitleBar::Construct(const FArguments& InArgs) {
    Window = InArgs._Window;
    OnBackRequested = InArgs._OnBackRequested;
    OnCloseRequested = InArgs._OnCloseRequested;

    ChildSlot [
        SNew(SBorder)
        .Padding(FNexusModsStyle::SectionHeaderPadding)
        .BorderImage(FAppStyle::GetBrush("Brushes.Recessed")) [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FNexusModsStyle::ButtonGroupPadding) [
                SNew(SImage).Image(FNexusModsStyle::Get().GetBrush("NexusMods.PluginAction"))
            ]
            + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                SNew(STextBlock).Text(InArgs._Title).Font(FAppStyle::GetFontStyle("BoldFont"))
            ]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
                SNew(SNexusModsButton)
                .IconBrushName("NexusMods.BackIcon")
                .bTransparentBackground(true)
                .HoverStyle(ENexusModsButtonHoverStyle::Both)
                .ToolTipText(FText::FromString("Back"))
                .Visibility(InArgs._BackButtonVisibility)
                .IsEnabled(InArgs._ActionsEnabled)
                .OnClicked(this, &SNexusModsTitleBar::OnBackClicked)
            ]

            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FNexusModsStyle::InlineControlPadding)[
                SNew(SNexusModsButton)
                .IconBrushName("NexusMods.CloseIcon")
                .bTransparentBackground(true)
                .HoverStyle(ENexusModsButtonHoverStyle::Both)
                .ToolTipText(FText::FromString("Close"))
                .Visibility(InArgs._CloseButtonVisibility)
                .IsEnabled(InArgs._ActionsEnabled)
                .OnClicked(this, &SNexusModsTitleBar::OnCloseClicked)
            ]
        ]
    ];
}

FReply SNexusModsTitleBar::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
        if (TSharedPtr<SWindow> PinnedWindow = Window.Pin()) {
            bDraggingWindow = true;
            DragStartMousePosition = MouseEvent.GetScreenSpacePosition();
            DragStartWindowPosition = PinnedWindow->GetPositionInScreen();

            return FReply::Handled().CaptureMouse(AsShared());
        }
    }

    return FReply::Unhandled();
}

FReply SNexusModsTitleBar::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bDraggingWindow) {
        bDraggingWindow = false;
        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}

FReply SNexusModsTitleBar::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
    if (bDraggingWindow && MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
        if (TSharedPtr<SWindow> PinnedWindow = Window.Pin()) {
            const FVector2D MouseDelta = MouseEvent.GetScreenSpacePosition() - DragStartMousePosition;
            PinnedWindow->MoveWindowTo(DragStartWindowPosition + MouseDelta);

            return FReply::Handled();
        }
    }

    return FReply::Unhandled();
}

FReply SNexusModsTitleBar::OnBackClicked() {
    OnBackRequested.ExecuteIfBound();
    return FReply::Handled();
}

FReply SNexusModsTitleBar::OnCloseClicked() {
    OnCloseRequested.ExecuteIfBound();
    return FReply::Handled();
}
