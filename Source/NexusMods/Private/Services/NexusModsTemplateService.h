#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphPin.h"

struct FAssetData;

enum class ENexusModsModTemplateType : uint8 {
    BuiltIn,
    ProjectFolder
};

struct FNexusModsModTemplate {
    FString Id;
    FText DisplayName;
    FText Description;
    FString SourcePath;
    FString TemplateImageAssetPath;
    FName IconBrushName = NAME_None;
    ENexusModsModTemplateType Type = ENexusModsModTemplateType::BuiltIn;

    bool IsBuiltIn() const { return Type == ENexusModsModTemplateType::BuiltIn; }

    FNexusModsModTemplate() = default;

    FNexusModsModTemplate(const FString& InId, const FText& InDisplayName, const FText& InDescription, ENexusModsModTemplateType InType)
        : Id(InId)
        , DisplayName(InDisplayName)
        , Description(InDescription)
        , Type(InType)
    {
    }
};

class FNexusModsTemplateService {

public:
    TArray<FNexusModsModTemplate> GetAvailableTemplates() const;
    bool CreateFromTemplate(const FNexusModsModTemplate& Template, const FString& ModName, const FString& DestinationPath, FString& OutError) const;

    FString SanitizeModName(const FString& ModName) const;
    FString GetDefaultDestinationPath(const FString& ModName) const;

private:
    void AddBuiltInTemplates(TArray<FNexusModsModTemplate>& Templates) const;
    void AddProjectFolderTemplates(TArray<FNexusModsModTemplate>& Templates) const;
    bool CreateUE4SSLogicMod(const FString& ModName, FString& OutError) const;
    bool CreateProjectFolderTemplate(const FNexusModsModTemplate& Template, const FString& ModName, const FString& DestinationPath, FString& OutError) const;
    class UBlueprint* CreateActorBlueprint(const FString& PackageName, const FName BlueprintName, FString& OutError) const;
    bool AddUE4SSVariables(class UBlueprint* Blueprint, FString& OutError) const;
    bool PrepareUE4SSEventGraph(class UBlueprint* Blueprint, FString& OutError) const;
    bool AddUE4SSCustomEvents(class UBlueprint* Blueprint, FString& OutError) const;
    bool AddBlueprintVariable(class UBlueprint* Blueprint, const FName VariableName, const FEdGraphPinType& PinType, FString& OutError) const;
    bool AddCustomEvent(class UBlueprint* Blueprint, class UEdGraph* EventGraph, const FName EventName, const TArray<TPair<FName, FEdGraphPinType>>& Parameters, int32 NodeIndex, FString& OutError) const;
    bool FinalizeAndSaveBlueprint(class UBlueprint* Blueprint, FString& OutError) const;
    FString GetTemplateImageAssetPath(const FString& TemplatePath) const;
    bool IsTemplateImageAsset(const FString& AssetPath) const;
    bool IsTemplateImageAsset(const FAssetData& AssetData) const;
    bool IsRootPrimaryAssetLabel(const FAssetData& AssetData, const FString& TemplatePath) const;
    bool DuplicateTemplateAssets(const FNexusModsModTemplate& Template, const FString& DestinationPath, const FString& ModName, FString& OutError) const;
    bool GetTemplateAssets(const FString& TemplatePath, TArray<FAssetData>& OutAssets, FString& OutError) const;
    FString GetTargetPackageNameForTemplateAsset(const FAssetData& SourceAssetData, const FString& TemplatePath, const FString& DestinationPath, const FString& ModName) const;
    void RemoveTemplateImageAssetsFromDestination(const FString& DestinationPath) const;
    bool SaveCopiedAssets(const TArray<UObject*>& CopiedAssets, FString& OutError) const;
    FEdGraphPinType MakeStringPinType() const;
    FEdGraphPinType MakeStringArrayPinType() const;
    FEdGraphPinType MakeIntegerPinType() const;
};
