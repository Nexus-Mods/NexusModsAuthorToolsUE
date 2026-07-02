#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SWindow;

class SNexusModsTitleBar : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsTitleBar) {}
        SLATE_ARGUMENT(TWeakPtr<SWindow>, Window)
        SLATE_ATTRIBUTE(FText, Title)
        SLATE_ATTRIBUTE(EVisibility, BackButtonVisibility)
        SLATE_ATTRIBUTE(EVisibility, CloseButtonVisibility)
        SLATE_ATTRIBUTE(bool, ActionsEnabled)
        SLATE_EVENT(FSimpleDelegate, OnBackRequested)
        SLATE_EVENT(FSimpleDelegate, OnCloseRequested)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    FReply OnBackClicked();
    FReply OnCloseClicked();

private:
    TWeakPtr<SWindow> Window;
    FSimpleDelegate OnBackRequested;
    FSimpleDelegate OnCloseRequested;

    bool bDraggingWindow = false;
    FVector2D DragStartMousePosition;
    FVector2D DragStartWindowPosition;
};
