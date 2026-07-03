#include "Services/NexusModsArchiveService.h"

#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"

THIRD_PARTY_INCLUDES_START
#include "miniz.h"
THIRD_PARTY_INCLUDES_END

FNexusModsArchiveBuildResult FNexusModsArchiveService::BuildArchive(const FNexusModsArchiveBuildRequest& Request, FNexusModsArchiveLogDelegate OnLog) {
    FNexusModsArchiveBuildResult Result;

    FString Error;
    if (!ValidateRequest(Request, Error)) {
        Result.ErrorMessage = Error;
        return Result;
    }

    TArray<FNexusModsArchiveEntry> Entries;
    if (!BuildArchiveEntries(Request.SourcePaths, Entries, Error)) {
        Result.ErrorMessage = Error;
        return Result;
    }

    if (Entries.Num() == 0) {
        Result.ErrorMessage = TEXT("No files were found to add to the archive.");
        return Result;
    }

    const FString ArchivePath = BuildOutputArchivePath(Request);
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(ArchivePath), true);

    if (FPaths::FileExists(ArchivePath)) {
        if (!IFileManager::Get().Delete(*ArchivePath, false, true)) {
            Result.ErrorMessage = FString::Printf(TEXT("Unable to replace existing archive: %s"), *ArchivePath);
            return Result;
        }
    }

    mz_zip_archive ZipArchive;
    FMemory::Memzero(&ZipArchive, sizeof(ZipArchive));

    if (!mz_zip_writer_init_file(&ZipArchive, TCHAR_TO_UTF8(*ArchivePath), 0)) {
        Result.ErrorMessage = FString::Printf(TEXT("Unable to create archive: %s"), *ArchivePath);
        return Result;
    }

    const int32 CompressionLevel = ClampCompressionLevel(Request.CompressionLevel);

    for (const FNexusModsArchiveEntry& Entry : Entries) {
        if (OnLog.IsBound()) {
            OnLog.Execute(FString::Printf(TEXT("Adding %s"), *Entry.PathInsideArchive));
        }

        const mz_bool bAdded = mz_zip_writer_add_file(
            &ZipArchive,
            TCHAR_TO_UTF8(*Entry.PathInsideArchive),
            TCHAR_TO_UTF8(*Entry.SourceFilePath),
            nullptr,
            0,
            CompressionLevel
        );

        if (!bAdded) {
            mz_zip_writer_end(&ZipArchive);
            Result.ErrorMessage = FString::Printf(TEXT("Failed to add file to archive: %s"), *Entry.SourceFilePath);
            return Result;
        }
    }

    if (!mz_zip_writer_finalize_archive(&ZipArchive)) {
        mz_zip_writer_end(&ZipArchive);
        Result.ErrorMessage = FString::Printf(TEXT("Failed to finalise archive: %s"), *ArchivePath);
        return Result;
    }

    mz_zip_writer_end(&ZipArchive);

    Result.bSuccess = true;
    Result.ArchivePath = ArchivePath;
    return Result;
}

bool FNexusModsArchiveService::ValidateRequest(const FNexusModsArchiveBuildRequest& Request, FString& OutError) {
    if (Request.SourcePaths.Num() == 0) {
        OutError = TEXT("No source files or folders were selected.");
        return false;
    }

    if (Request.OutputDirectory.IsEmpty()) {
        OutError = TEXT("No archive output directory was selected.");
        return false;
    }

    if (Request.ArchiveName.IsEmpty()) {
        OutError = TEXT("No archive name was supplied.");
        return false;
    }

    return true;
}

bool FNexusModsArchiveService::BuildArchiveEntries(const TArray<FString>& SourcePaths, TArray<FNexusModsArchiveEntry>& OutEntries, FString& OutError) {
    for (const FString& SourcePath : SourcePaths) {
        if (FPaths::FileExists(SourcePath)) {
            if (!AddFilePath(SourcePath, OutEntries, OutError)) {
                return false;
            }
            continue;
        }

        if (FPaths::DirectoryExists(SourcePath)) {
            if (!AddDirectoryPath(SourcePath, OutEntries, OutError)) {
                return false;
            }
            continue;
        }

        OutError = FString::Printf(TEXT("Selected source path does not exist: %s"), *SourcePath);
        return false;
    }

    return true;
}

bool FNexusModsArchiveService::AddFilePath(const FString& SourcePath, TArray<FNexusModsArchiveEntry>& OutEntries, FString& OutError) {
    if (!FPaths::FileExists(SourcePath)) {
        OutError = FString::Printf(TEXT("Source file does not exist: %s"), *SourcePath);
        return false;
    }

    FNexusModsArchiveEntry Entry;
    Entry.SourceFilePath = SourcePath;
    Entry.PathInsideArchive = NormalizeArchivePath(FPaths::GetCleanFilename(SourcePath));
    OutEntries.Add(Entry);
    return true;
}

bool FNexusModsArchiveService::AddDirectoryPath(const FString& SourcePath, TArray<FNexusModsArchiveEntry>& OutEntries, FString& OutError) {
    if (!FPaths::DirectoryExists(SourcePath)) {
        OutError = FString::Printf(TEXT("Source directory does not exist: %s"), *SourcePath);
        return false;
    }

    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *SourcePath, TEXT("*"), true, false);

    const FString CleanRootPath = FPaths::ConvertRelativePathToFull(SourcePath);
    const FString RootDirectoryName = FPaths::GetCleanFilename(CleanRootPath);

    for (const FString& FilePath : Files) {
        FString RelativePath = FilePath;
        FPaths::MakePathRelativeTo(RelativePath, *CleanRootPath);

        FNexusModsArchiveEntry Entry;
        Entry.SourceFilePath = FilePath;
        Entry.PathInsideArchive = NormalizeArchivePath(FPaths::Combine(RootDirectoryName, RelativePath));
        OutEntries.Add(Entry);
    }

    return true;
}

FString FNexusModsArchiveService::NormalizeArchivePath(const FString& Path) {
    FString NormalizedPath = Path;
    FPaths::NormalizeFilename(NormalizedPath);
    NormalizedPath.ReplaceInline(TEXT("\\"), TEXT("/"));
    return NormalizedPath;
}

FString FNexusModsArchiveService::BuildOutputArchivePath(const FNexusModsArchiveBuildRequest& Request) {
    FString ArchiveName = Request.ArchiveName;
    if (!ArchiveName.EndsWith(TEXT(".zip"))) {
        ArchiveName += TEXT(".zip");
    }

    return FPaths::Combine(Request.OutputDirectory, ArchiveName);
}

int32 FNexusModsArchiveService::ClampCompressionLevel(int32 CompressionLevel) {
    return FMath::Clamp(CompressionLevel, 0, 10);
}
