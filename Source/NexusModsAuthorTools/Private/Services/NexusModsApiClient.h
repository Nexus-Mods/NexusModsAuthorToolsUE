#pragma once

#include "CoreMinimal.h"
#include "Http.h"

DECLARE_DELEGATE_OneParam(FNexusModsUploadLogDelegate, const FString&);
DECLARE_DELEGATE_OneParam(FNexusModsUploadCompleteDelegate, bool);
DECLARE_DELEGATE_TwoParams(FNexusModsUploadProgressDelegate, int64, int64);

struct FNexusModsUploadRequest {
    FString GameDomain;

    // Global Nexus Mods API key.
    FString ApiKey;

    // Sensitive upload code; maps to API parameter "file_id".
    FString FileId;

    FString Filename;
    FString ArchivePath;
    FString Version;
    FString DisplayName;
    FString Description;
    FString Category = TEXT("main");

    bool bArchiveExistingVersion = false;
    bool bPrimaryModManagerDownload = false;
    bool bAllowModManagerDownload = true;
    bool bShowRequirementsPopUp = false;
};

struct FNexusModsMultipartUploadPart {
    int32 PartNumber = 0;
    FString ETag;
};

class FNexusModsApiClient {

public:
    void UploadModFile(
        const FNexusModsUploadRequest& Request,
        FNexusModsUploadLogDelegate OnLog,
        FNexusModsUploadProgressDelegate OnProgress,
        FNexusModsUploadCompleteDelegate OnComplete
    );

private:
    /** Functions: **/
    void ResetUploadState();
    bool ValidateUploadRequest();

    void CreateMultipartUpload();
    void HandleCreateMultipartUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);

    void UploadNextPart();
    void HandleUploadPartResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded, int32 PartNumber, int64 PartSize);

    void CompleteMultipartUpload();
    void HandleCompleteMultipartUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);

    void FinaliseUpload();
    void HandleFinaliseUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);

    void PollUploadState(int32 Attempt);
    void HandlePollUploadStateResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded, int32 Attempt);

    void CreateModFileVersion();
    void HandleCreateModFileVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateNexusRequest(const FString& Url, const FString& Verb) const;
    FString BuildCompleteMultipartXml() const;
    FString BuildCreateModFileVersionBody() const;
    FString NormalizeCategory(const FString& Category) const;
    FString GetResponseError(const FString& Prefix, FHttpResponsePtr Response) const;
    bool IsSuccessResponse(FHttpResponsePtr Response) const;
    void ReportProgress();
    void FailUpload(const FString& Error);
    void CompleteUpload();

    /** Variables: **/
    FNexusModsUploadRequest ActiveRequest;
    FNexusModsUploadLogDelegate LogDelegate;
    FNexusModsUploadProgressDelegate ProgressDelegate;
    FNexusModsUploadCompleteDelegate CompleteDelegate;

    bool bEnableUploads = true;
    FString ApiBaseUrl = TEXT("https://api.nexusmods.com/v3");
    FString UploadId;
    FString CompletePresignedUrl;

    TArray<FString> PartPresignedUrls;
    TArray<FNexusModsMultipartUploadPart> UploadedParts;

    int64 ArchiveSizeBytes = 0;
    int64 PartSizeBytes = 0;
    int64 UploadedBytes = 0;
    int32 NextPartIndex = 0;
    bool bUploadActive = false;
};
