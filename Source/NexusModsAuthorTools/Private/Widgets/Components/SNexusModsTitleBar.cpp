#include "Widgets/Components/SNexusModsTitleBar.h"

#include "InputCoreTypes.h"
#include "NexusModsStyle.h"
#include "NexusModsUECompatibility.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Components/SNexusModsButton.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformProcess.h"

void SNexusModsTitleBar::Construct(const FArguments& InArgs) {
    Window = InArgs._Window;
    OnBackRequested = InArgs._OnBackRequested;
    OnCloseRequested = InArgs._OnCloseRequested;

    ChildSlot [
        SNew(SBorder)
        .Padding(FNexusModsStyle::SectionHeaderPadding)
        .BorderImage(FNexusModsStyle::Get().GetBrush("NexusMods.TitleBarBackground")) [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FNexusModsStyle::ButtonGroupPadding) [
                SNew(SButton)
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
                    .ButtonStyle(NexusModsUECompatibility::GetEditorStyle(), "SimpleButton")
#else
                    .ButtonStyle(&FNexusModsStyle::Get().GetWidgetStyle<FButtonStyle>("NexusMods.Button.Transparent"))
#endif
                    .ContentPadding(0.0f)
                    .Cursor(EMouseCursor::Hand)
                    .ToolTipText(FText::FromString("Open Nexus Mods"))
                    .OnClicked(this, &SNexusModsTitleBar::OnNexusModsIconClicked) [
                        SNew(SImage).Image(FNexusModsStyle::Get().GetBrush("NexusMods.PluginAction.TitleBar"))
                    ]
            ]
            + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                SNew(STextBlock).Text(InArgs._Title).Font(NexusModsUECompatibility::GetEditorFontStyle("BoldFont"))
            ]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
                SNew(SNexusModsButton)
                .IconBrushName("NexusMods.Icon.NavigateBack")
                .bTransparentBackground(true)
                .HoverStyle(ENexusModsButtonHoverStyle::Both)
                .ToolTipText(FText::FromString("Back"))
                .Visibility(InArgs._BackButtonVisibility)
                .IsEnabled(InArgs._ActionsEnabled)
                .OnClicked(this, &SNexusModsTitleBar::OnBackClicked)
            ]

            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FNexusModsStyle::InlineControlPadding)[
                SNew(SNexusModsButton)
                .IconBrushName("NexusMods.Icon.ClosePanel")
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

FReply SNexusModsTitleBar::OnNexusModsIconClicked() {
    FPlatformProcess::LaunchURL(TEXT("https://www.nexusmods.com"), nullptr, nullptr);
    return FReply::Handled();
}

FReply SNexusModsTitleBar::OnBackClicked() {
    OnBackRequested.ExecuteIfBound();
    return FReply::Handled();
}

FReply SNexusModsTitleBar::OnCloseClicked() {
    OnCloseRequested.ExecuteIfBound();
    return FReply::Handled();
}
