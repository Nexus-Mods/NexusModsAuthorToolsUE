#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SNexusModsMarkdownViewer;
class SWindow;

/** Window content for displaying a Markdown file or Markdown text. */
class SNexusModsMarkdownWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SNexusModsMarkdownWindow)
		: _Title(FText::FromString(TEXT("Markdown")))
		, _MarkdownText(FText::GetEmpty())
		, _MarkdownFilePath(FString())
	{
	}

		SLATE_ARGUMENT(TWeakPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(FText, Title)
		SLATE_ARGUMENT(FText, MarkdownText)
		SLATE_ARGUMENT(FString, MarkdownFilePath)
		SLATE_EVENT(FSimpleDelegate, OnCloseRequested)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	void CloseWindow();

private:
	TWeakPtr<SWindow> ParentWindow;
	FSimpleDelegate OnCloseRequested;
	TSharedPtr<SNexusModsMarkdownViewer> MarkdownViewer;
};
