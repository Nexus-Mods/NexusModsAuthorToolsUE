#include "Widgets/Components/SNexusModsMarkdownViewer.h"

#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IPluginManager.h"
#include "NexusModsStyle.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SNexusModsMarkdownViewer"

namespace NexusModsMarkdownViewer {
    static const FLinearColor TextColor = FLinearColor(0.86f, 0.86f, 0.86f, 1.0f);
    static const FLinearColor MutedTextColor = FLinearColor(0.68f, 0.68f, 0.68f, 1.0f);
    static const FLinearColor HeadingColor = FNexusModsStyle::NexusOrange;
    static const FLinearColor RuleColor = FLinearColor(FNexusModsStyle::NexusOrange.R, FNexusModsStyle::NexusOrange.G, FNexusModsStyle::NexusOrange.B, 0.35f);
    static const FLinearColor CodeBackgroundColor = FLinearColor(0.02f, 0.02f, 0.02f, 0.35f);

    static bool IsRemoteImagePath(const FString& ImagePath) {
        return ImagePath.StartsWith(TEXT("http://"), ESearchCase::IgnoreCase) ||
            ImagePath.StartsWith(TEXT("https://"), ESearchCase::IgnoreCase);
    }

    static FString GetResourcesPath() {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("NexusModsAuthorTools"));
        if (!Plugin.IsValid()) return FString();
        return Plugin->GetBaseDir() / TEXT("Resources");
    }

    static FString ResolveLocalImagePath(const FString& ImagePath) {
        FString NormalizedPath = ImagePath;
        NormalizedPath.ReplaceInline(TEXT("\\"), TEXT("/"));

        if (FPaths::FileExists(NormalizedPath)) {
            return NormalizedPath;
        }

        const FString ResourcesPath = GetResourcesPath();
        if (ResourcesPath.IsEmpty()) return FString();

        FString RelativePath = NormalizedPath;
        if (RelativePath.StartsWith(TEXT("Resources/"), ESearchCase::IgnoreCase)) {
            RelativePath.RightChopInline(10, false);
        }

        const FString ResourceImagePath = ResourcesPath / RelativePath;
        if (FPaths::FileExists(ResourceImagePath)) {
            return ResourceImagePath;
        }

        return FString();
    }

    static EImageFormat GetImageFormatFromExtension(const FString& ImagePath) {
        const FString Extension = FPaths::GetExtension(ImagePath).ToLower();
        if (Extension == TEXT("jpg") || Extension == TEXT("jpeg")) return EImageFormat::JPEG;
        if (Extension == TEXT("bmp")) return EImageFormat::BMP;
        if (Extension == TEXT("ico")) return EImageFormat::ICO;
        if (Extension == TEXT("exr")) return EImageFormat::EXR;
        return EImageFormat::PNG;
    }

    static FVector2D GetImageDimensions(const FString& ImagePath, const FVector2D& FallbackSize) {
        TArray<uint8> ImageBytes;
        if (!FFileHelper::LoadFileToArray(ImageBytes, *ImagePath)) {
            return FallbackSize;
        }

        IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
        const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetImageFormatFromExtension(ImagePath));
        if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(ImageBytes.GetData(), ImageBytes.Num())) {
            return FallbackSize;
        }

        return FVector2D(static_cast<float>(ImageWrapper->GetWidth()), static_cast<float>(ImageWrapper->GetHeight()));
    }

    static FVector2D ScaleImageToFit(const FVector2D& ImageSize, const FVector2D& MaxImageSize) {
        if (ImageSize.X <= 0.0f || ImageSize.Y <= 0.0f) {
            return MaxImageSize;
        }

        const float Scale = FMath::Min(MaxImageSize.X / ImageSize.X, MaxImageSize.Y / ImageSize.Y);
        if (Scale >= 1.0f) {
            return ImageSize;
        }

        return FVector2D(ImageSize.X * Scale, ImageSize.Y * Scale);
    }
}

class SNexusModsMarkdownImage : public SCompoundWidget {
public:
    SLATE_BEGIN_ARGS(SNexusModsMarkdownImage)
        : _AltText(FString())
        , _ImageSource(FString())
        , _MaxImageSize(FVector2D(760.0f, 420.0f))
    {
    }

        SLATE_ARGUMENT(FString, AltText)
        SLATE_ARGUMENT(FString, ImageSource)
        SLATE_ARGUMENT(FVector2D, MaxImageSize)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs) {
        AltText = InArgs._AltText;
        ImageSource = InArgs._ImageSource;
        MaxImageSize = InArgs._MaxImageSize;

        ChildSlot [
            SAssignNew(ContentBox, SBox) [
                BuildLoadingWidget()
            ]
        ];

        LoadImage();
    }

private:
    void LoadImage() {
        using namespace NexusModsMarkdownViewer;

        if (IsRemoteImagePath(ImageSource)) {
            LoadRemoteImage();
            return;
        }

        const FString LocalImagePath = ResolveLocalImagePath(ImageSource);
        if (LocalImagePath.IsEmpty()) {
            SetStatusText(FString::Printf(TEXT("Image not found: %s"), *ImageSource));
            return;
        }

        SetLocalImage(LocalImagePath);
    }

    void LoadRemoteImage() {
        const FString Extension = FPaths::GetExtension(ImageSource).ToLower();
        const FString SafeExtension = Extension.IsEmpty() ? TEXT("png") : Extension;
        const FString DownloadPath = FPaths::ProjectIntermediateDir() / TEXT("NexusModsAuthorTools/MarkdownImages") /
            FString::Printf(TEXT("%s.%s"), *FGuid::NewGuid().ToString(EGuidFormats::Digits), *SafeExtension);

        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
        Request->SetURL(ImageSource);
        Request->SetVerb(TEXT("GET"));
        Request->OnProcessRequestComplete().BindSP(this, &SNexusModsMarkdownImage::OnRemoteImageLoaded, DownloadPath);
        Request->ProcessRequest();
    }

    void OnRemoteImageLoaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded, FString DownloadPath) {
        if (!bSucceeded || !Response.IsValid() || Response->GetResponseCode() < 200 || Response->GetResponseCode() >= 300) {
            SetStatusText(FString::Printf(TEXT("Unable to load image: %s"), *ImageSource));
            return;
        }

        IFileManager::Get().MakeDirectory(*FPaths::GetPath(DownloadPath), true);
        if (!FFileHelper::SaveArrayToFile(Response->GetContent(), *DownloadPath)) {
            SetStatusText(FString::Printf(TEXT("Unable to save image: %s"), *ImageSource));
            return;
        }

        SetLocalImage(DownloadPath);
    }

    void SetLocalImage(const FString& LocalImagePath) {
        using namespace NexusModsMarkdownViewer;

        const FVector2D ImageSize = ScaleImageToFit(GetImageDimensions(LocalImagePath, MaxImageSize), MaxImageSize);
        ImageBrush = MakeShared<FSlateDynamicImageBrush>(FName(*LocalImagePath), ImageSize);

        if (ContentBox.IsValid()) {
            ContentBox->SetContent(
                SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("Brushes.Recessed"))
                .Padding(6.0f) [
                    SNew(SImage)
                    .Image(ImageBrush.Get())
                ]
            );
        }
    }

    TSharedRef<SWidget> BuildLoadingWidget() const {
        return SNew(STextBlock)
            .Text(FText::FromString(AltText.IsEmpty() ? FString::Printf(TEXT("Loading image: %s"), *ImageSource) : AltText))
            .Font(FAppStyle::GetFontStyle("NormalFont"))
            .ColorAndOpacity(NexusModsMarkdownViewer::MutedTextColor);
    }

    void SetStatusText(const FString& StatusText) {
        if (ContentBox.IsValid()) {
            ContentBox->SetContent(
                SNew(STextBlock)
                .Text(FText::FromString(StatusText))
                .Font(FAppStyle::GetFontStyle("NormalFont"))
                .ColorAndOpacity(NexusModsMarkdownViewer::MutedTextColor)
            );
        }
    }

private:
    FString AltText;
    FString ImageSource;
    FVector2D MaxImageSize;

    TSharedPtr<SBox> ContentBox;
    TSharedPtr<FSlateDynamicImageBrush> ImageBrush;
};




void SNexusModsMarkdownViewer::Construct(const FArguments& InArgs) {
    MarkdownText = InArgs._MarkdownText;
    ContentPadding = InArgs._ContentPadding;
    WrapTextAt = InArgs._WrapTextAt;
    MaxImageSize = InArgs._MaxImageSize;

    ChildSlot [
        SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")) [
            SNew(SScrollBox)
            .ScrollBarStyle(&FNexusModsStyle::Get().GetWidgetStyle<FScrollBarStyle>("NexusMods.ScrollBar"))
            + SScrollBox::Slot().Padding(ContentPadding) [
                SAssignNew(MarkdownContainer, SVerticalBox)
            ]
        ]
    ];

    RebuildMarkdownContent();
}

void SNexusModsMarkdownViewer::SetMarkdownText(const FText& InMarkdownText) {
    MarkdownText = InMarkdownText;
    RebuildMarkdownContent();
}

bool SNexusModsMarkdownViewer::LoadMarkdownFile(const FString& FilePath) {
    FString LoadedText;
    if (!FFileHelper::LoadFileToString(LoadedText, *FilePath)) {
        return false;
    }
    SetMarkdownText(FText::FromString(LoadedText));
    return true;
}

void SNexusModsMarkdownViewer::RebuildMarkdownContent() {
    if (!MarkdownContainer.IsValid()) return;

    MarkdownContainer->ClearChildren();

    TArray<FNexusModsMarkdownLine> Lines;
    ParseMarkdown(MarkdownText.ToString(), Lines);

    for (const FNexusModsMarkdownLine& Line : Lines) {
        AddRenderedLine(Line);
    }
}

void SNexusModsMarkdownViewer::ParseMarkdown(const FString& Markdown, TArray<FNexusModsMarkdownLine>& OutLines) const {
    TArray<FString> RawLines;
    Markdown.ParseIntoArrayLines(RawLines, false);

    bool bInCodeBlock = false;
    FString CodeBlock;
    FString Paragraph;

    for (const FString& RawLine : RawLines) {
        const FString TrimmedLine = RawLine.TrimStartAndEnd();

        if (TrimmedLine.StartsWith(TEXT("```"))) {
            FlushParagraph(Paragraph, OutLines);
            if (bInCodeBlock) {
                OutLines.Add(FNexusModsMarkdownLine(ENexusModsMarkdownLineType::Code, CodeBlock.TrimEnd()));
                CodeBlock.Empty();
                bInCodeBlock = false;
            } else {
                bInCodeBlock = true;
            }
            continue;
        }

        if (bInCodeBlock) {
            CodeBlock += RawLine + LINE_TERMINATOR;
            continue;
        }

        if (TrimmedLine.IsEmpty()) {
            FlushParagraph(Paragraph, OutLines);
            continue;
        }

        ENexusModsMarkdownLineType HeadingType;
        FString HeadingText;
        if (TryParseHeading(TrimmedLine, HeadingType, HeadingText)) {
            FlushParagraph(Paragraph, OutLines);
            OutLines.Add(FNexusModsMarkdownLine(HeadingType, HeadingText));
            continue;
        }

        FString AltText;
        FString ImageSource;
        if (TryParseImage(TrimmedLine, AltText, ImageSource)) {
            FlushParagraph(Paragraph, OutLines);
            OutLines.Add(FNexusModsMarkdownLine(ENexusModsMarkdownLineType::Image, AltText, ImageSource));
            continue;
        }

        if (IsHorizontalRule(TrimmedLine)) {
            FlushParagraph(Paragraph, OutLines);
            OutLines.Add(FNexusModsMarkdownLine(ENexusModsMarkdownLineType::HorizontalRule, FString()));
            continue;
        }

        FString BulletText;
        if (TryParseUnorderedBullet(TrimmedLine, BulletText)) {
            FlushParagraph(Paragraph, OutLines);
            OutLines.Add(FNexusModsMarkdownLine(ENexusModsMarkdownLineType::Bullet, BulletText));
            continue;
        }

        int32 ListIndex = INDEX_NONE;
        FString NumberedText;
        if (TryParseNumberedBullet(TrimmedLine, ListIndex, NumberedText)) {
            FlushParagraph(Paragraph, OutLines);
            OutLines.Add(FNexusModsMarkdownLine(ENexusModsMarkdownLineType::Numbered, NumberedText, ListIndex));
            continue;
        }

        if (!Paragraph.IsEmpty()) {
            Paragraph += TEXT(" ");
        }

        Paragraph += TrimmedLine;
    }

    FlushParagraph(Paragraph, OutLines);

    if (bInCodeBlock && !CodeBlock.IsEmpty()) {
        OutLines.Add(FNexusModsMarkdownLine(ENexusModsMarkdownLineType::Code, CodeBlock.TrimEnd()));
    }
}

void SNexusModsMarkdownViewer::FlushParagraph(FString& Paragraph, TArray<FNexusModsMarkdownLine>& OutLines) const {
    Paragraph = Paragraph.TrimStartAndEnd();
    if (!Paragraph.IsEmpty()) {
        OutLines.Add(FNexusModsMarkdownLine(ENexusModsMarkdownLineType::Paragraph, Paragraph));
        Paragraph.Empty();
    }
}

void SNexusModsMarkdownViewer::AddRenderedLine(const FNexusModsMarkdownLine& Line) {
    using namespace NexusModsMarkdownViewer;

    switch (Line.Type) {
    case ENexusModsMarkdownLineType::Heading1:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 8.0f, 0.0f, 8.0f) [
            BuildParagraphWidget(Line.Text, GetBoldFont(22.0f), HeadingColor)
        ];
        break;

    case ENexusModsMarkdownLineType::Heading2:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 7.0f, 0.0f, 6.0f) [
            BuildParagraphWidget(Line.Text, GetBoldFont(18.0f), HeadingColor)
        ];
        break;

    case ENexusModsMarkdownLineType::Heading3:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 6.0f, 0.0f, 5.0f) [
            BuildParagraphWidget(Line.Text, GetBoldFont(15.0f), HeadingColor)
        ];
        break;

    case ENexusModsMarkdownLineType::Bullet:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 2.0f) [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().Padding(8.0f, 0.0f, 8.0f, 0.0f) [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("*")))
                .Font(GetBoldFont(12.0f))
                .ColorAndOpacity(HeadingColor)
            ]
            + SHorizontalBox::Slot().FillWidth(1.0f) [
                BuildInlineTextWidget(Line.Text, GetRegularFont(10.0f), TextColor)
            ]
        ];
        break;

    case ENexusModsMarkdownLineType::Numbered:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 2.0f) [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().Padding(8.0f, 0.0f, 8.0f, 0.0f) [
                SNew(STextBlock)
                .Text(FText::FromString(FString::Printf(TEXT("%d."), Line.ListIndex)))
                .Font(GetBoldFont(10.0f))
                .ColorAndOpacity(HeadingColor)
            ]
            + SHorizontalBox::Slot().FillWidth(1.0f) [
                BuildInlineTextWidget(Line.Text, GetRegularFont(10.0f), TextColor)
            ]
        ];
        break;

    case ENexusModsMarkdownLineType::HorizontalRule:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 10.0f) [
            BuildHorizontalRuleWidget()
        ];
        break;

    case ENexusModsMarkdownLineType::Code:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 6.0f) [
            BuildCodeBlockWidget(Line.Text)
        ];
        break;

    case ENexusModsMarkdownLineType::Image:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 8.0f) [
            BuildImageWidget(Line.Text, Line.ImageSource)
        ];
        break;

    case ENexusModsMarkdownLineType::Paragraph:
    default:
        MarkdownContainer->AddSlot().AutoHeight().Padding(0.0f, 3.0f) [
            BuildParagraphWidget(Line.Text, GetRegularFont(10.0f), TextColor)
        ];
        break;
    }
}

TSharedRef<SWidget> SNexusModsMarkdownViewer::BuildParagraphWidget(const FString& Text, const FSlateFontInfo& Font, const FLinearColor& Color) const {
    return SNew(SBox) [
        BuildInlineTextWidget(Text, Font, Color)
    ];
}

TSharedRef<SWidget> SNexusModsMarkdownViewer::BuildInlineTextWidget(const FString& Text, const FSlateFontInfo& Font, const FLinearColor& Color) const {
    TSharedRef<SWrapBox> WrapBox = SNew(SWrapBox).UseAllottedSize(true);

    int32 CurrentIndex = 0;
    while (CurrentIndex < Text.Len()) {
        const int32 LinkStart = Text.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, CurrentIndex);
        const int32 LabelEnd = LinkStart != INDEX_NONE ? Text.Find(TEXT("]("), ESearchCase::CaseSensitive, ESearchDir::FromStart, LinkStart) : INDEX_NONE;
        const int32 UrlEnd = LabelEnd != INDEX_NONE ? Text.Find(TEXT(")"), ESearchCase::CaseSensitive, ESearchDir::FromStart, LabelEnd + 2) : INDEX_NONE;

        if (LinkStart == INDEX_NONE || LabelEnd == INDEX_NONE || UrlEnd == INDEX_NONE) {
            WrapBox->AddSlot() [
                SNew(STextBlock)
                .Text(FText::FromString(Text.Mid(CurrentIndex)))
                .Font(Font)
                .ColorAndOpacity(Color)
                .WrapTextAt(WrapTextAt)
            ];
            break;
        }

        if (LinkStart > CurrentIndex) {
            WrapBox->AddSlot() [
                SNew(STextBlock)
                .Text(FText::FromString(Text.Mid(CurrentIndex, LinkStart - CurrentIndex)))
                .Font(Font)
                .ColorAndOpacity(Color)
                .WrapTextAt(WrapTextAt)
            ];
        }

        const FString Label = Text.Mid(LinkStart + 1, LabelEnd - LinkStart - 1);
        const FString Url = Text.Mid(LabelEnd + 2, UrlEnd - LabelEnd - 2);

        WrapBox->AddSlot() [
            SNew(SHyperlink)
            .Text(FText::FromString(Label))
            .OnNavigate_Lambda([Url]() {
                FPlatformProcess::LaunchURL(*Url, nullptr, nullptr);
            })
        ];

        CurrentIndex = UrlEnd + 1;
    }

    return WrapBox;
}

TSharedRef<SWidget> SNexusModsMarkdownViewer::BuildCodeBlockWidget(const FString& Text) const {
    using namespace NexusModsMarkdownViewer;

    return SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("Brushes.Recessed"))
        .BorderBackgroundColor(CodeBackgroundColor)
        .Padding(10.0f) [
            SNew(STextBlock)
            .Text(FText::FromString(Text))
            .Font(FCoreStyle::GetDefaultFontStyle("Mono", 9.0f))
            .ColorAndOpacity(MutedTextColor)
            .AutoWrapText(true)
        ];
}

TSharedRef<SWidget> SNexusModsMarkdownViewer::BuildHorizontalRuleWidget() const {
    using namespace NexusModsMarkdownViewer;

    return SNew(SBox)
        .HeightOverride(1.0f) [
            SNew(SBorder)
            .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
            .BorderBackgroundColor(RuleColor)
        ];
}

TSharedRef<SWidget> SNexusModsMarkdownViewer::BuildImageWidget(const FString& AltText, const FString& ImageSource) const {
    return SNew(SNexusModsMarkdownImage)
        .AltText(AltText)
        .ImageSource(ImageSource)
        .MaxImageSize(MaxImageSize);
}

bool SNexusModsMarkdownViewer::IsHorizontalRule(const FString& TrimmedLine) const {
    if (TrimmedLine.Len() < 3) return false;

    const TCHAR FirstCharacter = TrimmedLine[0];
    if (FirstCharacter != TEXT('-') && FirstCharacter != TEXT('*') && FirstCharacter != TEXT('_')) {
        return false;
    }

    for (const TCHAR Character : TrimmedLine) {
        if (Character != FirstCharacter && Character != TEXT(' ')) {
            return false;
        }
    }

    return true;
}

bool SNexusModsMarkdownViewer::TryParseHeading(const FString& TrimmedLine, ENexusModsMarkdownLineType& OutType, FString& OutText) const {
    if (TrimmedLine.StartsWith(TEXT("### "))) {
        OutType = ENexusModsMarkdownLineType::Heading3;
        OutText = TrimmedLine.Mid(4).TrimStartAndEnd();
        return true;
    }

    if (TrimmedLine.StartsWith(TEXT("## "))) {
        OutType = ENexusModsMarkdownLineType::Heading2;
        OutText = TrimmedLine.Mid(3).TrimStartAndEnd();
        return true;
    }

    if (TrimmedLine.StartsWith(TEXT("# "))) {
        OutType = ENexusModsMarkdownLineType::Heading1;
        OutText = TrimmedLine.Mid(2).TrimStartAndEnd();
        return true;
    }

    return false;
}

bool SNexusModsMarkdownViewer::TryParseImage(const FString& TrimmedLine, FString& OutAltText, FString& OutImageSource) const {
    if (!TrimmedLine.StartsWith(TEXT("!["))) {
        return false;
    }

    const int32 LabelEnd = TrimmedLine.Find(TEXT("]("), ESearchCase::CaseSensitive, ESearchDir::FromStart, 2);
    const int32 UrlEnd = LabelEnd != INDEX_NONE ? TrimmedLine.Find(TEXT(")"), ESearchCase::CaseSensitive, ESearchDir::FromStart, LabelEnd + 2) : INDEX_NONE;

    if (LabelEnd == INDEX_NONE || UrlEnd == INDEX_NONE) {
        return false;
    }

    OutAltText = TrimmedLine.Mid(2, LabelEnd - 2).TrimStartAndEnd();
    OutImageSource = TrimmedLine.Mid(LabelEnd + 2, UrlEnd - LabelEnd - 2).TrimStartAndEnd();
    return !OutImageSource.IsEmpty();
}

bool SNexusModsMarkdownViewer::TryParseUnorderedBullet(const FString& TrimmedLine, FString& OutText) const {
    if (TrimmedLine.Len() < 3) return false;

    const TCHAR Marker = TrimmedLine[0];
    if ((Marker == TEXT('-') || Marker == TEXT('*') || Marker == TEXT('+')) && FChar::IsWhitespace(TrimmedLine[1])) {
        OutText = TrimmedLine.Mid(2).TrimStartAndEnd();
        return true;
    }

    return false;
}

bool SNexusModsMarkdownViewer::TryParseNumberedBullet(const FString& TrimmedLine, int32& OutIndex, FString& OutText) const {
    int32 DotIndex = INDEX_NONE;
    if (!TrimmedLine.FindChar(TEXT('.'), DotIndex) || DotIndex <= 0 || DotIndex >= TrimmedLine.Len() - 1) {
        return false;
    }

    const FString NumberText = TrimmedLine.Left(DotIndex);
    if (!NumberText.IsNumeric() || !FChar::IsWhitespace(TrimmedLine[DotIndex + 1])) {
        return false;
    }

    OutIndex = FCString::Atoi(*NumberText);
    OutText = TrimmedLine.Mid(DotIndex + 2).TrimStartAndEnd();
    return true;
}

FSlateFontInfo SNexusModsMarkdownViewer::GetRegularFont(float Size) const {
    FSlateFontInfo Font = FAppStyle::GetFontStyle("NormalFont");
    Font.Size = Size;
    return Font;
}

FSlateFontInfo SNexusModsMarkdownViewer::GetBoldFont(float Size) const {
    FSlateFontInfo Font = FAppStyle::GetFontStyle("NormalFontBold");
    Font.Size = Size;
    return Font;
}

#undef LOCTEXT_NAMESPACE
