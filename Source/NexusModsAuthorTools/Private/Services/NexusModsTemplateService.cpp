#include "Services/NexusModsTemplateService.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/PrimaryAssetLabel.h"
#include "Engine/World.h"
#include "EditorAssetLibrary.h"
#include "GameFramework/Actor.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_Event.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "IAssetTools.h"
#include "Logging/TokenizedMessage.h"
#include "NexusModsUECompatibility.h"


namespace {

	FString GetAssetObjectPathString(const FAssetData& AssetData) {
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 1)
		return AssetData.GetObjectPathString();
#else
		return AssetData.ObjectPath.ToString();
#endif
	}

	bool SavePackageForCurrentEngine(UPackage* Package, UObject* Asset, const FString& PackageFilename) {
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;

		return UPackage::SavePackage(
			Package,
			Asset,
			*PackageFilename,
			SaveArgs
		);
#else
		return UPackage::SavePackage(
			Package,
			Asset,
			RF_Public | RF_Standalone,
			*PackageFilename,
			GError,
			nullptr,
			false,
			true,
			SAVE_NoError
		);
#endif
	}
}

TArray<FNexusModsModTemplate> FNexusModsTemplateService::GetAvailableTemplates() const {
    TArray<FNexusModsModTemplate> Templates;

    AddBuiltInTemplates(Templates);
    AddProjectFolderTemplates(Templates);

    return Templates;
}

void FNexusModsTemplateService::AddBuiltInTemplates(TArray<FNexusModsModTemplate>& Templates) const {
    FNexusModsModTemplate Template(
        TEXT("UE4SSLogicMod"),
        FText::FromString(TEXT("UE4SS Logic Mod")),
        FText::FromString(TEXT("Creates a UE4SS-compatible ModActor blueprint with the standard variables and custom events required by logic mods.")),
        ENexusModsModTemplateType::BuiltIn
    );

    Template.IconBrushName = TEXT("NexusMods.Icon.BuiltInTemplate");
    Templates.Add(Template);
}

void FNexusModsTemplateService::AddProjectFolderTemplates(TArray<FNexusModsModTemplate>& Templates) const {
    static const FString TemplateRootPath = TEXT("/Game/ModTemplates");

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    TArray<FString> TemplatePaths;
    AssetRegistry.GetSubPaths(TemplateRootPath, TemplatePaths, false);
    TemplatePaths.Sort();

    for (const FString& TemplatePath : TemplatePaths) {
        TArray<FAssetData> TemplateAssets;

        FARFilter TemplateFilter;
        TemplateFilter.PackagePaths.Add(*TemplatePath);
        TemplateFilter.bRecursivePaths = true;
        AssetRegistry.GetAssets(TemplateFilter, TemplateAssets);

        if (TemplateAssets.Num() == 0) {
            continue;
        }

        const FString TemplateName = FPackageName::GetLongPackageAssetName(TemplatePath);

        FNexusModsModTemplate Template(
            TemplatePath,
            FText::FromString(TemplateName),
            FText::FromString(FString::Printf(TEXT("Copies the project template from %s into a new mod folder."), *TemplatePath)),
            ENexusModsModTemplateType::ProjectFolder
        );

        Template.SourcePath = TemplatePath;
        Template.TemplateImageAssetPath = GetTemplateImageAssetPath(TemplatePath);
        Template.IconBrushName = TEXT("NexusMods.Icon.ProjectFolderTemplate");
        Templates.Add(Template);
    }
}

bool FNexusModsTemplateService::CreateFromTemplate(const FNexusModsModTemplate& Template, const FString& ModName, const FString& DestinationPath, FString& OutError) const {
    const FString SanitizedModName = SanitizeModName(ModName);

    if (SanitizedModName.IsEmpty()) {
        OutError = TEXT("Please enter a valid mod name.");
        return false;
    }

    if (Template.Type == ENexusModsModTemplateType::BuiltIn) {
        if (Template.Id == TEXT("UE4SSLogicMod")) {
            return CreateUE4SSLogicMod(SanitizedModName, OutError);
        }

        OutError = TEXT("Unsupported built-in template selected.");
        return false;
    }

    return CreateProjectFolderTemplate(Template, SanitizedModName, DestinationPath, OutError);
}

FString FNexusModsTemplateService::SanitizeModName(const FString& ModName) const {
    FString TrimmedName = ModName;
    TrimmedName.TrimStartAndEndInline();

    FString SanitizedName;
    SanitizedName.Reserve(TrimmedName.Len());

    for (int32 CharacterIndex = 0; CharacterIndex < TrimmedName.Len(); ++CharacterIndex) {
        const TCHAR Character = TrimmedName[CharacterIndex];

        if (FChar::IsAlnum(Character) || Character == TEXT('_')) {
            SanitizedName.AppendChar(Character);
        }
        else if (FChar::IsWhitespace(Character) || Character == TEXT('-')) {
            SanitizedName.AppendChar(TEXT('_'));
        }
    }

    while (SanitizedName.Contains(TEXT("__"))) {
        SanitizedName.ReplaceInline(TEXT("__"), TEXT("_"));
    }

    SanitizedName.RemoveFromStart(TEXT("_"));
    SanitizedName.RemoveFromEnd(TEXT("_"));

    return SanitizedName;
}

FString FNexusModsTemplateService::GetDefaultDestinationPath(const FString& ModName) const {
    const FString SanitizedModName = SanitizeModName(ModName);

    if (SanitizedModName.IsEmpty()) {
        return TEXT("/Game/Mods/{NewModName}");
    }

    return FString::Printf(TEXT("/Game/Mods/%s"), *SanitizedModName);
}

bool FNexusModsTemplateService::CreateUE4SSLogicMod(const FString& ModName, FString& OutError) const {
    const FString PackagePath = FString::Printf(TEXT("/Game/Mods/%s"), *ModName);
    const FString PackageName = PackagePath / TEXT("ModActor");
    const FName BlueprintName(TEXT("ModActor"));

    if (!FPackageName::IsValidLongPackageName(PackageName, true)) {
        OutError = FString::Printf(TEXT("The destination path is not valid: %s"), *PackageName);
        return false;
    }

    if (FPackageName::DoesPackageExist(PackageName)) {
        OutError = FString::Printf(TEXT("A ModActor already exists at %s."), *PackageName);
        return false;
    }

    UBlueprint* Blueprint = CreateActorBlueprint(PackageName, BlueprintName, OutError);
    if (!Blueprint) {
        return false;
    }

    if (!AddUE4SSVariables(Blueprint, OutError)) return false;
    if (!PrepareUE4SSEventGraph(Blueprint, OutError)) return false;
    if (!AddUE4SSCustomEvents(Blueprint, OutError)) return false;
    if (!FinalizeAndSaveBlueprint(Blueprint, OutError)) return false;

    OutError.Reset();
    return true;
}


bool FNexusModsTemplateService::CreateProjectFolderTemplate(const FNexusModsModTemplate& Template, const FString& ModName, const FString& DestinationPath, FString& OutError) const {
    if (Template.SourcePath.IsEmpty()) {
        OutError = TEXT("The selected project template does not have a valid source folder.");
        return false;
    }

    if (!UEditorAssetLibrary::DoesDirectoryExist(Template.SourcePath)) {
        OutError = FString::Printf(TEXT("The template folder does not exist: %s"), *Template.SourcePath);
        return false;
    }

    FString CleanDestinationPath = DestinationPath;
    CleanDestinationPath.TrimStartAndEndInline();
    CleanDestinationPath.RemoveFromEnd(TEXT("/"));

    if (CleanDestinationPath.IsEmpty()) {
        CleanDestinationPath = GetDefaultDestinationPath(ModName);
    }

    if (!CleanDestinationPath.StartsWith(TEXT("/Game/"))) {
        OutError = TEXT("The destination path must be inside /Game.");
        return false;
    }

    if (!FPackageName::IsValidLongPackageName(CleanDestinationPath, true)) {
        OutError = FString::Printf(TEXT("The destination path is not valid: %s"), *CleanDestinationPath);
        return false;
    }

    if (UEditorAssetLibrary::DoesDirectoryExist(CleanDestinationPath)) {
        OutError = FString::Printf(TEXT("The destination folder already exists: %s"), *CleanDestinationPath);
        return false;
    }

    if (!DuplicateTemplateAssets(Template, CleanDestinationPath, ModName, OutError)) {
        return false;
    }

    TArray<FString> PathsToScan;
    PathsToScan.Add(CleanDestinationPath);

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    AssetRegistryModule.Get().ScanPathsSynchronous(PathsToScan, true);

    OutError.Reset();
    return true;
}

UBlueprint* FNexusModsTemplateService::CreateActorBlueprint(const FString& PackageName, const FName BlueprintName, FString& OutError) const {
    UPackage* Package = CreatePackage(*PackageName);
    if (!Package) {
        OutError = FString::Printf(TEXT("Unable to create package: %s"), *PackageName);
        return nullptr;
    }

    UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(
        AActor::StaticClass(),
        Package,
        BlueprintName,
        BPTYPE_Normal,
        UBlueprint::StaticClass(),
        UBlueprintGeneratedClass::StaticClass(),
        NAME_None
    );

    if (!Blueprint) {
        OutError = TEXT("Unable to create the ModActor blueprint.");
        return nullptr;
    }

    FAssetRegistryModule::AssetCreated(Blueprint);
    return Blueprint;
}

bool FNexusModsTemplateService::AddUE4SSVariables(UBlueprint* Blueprint, FString& OutError) const {
    const FEdGraphPinType StringPinType = MakeStringPinType();
    const FEdGraphPinType StringArrayPinType = MakeStringArrayPinType();

    if (!AddBlueprintVariable(Blueprint, TEXT("ModAuthor"), StringPinType, OutError)) return false;
    if (!AddBlueprintVariable(Blueprint, TEXT("ModVersion"), StringPinType, OutError)) return false;
    if (!AddBlueprintVariable(Blueprint, TEXT("ModDescription"), StringPinType, OutError)) return false;
    if (!AddBlueprintVariable(Blueprint, TEXT("ModButtons"), StringArrayPinType, OutError)) return false;

    return true;
}

bool FNexusModsTemplateService::AddUE4SSCustomEvents(UBlueprint* Blueprint, FString& OutError) const {
    if (!Blueprint) {
        OutError = TEXT("Invalid blueprint while adding custom events.");
        return false;
    }

    UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(Blueprint);
    if (!EventGraph) {
        OutError = TEXT("Unable to find the ModActor event graph.");
        return false;
    }

    const FEdGraphPinType StringPinType = MakeStringPinType();
    const FEdGraphPinType IntegerPinType = MakeIntegerPinType();

    TArray<TPair<FName, FEdGraphPinType>> PrintToModLoaderParameters;
    PrintToModLoaderParameters.Add(TPair<FName, FEdGraphPinType>(TEXT("Message"), StringPinType));

    TArray<TPair<FName, FEdGraphPinType>> EmptyParameters;

    TArray<TPair<FName, FEdGraphPinType>> ModMenuButtonPressedParameters;
    ModMenuButtonPressedParameters.Add(TPair<FName, FEdGraphPinType>(TEXT("Index"), IntegerPinType));

    if (!AddCustomEvent(Blueprint, EventGraph, TEXT("PrintToModLoader"), PrintToModLoaderParameters, 0, OutError)) return false;
    if (!AddCustomEvent(Blueprint, EventGraph, TEXT("PreBeginPlay"), EmptyParameters, 2, OutError)) return false;
    if (!AddCustomEvent(Blueprint, EventGraph, TEXT("PostBeginPlay"), EmptyParameters, 3, OutError)) return false;
    if (!AddCustomEvent(Blueprint, EventGraph, TEXT("ModMenuButtonPressed"), ModMenuButtonPressedParameters, 4, OutError)) return false;

    return true;
}


bool FNexusModsTemplateService::PrepareUE4SSEventGraph(UBlueprint* Blueprint, FString& OutError) const {
    if (!Blueprint) {
        OutError = TEXT("Invalid blueprint while preparing the event graph.");
        return false;
    }

    UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(Blueprint);
    if (!EventGraph) {
        OutError = TEXT("Unable to find the ModActor event graph.");
        return false;
    }

    EventGraph->Modify();

    TArray<UEdGraphNode*> NodesToRemove;

    for (UEdGraphNode* Node : EventGraph->Nodes) {
        UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
        if (!EventNode) {
            continue;
        }

        const FName EventName = EventNode->EventReference.GetMemberName();

        if (EventName == TEXT("ReceiveBeginPlay")) {
            EventNode->Modify();
            EventNode->NodePosX = 0;
            EventNode->NodePosY = 220;
            continue;
        }

        if (EventName == TEXT("ReceiveTick") || EventName == TEXT("ReceiveActorBeginOverlap")) {
            NodesToRemove.Add(EventNode);
        }
    }

    for (UEdGraphNode* NodeToRemove : NodesToRemove) {
        FBlueprintEditorUtils::RemoveNode(Blueprint, NodeToRemove, true);
    }

    EventGraph->NotifyGraphChanged();
    return true;
}

bool FNexusModsTemplateService::AddBlueprintVariable(UBlueprint* Blueprint, const FName VariableName, const FEdGraphPinType& PinType, FString& OutError) const {
    if (!Blueprint) {
        OutError = TEXT("Invalid blueprint while adding variables.");
        return false;
    }

    if (!FBlueprintEditorUtils::AddMemberVariable(Blueprint, VariableName, PinType)) {
        OutError = FString::Printf(TEXT("Unable to add variable: %s"), *VariableName.ToString());
        return false;
    }

    return true;
}

bool FNexusModsTemplateService::AddCustomEvent(UBlueprint* Blueprint, UEdGraph* EventGraph, const FName EventName, const TArray<TPair<FName, FEdGraphPinType>>& Parameters, int32 NodeIndex, FString& OutError) const {
    if (!Blueprint || !EventGraph) {
        OutError = TEXT("Invalid blueprint graph while adding custom events.");
        return false;
    }

    UK2Node_CustomEvent* CustomEventNode = NewObject<UK2Node_CustomEvent>(EventGraph);
    if (!CustomEventNode) {
        OutError = FString::Printf(TEXT("Unable to create custom event: %s"), *EventName.ToString());
        return false;
    }

    EventGraph->Modify();
    EventGraph->AddNode(CustomEventNode, true, false);

    CustomEventNode->SetFlags(RF_Transactional);
    CustomEventNode->Modify();
    CustomEventNode->CreateNewGuid();
    CustomEventNode->PostPlacedNewNode();
    CustomEventNode->CustomFunctionName = EventName;
    CustomEventNode->NodePosX = 0;
    CustomEventNode->NodePosY = NodeIndex * 220;
    CustomEventNode->AllocateDefaultPins();

    for (const TPair<FName, FEdGraphPinType>& Parameter : Parameters) {
        CustomEventNode->CreateUserDefinedPin(Parameter.Key, Parameter.Value, EGPD_Output);
    }

    CustomEventNode->ReconstructNode();
    EventGraph->NotifyGraphChanged();
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

    return true;
}

bool FNexusModsTemplateService::FinalizeAndSaveBlueprint(UBlueprint* Blueprint, FString& OutError) const {
    if (!Blueprint) {
        OutError = TEXT("Invalid blueprint while saving.");
        return false;
    }

    Blueprint->Modify();

    FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
    FKismetEditorUtilities::CompileBlueprint(Blueprint);

    if (Blueprint->Status == BS_Error) {
        OutError = TEXT("The ModActor blueprint was created, but it failed to compile.");
        return false;
    }

    if (Blueprint->Status != BS_UpToDate) {
        FKismetEditorUtilities::CompileBlueprint(Blueprint);

        if (Blueprint->Status == BS_Error) {
            OutError = TEXT("The ModActor blueprint was created, but it failed to compile.");
            return false;
        }
    }

    UPackage* Package = Blueprint->GetOutermost();
    if (!Package) {
        OutError = TEXT("Unable to find the ModActor package while saving.");
        return false;
    }

    Package->MarkPackageDirty();

    const FString PackageFilename = FPackageName::LongPackageNameToFilename(
        Package->GetName(),
        FPackageName::GetAssetPackageExtension()
    );

    const bool bSaved = SavePackageForCurrentEngine(Package, Blueprint, PackageFilename);

    if (!bSaved) {
        OutError = TEXT("The ModActor was created, but it could not be saved to disk.");
        return false;
    }

    Package->SetDirtyFlag(false);
    return true;
}

FString FNexusModsTemplateService::GetTemplateImageAssetPath(const FString& TemplatePath) const {
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter TemplateFilter;
    TemplateFilter.PackagePaths.Add(*TemplatePath);
    TemplateFilter.bRecursivePaths = false;

    TArray<FAssetData> TemplateAssets;
    AssetRegistry.GetAssets(TemplateFilter, TemplateAssets);

    for (const FAssetData& TemplateAsset : TemplateAssets) {
        if (TemplateAsset.AssetName == TEXT("TemplateImage") || TemplateAsset.AssetName == TEXT("T_TemplateImage")) {
            return GetAssetObjectPathString(TemplateAsset);
        }
    }

    return FString();
}

bool FNexusModsTemplateService::IsTemplateImageAsset(const FString& AssetPath) const {
    const FString ObjectPathAssetName = AssetPath.Contains(TEXT("."))
        ? AssetPath.RightChop(AssetPath.Find(TEXT("."), ESearchCase::CaseSensitive) + 1)
        : FPackageName::GetLongPackageAssetName(AssetPath);

    return ObjectPathAssetName == TEXT("TemplateImage") || ObjectPathAssetName == TEXT("T_TemplateImage");
}

bool FNexusModsTemplateService::IsTemplateImageAsset(const FAssetData& AssetData) const {
    return AssetData.AssetName == TEXT("TemplateImage") || AssetData.AssetName == TEXT("T_TemplateImage");
}

bool FNexusModsTemplateService::IsRootPrimaryAssetLabel(const FAssetData& AssetData, const FString& TemplatePath) const {
    if (AssetData.PackagePath.ToString() != TemplatePath) {
        return false;
    }

    return AssetData.GetClass() && AssetData.GetClass()->IsChildOf(UPrimaryAssetLabel::StaticClass());
}

bool FNexusModsTemplateService::DuplicateTemplateAssets(const FNexusModsModTemplate& Template, const FString& DestinationPath, const FString& ModName, FString& OutError) const {
    TArray<FAssetData> SourceAssets;
    if (!GetTemplateAssets(Template.SourcePath, SourceAssets, OutError)) {
        return false;
    }

    TMap<FString, FString> SourceAndDestPackages;

    for (const FAssetData& SourceAssetData : SourceAssets) {
        if (IsTemplateImageAsset(SourceAssetData)) {
            continue;
        }

        const FString SourcePackageName = SourceAssetData.PackageName.ToString();
        const FString TargetPackageName = GetTargetPackageNameForTemplateAsset(SourceAssetData, Template.SourcePath, DestinationPath, ModName);

        if (SourcePackageName.IsEmpty() || TargetPackageName.IsEmpty()) {
            continue;
        }

        SourceAndDestPackages.Add(SourcePackageName, TargetPackageName);
    }

    if (SourceAndDestPackages.Num() == 0) {
        OutError = FString::Printf(TEXT("The template folder does not contain any assets that can be copied: %s"), *Template.SourcePath);
        return false;
    }

    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
    IAssetTools& AssetTools = AssetToolsModule.Get();

#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 1)
    const bool bCopied = AssetTools.AdvancedCopyPackages(
        SourceAndDestPackages,
        true,
        false,
        nullptr,
        EMessageSeverity::Error
    );
#else
    const bool bCopied = AssetTools.AdvancedCopyPackages(
        SourceAndDestPackages,
        true,
        false
    );
#endif

    if (!bCopied) {
        OutError = TEXT("Unable to advanced copy the selected template. The destination may already contain one or more conflicting assets.");
        return false;
    }

    RemoveTemplateImageAssetsFromDestination(DestinationPath);

    TArray<FString> PathsToScan;
    PathsToScan.Add(DestinationPath);

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    AssetRegistryModule.Get().ScanPathsSynchronous(PathsToScan, true);

    TArray<FAssetData> CopiedAssets;
    FARFilter DestinationFilter;
    DestinationFilter.PackagePaths.Add(*DestinationPath);
    DestinationFilter.bRecursivePaths = true;
    AssetRegistryModule.Get().GetAssets(DestinationFilter, CopiedAssets);

    TArray<UObject*> LoadedCopiedAssets;
    for (const FAssetData& CopiedAssetData : CopiedAssets) {
        UObject* CopiedAsset = CopiedAssetData.GetAsset();
        if (CopiedAsset) {
            LoadedCopiedAssets.Add(CopiedAsset);
        }
    }

    if (!SaveCopiedAssets(LoadedCopiedAssets, OutError)) {
        return false;
    }

    return true;
}

bool FNexusModsTemplateService::GetTemplateAssets(const FString& TemplatePath, TArray<FAssetData>& OutAssets, FString& OutError) const {
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter TemplateFilter;
    TemplateFilter.PackagePaths.Add(*TemplatePath);
    TemplateFilter.bRecursivePaths = true;
    AssetRegistry.GetAssets(TemplateFilter, OutAssets);

    if (OutAssets.Num() == 0) {
        OutError = FString::Printf(TEXT("The template folder does not contain any assets: %s"), *TemplatePath);
        return false;
    }

    return true;
}

FString FNexusModsTemplateService::GetTargetPackageNameForTemplateAsset(const FAssetData& SourceAssetData, const FString& TemplatePath, const FString& DestinationPath, const FString& ModName) const {
    FString RelativePackagePath = SourceAssetData.PackagePath.ToString();
    RelativePackagePath.RemoveFromStart(TemplatePath);
    RelativePackagePath.RemoveFromStart(TEXT("/"));

    FString TargetPackagePath = DestinationPath;
    if (!RelativePackagePath.IsEmpty()) {
        TargetPackagePath /= RelativePackagePath;
    }

    FString TargetAssetName = SourceAssetData.AssetName.ToString();
    if (IsRootPrimaryAssetLabel(SourceAssetData, TemplatePath)) {
        TargetAssetName = FString::Printf(TEXT("DA_%s"), *ModName);
    }

    return TargetPackagePath / TargetAssetName;
}

void FNexusModsTemplateService::RemoveTemplateImageAssetsFromDestination(const FString& DestinationPath) const {
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    TArray<FAssetData> DestinationAssets;

    FARFilter DestinationFilter;
    DestinationFilter.PackagePaths.Add(*DestinationPath);
    DestinationFilter.bRecursivePaths = true;
    AssetRegistry.GetAssets(DestinationFilter, DestinationAssets);

    for (const FAssetData& DestinationAsset : DestinationAssets) {
        if (!IsTemplateImageAsset(DestinationAsset)) {
            continue;
        }

        UEditorAssetLibrary::DeleteAsset(GetAssetObjectPathString(DestinationAsset));
    }
}

bool FNexusModsTemplateService::SaveCopiedAssets(const TArray<UObject*>& CopiedAssets, FString& OutError) const {
    for (UObject* CopiedAsset : CopiedAssets) {
        if (!CopiedAsset) continue;

        UPackage* Package = CopiedAsset->GetOutermost();
        if (!Package) {
            OutError = FString::Printf(TEXT("Unable to find package while saving copied asset: %s"), *CopiedAsset->GetName());
            return false;
        }

        const FString PackageFilename = FPackageName::LongPackageNameToFilename(
            Package->GetName(),
            FPackageName::GetAssetPackageExtension()
        );

        if (!SavePackageForCurrentEngine(Package, CopiedAsset, PackageFilename)) {
            OutError = FString::Printf(TEXT("Unable to save copied asset: %s"), *CopiedAsset->GetName());
            return false;
        }

        Package->SetDirtyFlag(false);
    }

    return true;
}

FEdGraphPinType FNexusModsTemplateService::MakeStringPinType() const {
    FEdGraphPinType PinType;
    PinType.PinCategory = UEdGraphSchema_K2::PC_String;
    PinType.ContainerType = EPinContainerType::None;
    return PinType;
}

FEdGraphPinType FNexusModsTemplateService::MakeStringArrayPinType() const {
    FEdGraphPinType PinType;
    PinType.PinCategory = UEdGraphSchema_K2::PC_String;
    PinType.ContainerType = EPinContainerType::Array;
    return PinType;
}

FEdGraphPinType FNexusModsTemplateService::MakeIntegerPinType() const {
    FEdGraphPinType PinType;
    PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
    PinType.ContainerType = EPinContainerType::None;
    return PinType;
}
