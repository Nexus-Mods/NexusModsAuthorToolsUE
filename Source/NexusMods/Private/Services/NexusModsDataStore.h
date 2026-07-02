#pragma once

#include "CoreMinimal.h"
#include "Services/NexusModsCredentialStore.h"

struct FNexusModsModUploadState {
    FGuid EntryId;

    FString DisplayName;
    FString ArchivePath;
    FString Version;
    FString Description;
    FString Category = TEXT("main");

    bool bAutoIncrementVersion = false;
    bool bArchiveExistingVersion = false;
    bool bPrimaryModManagerDownload = false;
    bool bAllowModManagerDownload = true;
    bool bShowRequirementsPopUp = false;
};

class FNexusModsDataStore {

public:
    FNexusModsDataStore();

    FString GetGameDomain() const;
    void SetGameDomain(const FString& InGameDomain);

    TArray<FNexusModsModUploadState> GetMods() const;
    bool GetModState(const FGuid& EntryId, FNexusModsModUploadState& OutState) const;
    void SetModState(FNexusModsModUploadState InState);
    void RemoveModState(const FGuid& EntryId);

    bool SaveApiKey(const FString& ApiKey);
    bool LoadApiKey(FString& OutApiKey) const;
    bool DeleteApiKey();
    bool HasApiKey() const;

    bool SaveFileId(const FGuid& EntryId, const FString& FileId);
    bool LoadFileId(const FGuid& EntryId, FString& OutFileId) const;
    bool DeleteFileId(const FGuid& EntryId);
    bool HasFileId(const FGuid& EntryId) const;

    bool Load();
    bool Save() const;

private:
    /** Functions: **/
    FString GetStoreFilePath() const;

    /** Variables: **/
    FString GameDomain;
    TMap<FGuid, FNexusModsModUploadState> ModsByEntryId;
    FNexusModsCredentialStore CredentialStore;
};