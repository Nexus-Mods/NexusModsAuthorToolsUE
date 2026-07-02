#include "Services/NexusModsDataStore.h"

#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FNexusModsDataStore::FNexusModsDataStore() {
    // Empty contructor for now..
}

FString FNexusModsDataStore::GetStoreFilePath() const {
    return FPaths::ProjectSavedDir() / TEXT("NexusModsUploader/DataStore.json");
}

bool FNexusModsDataStore::Load() {
    GameDomain.Empty();
    ModsByEntryId.Empty();

    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *GetStoreFilePath())) {
        return true;
    }

    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);

    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid()) {
        return false;
    }

    RootObject->TryGetStringField(TEXT("GameDomain"), GameDomain);

    const TArray<TSharedPtr<FJsonValue>>* ModsArray = nullptr;
    if (RootObject->TryGetArrayField(TEXT("Mods"), ModsArray)) {
        for (const TSharedPtr<FJsonValue>& ModValue : *ModsArray) {
            const TSharedPtr<FJsonObject> ModObject = ModValue->AsObject();
            if (!ModObject.IsValid()) continue;

            FNexusModsModUploadState State;

            FString EntryIdString;
            if (ModObject->TryGetStringField(TEXT("EntryId"), EntryIdString)) {
                FGuid::Parse(EntryIdString, State.EntryId);
            }

            if (!State.EntryId.IsValid()) {
                State.EntryId = FGuid::NewGuid();
            }

            ModObject->TryGetStringField(TEXT("DisplayName"), State.DisplayName);
            ModObject->TryGetStringField(TEXT("ArchivePath"), State.ArchivePath);
            ModObject->TryGetStringField(TEXT("Version"), State.Version);
            ModObject->TryGetStringField(TEXT("Description"), State.Description);
            ModObject->TryGetStringField(TEXT("Category"), State.Category);

            ModObject->TryGetBoolField(TEXT("AutoIncrementVersion"), State.bAutoIncrementVersion);
            ModObject->TryGetBoolField(TEXT("ArchiveExistingVersion"), State.bArchiveExistingVersion);
            ModObject->TryGetBoolField(TEXT("PrimaryModManagerDownload"), State.bPrimaryModManagerDownload);
            ModObject->TryGetBoolField(TEXT("AllowModManagerDownload"), State.bAllowModManagerDownload);
            ModObject->TryGetBoolField(TEXT("ShowRequirementsPopUp"), State.bShowRequirementsPopUp);

            if (State.Category.IsEmpty()) {
                State.Category = TEXT("main");
            }

            ModsByEntryId.Add(State.EntryId, State);
        }
    }

    return true;
}

bool FNexusModsDataStore::Save() const {
    TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();

    RootObject->SetStringField(TEXT("GameDomain"), GameDomain);

    TArray<TSharedPtr<FJsonValue>> ModsArray;

    for (const TPair<FGuid, FNexusModsModUploadState>& Pair : ModsByEntryId) {
        const FNexusModsModUploadState& State = Pair.Value;

        TSharedRef<FJsonObject> ModObject = MakeShared<FJsonObject>();

        ModObject->SetStringField(TEXT("EntryId"), State.EntryId.ToString());
        ModObject->SetStringField(TEXT("DisplayName"), State.DisplayName);
        ModObject->SetStringField(TEXT("ArchivePath"), State.ArchivePath);
        ModObject->SetStringField(TEXT("Version"), State.Version);
        ModObject->SetStringField(TEXT("Description"), State.Description);
        ModObject->SetStringField(TEXT("Category"), State.Category);

        ModObject->SetBoolField(TEXT("AutoIncrementVersion"), State.bAutoIncrementVersion);
        ModObject->SetBoolField(TEXT("ArchiveExistingVersion"), State.bArchiveExistingVersion);
        ModObject->SetBoolField(TEXT("PrimaryModManagerDownload"), State.bPrimaryModManagerDownload);
        ModObject->SetBoolField(TEXT("AllowModManagerDownload"), State.bAllowModManagerDownload);
        ModObject->SetBoolField(TEXT("ShowRequirementsPopUp"), State.bShowRequirementsPopUp);

        ModsArray.Add(MakeShared<FJsonValueObject>(ModObject));
    }

    RootObject->SetArrayField(TEXT("Mods"), ModsArray);

    FString JsonText;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonText);

    if (!FJsonSerializer::Serialize(RootObject, Writer)) {
        return false;
    }

    IFileManager::Get().MakeDirectory(*FPaths::GetPath(GetStoreFilePath()), true);

    return FFileHelper::SaveStringToFile(JsonText, *GetStoreFilePath());
}

FString FNexusModsDataStore::GetGameDomain() const {
    return GameDomain;
}

void FNexusModsDataStore::SetGameDomain(const FString& InGameDomain) {
    GameDomain = InGameDomain;
}

TArray<FNexusModsModUploadState> FNexusModsDataStore::GetMods() const {
    TArray<FNexusModsModUploadState> Mods;
    ModsByEntryId.GenerateValueArray(Mods);
    Mods.Sort([](const FNexusModsModUploadState& A, const FNexusModsModUploadState& B) {
        return A.DisplayName < B.DisplayName;
    });
    return Mods;
}

bool FNexusModsDataStore::GetModState(const FGuid& EntryId, FNexusModsModUploadState& OutState) const {
    if (const FNexusModsModUploadState* Found = ModsByEntryId.Find(EntryId)) {
        OutState = *Found;
        return true;
    }
    return false;
}

void FNexusModsDataStore::SetModState(FNexusModsModUploadState InState) {
    if (!InState.EntryId.IsValid()) {
        InState.EntryId = FGuid::NewGuid();
    }
    if (InState.Category.IsEmpty()) {
        InState.Category = TEXT("main");
    }
    ModsByEntryId.Add(InState.EntryId, InState);
}

void FNexusModsDataStore::RemoveModState(const FGuid& EntryId) {
    ModsByEntryId.Remove(EntryId);
}


bool FNexusModsDataStore::SaveApiKey(const FString& ApiKey) {
    return CredentialStore.SaveApiKey(ApiKey);
}

bool FNexusModsDataStore::LoadApiKey(FString& OutApiKey) const {
    return CredentialStore.LoadApiKey(OutApiKey);
}

bool FNexusModsDataStore::DeleteApiKey() {
    return CredentialStore.DeleteApiKey();
}

bool FNexusModsDataStore::HasApiKey() const {
    return CredentialStore.HasApiKey();
}

bool FNexusModsDataStore::SaveFileId(const FGuid& EntryId, const FString& FileId) {
    return CredentialStore.SaveFileId(GameDomain, EntryId, FileId);
}

bool FNexusModsDataStore::LoadFileId(const FGuid& EntryId, FString& OutFileId) const {
    return CredentialStore.LoadFileId(GameDomain, EntryId, OutFileId);
}

bool FNexusModsDataStore::DeleteFileId(const FGuid& EntryId) {
    return CredentialStore.DeleteFileId(GameDomain, EntryId);
}

bool FNexusModsDataStore::HasFileId(const FGuid& EntryId) const {
    return CredentialStore.HasFileId(GameDomain, EntryId);
}