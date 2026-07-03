#include "Services/NexusModsCredentialStore.h"

#include "HAL/PlatformMisc.h"

namespace {
    const TCHAR* StoreId = TEXT("NexusModsUploader");
    const TCHAR* GlobalSection = TEXT("Global");
    const TCHAR* ApiKeyKey = TEXT("ApiKey");
}

FString FNexusModsCredentialStore::MakeSectionName(const FString& GameDomain) const {
    return FString::Printf(TEXT("NexusModsUploader/%s"), *GameDomain);
}

FString FNexusModsCredentialStore::MakeKeyName(const FGuid& EntryId) const {
    return FString::Printf(TEXT("%s/FileId"), *EntryId.ToString());
}

bool FNexusModsCredentialStore::SaveApiKey(const FString& ApiKey) {
    //if (ApiKey.IsEmpty()) return false;
    return FPlatformMisc::SetStoredValue(StoreId, GlobalSection, ApiKeyKey, ApiKey);
}

bool FNexusModsCredentialStore::LoadApiKey(FString& OutApiKey) const {
    return FPlatformMisc::GetStoredValue(StoreId, GlobalSection, ApiKeyKey, OutApiKey);
}

bool FNexusModsCredentialStore::DeleteApiKey() {
    return FPlatformMisc::DeleteStoredValue(StoreId, GlobalSection, ApiKeyKey);
}

bool FNexusModsCredentialStore::HasApiKey() const {
    FString ExistingValue;
    return LoadApiKey(ExistingValue) && !ExistingValue.IsEmpty();
}

bool FNexusModsCredentialStore::SaveFileId(const FString& GameDomain, const FGuid& EntryId, const FString& FileId) {
    if (GameDomain.IsEmpty() || !EntryId.IsValid() || FileId.IsEmpty()) return false;
    return FPlatformMisc::SetStoredValue(
        StoreId,
        MakeSectionName(GameDomain),
        MakeKeyName(EntryId),
        FileId
    );
}

bool FNexusModsCredentialStore::LoadFileId(const FString& GameDomain, const FGuid& EntryId, FString& OutFileId) const {
    if (GameDomain.IsEmpty() || !EntryId.IsValid()) return false;
    return FPlatformMisc::GetStoredValue(
        StoreId,
        MakeSectionName(GameDomain),
        MakeKeyName(EntryId),
        OutFileId
    );
}

bool FNexusModsCredentialStore::DeleteFileId(const FString& GameDomain, const FGuid& EntryId) {
    if (GameDomain.IsEmpty() || !EntryId.IsValid()) return false;
    return FPlatformMisc::DeleteStoredValue(
        StoreId,
        MakeSectionName(GameDomain),
        MakeKeyName(EntryId)
    );
}

bool FNexusModsCredentialStore::HasFileId(const FString& GameDomain, const FGuid& EntryId) const {
    FString ExistingValue;
    return LoadFileId(GameDomain, EntryId, ExistingValue) && !ExistingValue.IsEmpty();
}
