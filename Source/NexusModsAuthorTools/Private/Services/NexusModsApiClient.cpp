#include "Services/NexusModsApiClient.h"

#include "Async/Async.h"
#include "Dom/JsonObject.h"
#include "HAL/PlatformFileManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace {
    constexpr int32 MaxUploadPollAttempts = 60;
    constexpr float UploadPollDelaySeconds = 2.0f;

    FString GetJsonStringField(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) {
        if (!JsonObject.IsValid()) {
            return FString();
        }

        FString Value;
        JsonObject->TryGetStringField(FieldName, Value);
        return Value;
    }

    int64 GetJsonInt64Field(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) {
        if (!JsonObject.IsValid()) {
            return 0;
        }

        FString StringValue;
        if (JsonObject->TryGetStringField(FieldName, StringValue)) {
            int64 ParsedValue = 0;
            if (LexTryParseString(ParsedValue, *StringValue)) {
                return ParsedValue;
            }
        }

        double NumberValue = 0.0;
        if (JsonObject->TryGetNumberField(FieldName, NumberValue)) {
            return static_cast<int64>(NumberValue);
        }

        return 0;
    }

    FString StripETagQuotes(const FString& InETag) {
        FString ETag = InETag;
        ETag.ReplaceInline(TEXT("\""), TEXT(""));
        return ETag;
    }
}

void FNexusModsApiClient::UploadModFile(
    const FNexusModsUploadRequest& Request,
    FNexusModsUploadLogDelegate OnLog,
    FNexusModsUploadProgressDelegate OnProgress,
    FNexusModsUploadCompleteDelegate OnComplete
) {
    if (!bEnableUploads) {
        OnLog.ExecuteIfBound(TEXT("Upload functionality is currently disabled"));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    ResetUploadState();

    ActiveRequest = Request;
    LogDelegate = OnLog;
    ProgressDelegate = OnProgress;
    CompleteDelegate = OnComplete;
    bUploadActive = true;

    LogDelegate.ExecuteIfBound(TEXT("Starting upload..."));
    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Game: %s"), *ActiveRequest.GameDomain));
    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Filename: %s"), *ActiveRequest.Filename));
    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Archive: %s"), *ActiveRequest.ArchivePath));
    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Version: %s"), *ActiveRequest.Version));
    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Display name: %s"), *ActiveRequest.DisplayName));
    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Category: %s"), *NormalizeCategory(ActiveRequest.Category)));

    if (!ValidateUploadRequest()) {
        return;
    }

    ReportProgress();
    CreateMultipartUpload();
}

void FNexusModsApiClient::ResetUploadState() {
    ActiveRequest = FNexusModsUploadRequest();
    UploadId.Reset();
    CompletePresignedUrl.Reset();
    PartPresignedUrls.Reset();
    UploadedParts.Reset();
    ArchiveSizeBytes = 0;
    PartSizeBytes = 0;
    UploadedBytes = 0;
    NextPartIndex = 0;
    bUploadActive = false;
}

bool FNexusModsApiClient::ValidateUploadRequest() {
    if (ActiveRequest.ApiKey.IsEmpty()) {
        FailUpload(TEXT("API key is missing."));
        return false;
    }

    if (ActiveRequest.FileId.IsEmpty()) {
        FailUpload(TEXT("File ID / upload code is missing."));
        return false;
    }

    if (ActiveRequest.ArchivePath.IsEmpty()) {
        FailUpload(TEXT("Archive path is missing."));
        return false;
    }

    if (!FPaths::FileExists(ActiveRequest.ArchivePath)) {
        FailUpload(TEXT("Archive file does not exist."));
        return false;
    }

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    const int64 FileSize = PlatformFile.FileSize(*ActiveRequest.ArchivePath);
    if (FileSize <= 0) {
        FailUpload(TEXT("Archive file is empty or could not be read."));
        return false;
    }

    if (ActiveRequest.Version.IsEmpty()) {
        FailUpload(TEXT("Version is missing."));
        return false;
    }

    return true;
}

void FNexusModsApiClient::CreateMultipartUpload() {
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    ArchiveSizeBytes = PlatformFile.FileSize(*ActiveRequest.ArchivePath);

    TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
    Body->SetStringField(TEXT("filename"), FPaths::GetCleanFilename(ActiveRequest.ArchivePath));
    Body->SetStringField(TEXT("size_bytes"), FString::Printf(TEXT("%lld"), ArchiveSizeBytes));

    FString BodyString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
    FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);

    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Creating multipart upload for %lld bytes..."), ArchiveSizeBytes));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateNexusRequest(TEXT("/uploads/multipart"), TEXT("POST"));
    Request->SetContentAsString(BodyString);
    Request->OnProcessRequestComplete().BindRaw(this, &FNexusModsApiClient::HandleCreateMultipartUploadResponse);
    Request->ProcessRequest();
}

void FNexusModsApiClient::HandleCreateMultipartUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded) {
    if (!bSucceeded || !IsSuccessResponse(Response)) {
        FailUpload(GetResponseError(TEXT("Failed to create multipart upload"), Response));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid()) {
        FailUpload(TEXT("Failed to parse multipart upload response."));
        return;
    }

    const TSharedPtr<FJsonObject>* DataObject = nullptr;
    if (!JsonObject->TryGetObjectField(TEXT("data"), DataObject) || !DataObject || !DataObject->IsValid()) {
        FailUpload(TEXT("Multipart upload response did not contain data."));
        return;
    }

    UploadId = GetJsonStringField(*DataObject, TEXT("id"));
    CompletePresignedUrl = GetJsonStringField(*DataObject, TEXT("complete_presigned_url"));
    PartSizeBytes = GetJsonInt64Field(*DataObject, TEXT("part_size_bytes"));

    const TArray<TSharedPtr<FJsonValue>>* PartUrls = nullptr;
    if ((*DataObject)->TryGetArrayField(TEXT("part_presigned_urls"), PartUrls)) {
        for (const TSharedPtr<FJsonValue>& PartUrl : *PartUrls) {
            PartPresignedUrls.Add(PartUrl->AsString());
        }
    }

    if (UploadId.IsEmpty() || CompletePresignedUrl.IsEmpty() || PartSizeBytes <= 0 || PartPresignedUrls.Num() == 0) {
        FailUpload(TEXT("Multipart upload response was incomplete."));
        return;
    }

    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Created multipart upload %s with %d part(s)."), *UploadId, PartPresignedUrls.Num()));
    UploadNextPart();
}

void FNexusModsApiClient::UploadNextPart() {
    if (NextPartIndex >= PartPresignedUrls.Num()) {
        CompleteMultipartUpload();
        return;
    }

    const int32 PartNumber = NextPartIndex + 1;
    const int64 Offset = static_cast<int64>(NextPartIndex) * PartSizeBytes;
    const int64 CurrentPartSize = FMath::Min(PartSizeBytes, ArchiveSizeBytes - Offset);

    if (CurrentPartSize <= 0) {
        FailUpload(FString::Printf(TEXT("Invalid part size for part %d."), PartNumber));
        return;
    }

    TArray<uint8> PartData;
    PartData.SetNumUninitialized(static_cast<int32>(CurrentPartSize));

    TUniquePtr<IFileHandle> FileHandle(FPlatformFileManager::Get().GetPlatformFile().OpenRead(*ActiveRequest.ArchivePath));
    if (!FileHandle.IsValid()) {
        FailUpload(TEXT("Failed to open archive file for reading."));
        return;
    }

    if (!FileHandle->Seek(Offset) || !FileHandle->Read(PartData.GetData(), CurrentPartSize)) {
        FailUpload(FString::Printf(TEXT("Failed to read upload part %d."), PartNumber));
        return;
    }

    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Uploading part %d/%d (%lld bytes)..."), PartNumber, PartPresignedUrls.Num(), CurrentPartSize));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(PartPresignedUrls[NextPartIndex]);
    Request->SetVerb(TEXT("PUT"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
    Request->SetHeader(TEXT("Content-Length"), FString::Printf(TEXT("%lld"), CurrentPartSize));
    Request->SetContent(MoveTemp(PartData));
    Request->OnProcessRequestComplete().BindRaw(this, &FNexusModsApiClient::HandleUploadPartResponse, PartNumber, CurrentPartSize);
    Request->ProcessRequest();
}

void FNexusModsApiClient::HandleUploadPartResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded, int32 PartNumber, int64 PartSize) {
    if (!bSucceeded || !IsSuccessResponse(Response)) {
        FailUpload(GetResponseError(FString::Printf(TEXT("Failed to upload part %d"), PartNumber), Response));
        return;
    }

    FString ETag = Response->GetHeader(TEXT("ETag"));
    if (ETag.IsEmpty()) {
        FailUpload(FString::Printf(TEXT("No ETag returned for upload part %d."), PartNumber));
        return;
    }

    FNexusModsMultipartUploadPart UploadedPart;
    UploadedPart.PartNumber = PartNumber;
    UploadedPart.ETag = StripETagQuotes(ETag);
    UploadedParts.Add(UploadedPart);

    UploadedBytes += PartSize;
    NextPartIndex++;
    ReportProgress();

    UploadNextPart();
}

void FNexusModsApiClient::CompleteMultipartUpload() {
    UploadedParts.Sort([](const FNexusModsMultipartUploadPart& A, const FNexusModsMultipartUploadPart& B) {
        return A.PartNumber < B.PartNumber;
    });

    LogDelegate.ExecuteIfBound(TEXT("Completing multipart upload..."));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(CompletePresignedUrl);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/xml"));
    Request->SetContentAsString(BuildCompleteMultipartXml());
    Request->OnProcessRequestComplete().BindRaw(this, &FNexusModsApiClient::HandleCompleteMultipartUploadResponse);
    Request->ProcessRequest();
}

void FNexusModsApiClient::HandleCompleteMultipartUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded) {
    if (!bSucceeded || !IsSuccessResponse(Response)) {
        FailUpload(GetResponseError(TEXT("Failed to complete multipart upload"), Response));
        return;
    }

    LogDelegate.ExecuteIfBound(TEXT("Multipart upload completed."));
    FinaliseUpload();
}

void FNexusModsApiClient::FinaliseUpload() {
    LogDelegate.ExecuteIfBound(TEXT("Finalising upload..."));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateNexusRequest(FString::Printf(TEXT("/uploads/%s/finalise"), *UploadId), TEXT("POST"));
    Request->OnProcessRequestComplete().BindRaw(this, &FNexusModsApiClient::HandleFinaliseUploadResponse);
    Request->ProcessRequest();
}

void FNexusModsApiClient::HandleFinaliseUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded) {
    if (!bSucceeded || !IsSuccessResponse(Response)) {
        FailUpload(GetResponseError(TEXT("Failed to finalise upload"), Response));
        return;
    }

    LogDelegate.ExecuteIfBound(TEXT("Upload finalised. Waiting for Nexus Mods to process it..."));
    PollUploadState(0);
}

void FNexusModsApiClient::PollUploadState(int32 Attempt) {
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateNexusRequest(FString::Printf(TEXT("/uploads/%s"), *UploadId), TEXT("GET"));
    Request->OnProcessRequestComplete().BindRaw(this, &FNexusModsApiClient::HandlePollUploadStateResponse, Attempt);
    Request->ProcessRequest();
}

void FNexusModsApiClient::HandlePollUploadStateResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded, int32 Attempt) {
    if (!bSucceeded || !IsSuccessResponse(Response)) {
        FailUpload(GetResponseError(TEXT("Failed to get upload state"), Response));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid()) {
        FailUpload(TEXT("Failed to parse upload state response."));
        return;
    }

    const TSharedPtr<FJsonObject>* DataObject = nullptr;
    if (!JsonObject->TryGetObjectField(TEXT("data"), DataObject) || !DataObject || !DataObject->IsValid()) {
        FailUpload(TEXT("Upload state response did not contain data."));
        return;
    }

    const FString State = GetJsonStringField(*DataObject, TEXT("state"));
    LogDelegate.ExecuteIfBound(FString::Printf(TEXT("Upload state: %s"), State.IsEmpty() ? TEXT("unknown") : *State));

    if (State.Equals(TEXT("available"), ESearchCase::IgnoreCase)) {
        CreateModFileVersion();
        return;
    }

    if (Attempt + 1 >= MaxUploadPollAttempts) {
        FailUpload(TEXT("Upload processing timed out."));
        return;
    }

    Async(EAsyncExecution::ThreadPool, [this, Attempt]() {
        FPlatformProcess::Sleep(UploadPollDelaySeconds);
        AsyncTask(ENamedThreads::GameThread, [this, Attempt]() {
            if (bUploadActive) {
                PollUploadState(Attempt + 1);
            }
        });
    });
}

void FNexusModsApiClient::CreateModFileVersion() {
    LogDelegate.ExecuteIfBound(TEXT("Creating mod file version..."));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateNexusRequest(FString::Printf(TEXT("/mod-files/%s/versions"), *ActiveRequest.FileId), TEXT("POST"));
    Request->SetContentAsString(BuildCreateModFileVersionBody());
    Request->OnProcessRequestComplete().BindRaw(this, &FNexusModsApiClient::HandleCreateModFileVersionResponse);
    Request->ProcessRequest();
}

void FNexusModsApiClient::HandleCreateModFileVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded) {
    if (!bSucceeded || !IsSuccessResponse(Response)) {
        FailUpload(GetResponseError(TEXT("Failed to create mod file version"), Response));
        return;
    }

    LogDelegate.ExecuteIfBound(TEXT("Mod file version created successfully."));
    CompleteUpload();
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> FNexusModsApiClient::CreateNexusRequest(const FString& Url, const FString& Verb) const {
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(ApiBaseUrl + Url);
    Request->SetVerb(Verb);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
    Request->SetHeader(TEXT("apikey"), ActiveRequest.ApiKey);
    Request->SetHeader(TEXT("User-Agent"), TEXT("NexusMods-UnrealEditorPlugin"));
    return Request;
}

FString FNexusModsApiClient::BuildCompleteMultipartXml() const {
    FString Xml = TEXT("<CompleteMultipartUpload>");
    for (const FNexusModsMultipartUploadPart& UploadedPart : UploadedParts) {
        Xml += FString::Printf(
            TEXT("<Part><PartNumber>%d</PartNumber><ETag>%s</ETag></Part>"),
            UploadedPart.PartNumber,
            *UploadedPart.ETag
        );
    }
    Xml += TEXT("</CompleteMultipartUpload>");
    return Xml;
}

FString FNexusModsApiClient::BuildCreateModFileVersionBody() const {
    TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
    Body->SetStringField(TEXT("upload_id"), UploadId);
    Body->SetStringField(TEXT("name"), ActiveRequest.DisplayName.IsEmpty() ? ActiveRequest.Filename : ActiveRequest.DisplayName);
    Body->SetStringField(TEXT("version"), ActiveRequest.Version);
    Body->SetStringField(TEXT("file_category"), NormalizeCategory(ActiveRequest.Category));
    Body->SetBoolField(TEXT("archive_existing_file"), ActiveRequest.bArchiveExistingVersion);
    Body->SetBoolField(TEXT("primary_mod_manager_download"), ActiveRequest.bPrimaryModManagerDownload);
    Body->SetBoolField(TEXT("allow_mod_manager_download"), ActiveRequest.bAllowModManagerDownload);
    Body->SetBoolField(TEXT("show_requirements_pop_up"), ActiveRequest.bShowRequirementsPopUp);

    if (!ActiveRequest.Description.IsEmpty()) {
        Body->SetStringField(TEXT("description"), ActiveRequest.Description);
    }

    FString BodyString;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&BodyString);
    FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
    return BodyString;
}

FString FNexusModsApiClient::NormalizeCategory(const FString& Category) const {
    if (Category.Equals(TEXT("misc"), ESearchCase::IgnoreCase)) {
        return TEXT("miscellaneous");
    }

    if (Category.Equals(TEXT("miscellaneous"), ESearchCase::IgnoreCase)) {
        return TEXT("miscellaneous");
    }

    if (Category.Equals(TEXT("optional"), ESearchCase::IgnoreCase)) {
        return TEXT("optional");
    }

    return TEXT("main");
}

FString FNexusModsApiClient::GetResponseError(const FString& Prefix, FHttpResponsePtr Response) const {
    if (!Response.IsValid()) {
        return Prefix + TEXT(": no response received.");
    }

    const FString Body = Response->GetContentAsString();
    if (Body.IsEmpty()) {
        return FString::Printf(TEXT("%s: HTTP %d."), *Prefix, Response->GetResponseCode());
    }

    return FString::Printf(TEXT("%s: HTTP %d - %s"), *Prefix, Response->GetResponseCode(), *Body);
}

bool FNexusModsApiClient::IsSuccessResponse(FHttpResponsePtr Response) const {
    return Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode());
}

void FNexusModsApiClient::ReportProgress() {
    ProgressDelegate.ExecuteIfBound(UploadedBytes, ArchiveSizeBytes);
}

void FNexusModsApiClient::FailUpload(const FString& Error) {
    bUploadActive = false;
    LogDelegate.ExecuteIfBound(Error);
    CompleteDelegate.ExecuteIfBound(false);
}

void FNexusModsApiClient::CompleteUpload() {
    bUploadActive = false;
    UploadedBytes = ArchiveSizeBytes;
    ReportProgress();
    LogDelegate.ExecuteIfBound(TEXT("File uploaded successfully to Nexus Mods."));
    CompleteDelegate.ExecuteIfBound(true);
}
