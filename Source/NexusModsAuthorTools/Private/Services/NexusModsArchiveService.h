#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FNexusModsArchiveLogDelegate, const FString&);

struct FNexusModsArchiveBuildRequest {
    TArray<FString> SourcePaths;
    FString OutputDirectory;
    FString ArchiveName;
    int32 CompressionLevel = 6;
};

struct FNexusModsArchiveBuildResult {
    bool bSuccess = false;
    FString ArchivePath;
    FString ErrorMessage;
};

class FNexusModsArchiveService {

public:
    static FNexusModsArchiveBuildResult BuildArchive(
        const FNexusModsArchiveBuildRequest& Request,
        FNexusModsArchiveLogDelegate OnLog = FNexusModsArchiveLogDelegate()
    );

private:
    struct FNexusModsArchiveEntry {
        FString SourceFilePath;
        FString PathInsideArchive;
    };

    static bool ValidateRequest(const FNexusModsArchiveBuildRequest& Request, FString& OutError);
    static bool BuildArchiveEntries(const TArray<FString>& SourcePaths, TArray<FNexusModsArchiveEntry>& OutEntries, FString& OutError);
    static bool AddFilePath(const FString& SourcePath, TArray<FNexusModsArchiveEntry>& OutEntries, FString& OutError);
    static bool AddDirectoryPath(const FString& SourcePath, TArray<FNexusModsArchiveEntry>& OutEntries, FString& OutError);
    static FString NormalizeArchivePath(const FString& Path);
    static FString BuildOutputArchivePath(const FNexusModsArchiveBuildRequest& Request);
    static int32 ClampCompressionLevel(int32 CompressionLevel);
};
