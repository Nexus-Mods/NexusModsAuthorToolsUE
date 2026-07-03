#include "Widgets/SNexusModsMarkdownWindow.h"

#include "NexusModsStyle.h"
#include "Styling/AppStyle.h"
#include "Widgets/Components/SNexusModsMarkdownViewer.h"
#include "Widgets/Components/SNexusModsTitleBar.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"

void SNexusModsMarkdownWindow::Construct(const FArguments& InArgs) {
	ParentWindow = InArgs._ParentWindow;
	OnCloseRequested = InArgs._OnCloseRequested;

	ChildSlot [
		SNew(SBorder)
		.Padding(0.0f)
		.BorderImage(FAppStyle::GetBrush("Brushes.Panel")) [
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight() [
				SNew(SNexusModsTitleBar)
				.Window(ParentWindow)
				.Title(InArgs._Title)
				.BackButtonVisibility(EVisibility::Collapsed)
				.CloseButtonVisibility(EVisibility::Visible)
				.ActionsEnabled(true)
				.OnCloseRequested(FSimpleDelegate::CreateSP(this, &SNexusModsMarkdownWindow::CloseWindow))
			]
			+ SVerticalBox::Slot().FillHeight(1.0f) [
				SAssignNew(MarkdownViewer, SNexusModsMarkdownViewer)
				.MarkdownText(InArgs._MarkdownText)
				.ContentPadding(FNexusModsStyle::ContentPadding)
			]
		]
	];

	if (!InArgs._MarkdownFilePath.IsEmpty() && MarkdownViewer.IsValid()) {
		MarkdownViewer->LoadMarkdownFile(InArgs._MarkdownFilePath);
	}
}

void SNexusModsMarkdownWindow::CloseWindow() {
	if (OnCloseRequested.IsBound()) {
		OnCloseRequested.Execute();
		return;
	}

	if (TSharedPtr<SWindow> PinnedWindow = ParentWindow.Pin()) {
		PinnedWindow->RequestDestroyWindow();
	}
}
