#pragma once

#include "CoreMinimal.h"

class FNexusModsCredentialStore {

public:
    bool SaveApiKey(const FString& ApiKey);
    bool LoadApiKey(FString& OutApiKey) const;
    bool DeleteApiKey();
    bool HasApiKey() const;

    bool SaveFileId(const FString& GameDomain, const FGuid& EntryId, const FString& FileId);
    bool LoadFileId(const FString& GameDomain, const FGuid& EntryId, FString& OutFileId) const;
    bool DeleteFileId(const FString& GameDomain, const FGuid& EntryId);
    bool HasFileId(const FString& GameDomain, const FGuid& EntryId) const;

private:
    FString MakeSectionName(const FString& GameDomain) const;
    FString MakeKeyName(const FGuid& EntryId) const;
};
