#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SVerticalBox;

/**
 * Lightweight Slate Markdown viewer for Nexus Mods Author Tools.
 *
 * Supported Markdown:
 * - Headings: #, ##, ###
 * - Bullet lists: -, *, +
 * - Numbered lists: 1., 2., etc.
 * - Horizontal rules: ---, ***, ___
 * - Inline links: [label](https://example.com)
 * - Images: ![alt](Resources/Path.png), ![alt](Path.png), or ![alt](https://example.com/image.png)
 * - Fenced code blocks: ```
 * - Paragraphs and simple line wrapping
 *
 * This is intentionally small and dependency-free. It is not a complete Markdown parser.
 */
class SNexusModsMarkdownViewer : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SNexusModsMarkdownViewer)
        : _MarkdownText(FText::GetEmpty())
        , _ContentPadding(FMargin(16.0f))
        , _WrapTextAt(900.0f)
        , _MaxImageSize(FVector2D(760.0f, 420.0f))
    {
    }

        SLATE_ARGUMENT(FText, MarkdownText)
        SLATE_ARGUMENT(FMargin, ContentPadding)
        SLATE_ARGUMENT(float, WrapTextAt)
        SLATE_ARGUMENT(FVector2D, MaxImageSize)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    /** Replace the current Markdown text and rebuild the rendered content. */
    void SetMarkdownText(const FText& InMarkdownText);

    /** Convenience helper for loading a Markdown file directly from disk. */
    bool LoadMarkdownFile(const FString& FilePath);

private:
    enum class ENexusModsMarkdownLineType : uint8
    {
        Paragraph,
        Heading1,
        Heading2,
        Heading3,
        Bullet,
        Numbered,
        HorizontalRule,
        Code,
        Image
    };

    struct FNexusModsMarkdownLine
    {
        FNexusModsMarkdownLine()
            : Type(ENexusModsMarkdownLineType::Paragraph)
            , Text()
            , ImageSource()
            , ListIndex(INDEX_NONE)
        {
        }

        FNexusModsMarkdownLine(ENexusModsMarkdownLineType InType, const FString& InText, int32 InListIndex = INDEX_NONE)
            : Type(InType)
            , Text(InText)
            , ImageSource()
            , ListIndex(InListIndex)
        {
        }

        FNexusModsMarkdownLine(ENexusModsMarkdownLineType InType, const FString& InText, const FString& InImageSource)
            : Type(InType)
            , Text(InText)
            , ImageSource(InImageSource)
            , ListIndex(INDEX_NONE)
        {
        }

        ENexusModsMarkdownLineType Type;
        FString Text;
        FString ImageSource;
        int32 ListIndex;
    };

    void RebuildMarkdownContent();
    void ParseMarkdown(const FString& Markdown, TArray<FNexusModsMarkdownLine>& OutLines) const;
    void FlushParagraph(FString& Paragraph, TArray<FNexusModsMarkdownLine>& OutLines) const;
    void AddRenderedLine(const FNexusModsMarkdownLine& Line);

    TSharedRef<SWidget> BuildParagraphWidget(const FString& Text, const FSlateFontInfo& Font, const FLinearColor& Color) const;
    TSharedRef<SWidget> BuildInlineTextWidget(const FString& Text, const FSlateFontInfo& Font, const FLinearColor& Color) const;
    TSharedRef<SWidget> BuildCodeBlockWidget(const FString& Text) const;
    TSharedRef<SWidget> BuildHorizontalRuleWidget() const;
    TSharedRef<SWidget> BuildImageWidget(const FString& AltText, const FString& ImageSource) const;

    bool IsHorizontalRule(const FString& TrimmedLine) const;
    bool TryParseHeading(const FString& TrimmedLine, ENexusModsMarkdownLineType& OutType, FString& OutText) const;
    bool TryParseImage(const FString& TrimmedLine, FString& OutAltText, FString& OutImageSource) const;
    bool TryParseUnorderedBullet(const FString& TrimmedLine, FString& OutText) const;
    bool TryParseNumberedBullet(const FString& TrimmedLine, int32& OutIndex, FString& OutText) const;

    FSlateFontInfo GetRegularFont(float Size) const;
    FSlateFontInfo GetBoldFont(float Size) const;

private:
    FText MarkdownText;
    FMargin ContentPadding;
    float WrapTextAt;
    FVector2D MaxImageSize;

    TSharedPtr<SVerticalBox> MarkdownContainer;
};
