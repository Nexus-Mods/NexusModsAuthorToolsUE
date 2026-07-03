$ErrorActionPreference = "Stop"

# ============================================================
# Configuration
# ============================================================

$PluginFileName = "NexusModsAuthorTools.uplugin"
$ArchiveName = "NexusModsAuthorTools"

$UploadToGitHub = $true

$TargetPlatforms = "Win64"

$UnrealEngines = [ordered]@{
    "5.1" = "S:\UE_5.1"
    "5.2" = "S:\UE_5.2"
    "5.3" = "S:\UE_5.3"
    "5.4" = "S:\UE_5.4"
    # "5.5" = "S:\UE_5.5"
    # "5.6" = "S:\UE_5.6"
    # "5.7" = "S:\UE_5.7"
    # "5.8" = "S:\UE_5.8"
}

# ============================================================
# Paths
# ============================================================

$RootDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$PluginPath = Join-Path $RootDir $PluginFileName
$BuildRoot = Join-Path $RootDir "Build"
$DistRoot = Join-Path $RootDir "Dist"

# ============================================================
# Helpers
# ============================================================

function Write-Section {
    param([string]$Message)

    Write-Host ""
    Write-Host "============================================================"
    Write-Host $Message
    Write-Host "============================================================"
}

function Write-Step {
    param([string]$Message)

    Write-Host "[INFO] $Message"
}

function Write-WarningStep {
    param([string]$Message)

    Write-Host "[WARN] $Message"
}

function Test-CommandAvailable {
    param([string]$Command)

    return [bool](Get-Command $Command -ErrorAction SilentlyContinue)
}

function Test-IsPrereleaseVersion {
    param([string]$VersionName)

    return $VersionName -match "(alpha|beta|preview|pre-release|prerelease|rc)"
}

function Get-PluginInfo {
    if (!(Test-Path $PluginPath)) {
        throw "Could not find plugin file: $PluginPath"
    }

    $PluginJson = Get-Content $PluginPath -Raw | ConvertFrom-Json

    if ([string]::IsNullOrWhiteSpace($PluginJson.VersionName)) {
        throw "VersionName is missing from $PluginPath"
    }

    $FriendlyName = $PluginJson.FriendlyName

    if ([string]::IsNullOrWhiteSpace($FriendlyName)) {
        $FriendlyName = [System.IO.Path]::GetFileNameWithoutExtension($PluginFileName)
    }

    return [PSCustomObject]@{
        FriendlyName = $FriendlyName
        VersionName  = $PluginJson.VersionName
        Tag          = "v$($PluginJson.VersionName)"
        IsPrerelease = Test-IsPrereleaseVersion $PluginJson.VersionName
    }
}

function Test-Prerequisites {
    Write-Section "Checking prerequisites"

    if (!(Test-Path $RootDir)) {
        throw "Root directory does not exist: $RootDir"
    }

    if (!(Test-Path $PluginPath)) {
        throw "Plugin file does not exist: $PluginPath"
    }

    Write-Step "Plugin file found: $PluginPath"

    if ($UploadToGitHub) {
        if (!(Test-CommandAvailable "gh")) {
            throw "GitHub CLI is not installed or not available in PATH."
        }

        Write-Step "GitHub CLI found."

        gh auth status *> $null

        if ($LASTEXITCODE -ne 0) {
            throw "GitHub CLI is not authenticated. Run: gh auth login"
        }

        Write-Step "GitHub CLI authenticated."
    }
    else {
        Write-WarningStep "GitHub upload disabled."
    }

    foreach ($Engine in $UnrealEngines.GetEnumerator()) {
        $EngineVersion = $Engine.Key
        $EnginePath = $Engine.Value
        $RunUAT = Join-Path $EnginePath "Engine\Build\BatchFiles\RunUAT.bat"

        if (!(Test-Path $EnginePath)) {
            throw "Configured Unreal Engine path does not exist for UE $EngineVersion`: $EnginePath"
        }

        if (!(Test-Path $RunUAT)) {
            throw "RunUAT.bat not found for UE $EngineVersion`: $RunUAT"
        }

        Write-Step "UE $EngineVersion found: $EnginePath"
    }
}

function Clear-OutputFolders {
    Write-Section "Preparing output folders"

    Remove-Item $BuildRoot -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item $DistRoot -Recurse -Force -ErrorAction SilentlyContinue

    New-Item -ItemType Directory -Path $BuildRoot | Out-Null
    New-Item -ItemType Directory -Path $DistRoot | Out-Null

    Write-Step "Build folder: $BuildRoot"
    Write-Step "Dist folder: $DistRoot"
}

function Build-Plugin {
    param(
        [string]$EngineVersion,
        [string]$EnginePath,
        [object]$PluginInfo
    )

    $RunUAT = Join-Path $EnginePath "Engine\Build\BatchFiles\RunUAT.bat"
    $PackagePath = Join-Path $BuildRoot "UE-$EngineVersion"
    $ZipPath = Join-Path $DistRoot "$ArchiveName-$($PluginInfo.Tag)-UE-$EngineVersion.zip"

    Write-Section "Building $ArchiveName for UE $EngineVersion"

    Write-Step "Engine path:  $EnginePath"
    Write-Step "Package path: $PackagePath"
    Write-Step "Archive path: $ZipPath"

    $UatArgs = @(
        "BuildPlugin",
        "-Plugin=$PluginPath",
        "-Package=$PackagePath",
        "-Rocket",
        "-TargetPlatforms=$TargetPlatforms"
    )

    & $RunUAT @UatArgs 2>&1 | ForEach-Object {
        Write-Host $_
    }

    $ExitCode = $LASTEXITCODE

    if ($ExitCode -ne 0) {
        throw "Build failed for UE $EngineVersion with exit code $ExitCode"
    }

    if (!(Test-Path $PackagePath)) {
        throw "Expected package folder was not created: $PackagePath"
    }

    Compress-Archive -Path (Join-Path $PackagePath "*") -DestinationPath $ZipPath -Force

    if (!(Test-Path $ZipPath)) {
        throw "Failed to create archive: $ZipPath"
    }

    Write-Step "Created archive: $ZipPath"

    return [PSCustomObject]@{
        EngineVersion = $EngineVersion
        EnginePath    = $EnginePath
        PackagePath   = $PackagePath
        ZipPath       = $ZipPath
    }
}

function New-CommitReleaseNotes {
    $PreviousTag = $null

    try {
        $PreviousTag = git describe --tags --abbrev=0 2>$null
    }
    catch {
        $PreviousTag = $null
    }

    if ([string]::IsNullOrWhiteSpace($PreviousTag)) {
        Write-WarningStep "No previous tag found. Using recent commits instead."

        try {
            $RecentCommits = @(git log -10 --pretty=format:"- %s (%h)" 2>$null)
        }
        catch {
            $RecentCommits = @()
        }

        if ($RecentCommits.Count -eq 0) {
            return "- No commit notes found."
        }

        return ($RecentCommits -join "`n")
    }

    Write-Step "Latest existing tag found: $PreviousTag"

    try {
        $CommitNotes = @(git log "$PreviousTag..HEAD" --pretty=format:"- %s (%h)" 2>$null)
    }
    catch {
        $CommitNotes = @()
    }

    if ($CommitNotes.Count -eq 0) {
        Write-WarningStep "No commits found since $PreviousTag. Using recent commits instead."

        try {
            $RecentCommits = @(git log -10 --pretty=format:"- %s (%h)" 2>$null)
        }
        catch {
            $RecentCommits = @()
        }

        if ($RecentCommits.Count -eq 0) {
            return "- No commit notes found."
        }

        return ($RecentCommits -join "`n")
    }

    return ($CommitNotes -join "`n")
}

function New-ReleaseNotes {
    param(
        [object]$PluginInfo,
        [array]$BuildResults
    )

    $CommitNotes = New-CommitReleaseNotes 

    if ([string]::IsNullOrWhiteSpace($CommitNotes)) {
        $CommitNotes = "- No commit notes found."
    }

    return @"
Packaged builds of $($PluginInfo.FriendlyName).

Version: $($PluginInfo.VersionName)

Included builds:
$(($BuildResults | ForEach-Object { "- Unreal Engine $($_.EngineVersion) - $TargetPlatforms" }) -join "`n")

Changes:
$CommitNotes
"@
}

function Publish-GitHubRelease {
    param(
        [object]$PluginInfo,
        [array]$BuildResults
    )

    Write-Section "Publishing GitHub release"

    $ReleaseNotes = New-ReleaseNotes `
        -PluginInfo $PluginInfo `
        -BuildResults $BuildResults

    $Archives = @($BuildResults | ForEach-Object { $_.ZipPath })

    $ReleaseExists = $false

    try {
        $null = gh release view $PluginInfo.Tag 2>$null

        if ($LASTEXITCODE -eq 0) {
            $ReleaseExists = $true
        }
    }
    catch {
        $ReleaseExists = $false
    }

    if ($ReleaseExists) {
        Write-Step "Existing release found. Uploading assets with --clobber..."

        $UploadArgs = @("release", "upload", $PluginInfo.Tag)
        $UploadArgs += $Archives
        $UploadArgs += "--clobber"

        gh @UploadArgs

        if ($LASTEXITCODE -ne 0) {
            throw "Failed to upload assets to existing GitHub release."
        }

        return
    }

    Write-Step "No existing release found. Creating release..."

    $CreateArgs = @("release", "create", $PluginInfo.Tag)
    $CreateArgs += $Archives
    $CreateArgs += @(
        "--title", "$($PluginInfo.FriendlyName) $($PluginInfo.Tag)",
        "--notes", $ReleaseNotes
    )

    if ($PluginInfo.IsPrerelease) {
        $CreateArgs += "--prerelease"
    }

    gh @CreateArgs

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to create GitHub release."
    }
}

function Write-UploadSkipped {
    param([array]$BuildResults)

    Write-Section "GitHub upload skipped"

    Write-WarningStep "Upload disabled via configuration."
    Write-Step "Built archives are available at:"

    foreach ($Result in $BuildResults) {
        Write-Host "  $($Result.ZipPath)"
    }
}

function Write-BuildSummary {
    param(
        [object]$PluginInfo,
        [array]$BuildResults
    )

    Write-Section "Build summary"

    Write-Host "Plugin:        $($PluginInfo.FriendlyName)"
    Write-Host "Version:       $($PluginInfo.VersionName)"
    Write-Host "Release Tag:   $($PluginInfo.Tag)"
    Write-Host "Prerelease:    $($PluginInfo.IsPrerelease)"
    Write-Host "Upload:        $UploadToGitHub"
    Write-Host "Platforms:     $TargetPlatforms"
    Write-Host ""

    foreach ($Result in $BuildResults) {
        Write-Host "UE $($Result.EngineVersion):"
        Write-Host "  $($Result.ZipPath)"
    }

    Write-Host ""
    Write-Host "Done."
}

# ============================================================
# Main
# ============================================================

$PluginInfo = Get-PluginInfo

Write-Section "Starting release build"

Write-Host "Plugin:        $($PluginInfo.FriendlyName)"
Write-Host "Version:       $($PluginInfo.VersionName)"
Write-Host "Release Tag:   $($PluginInfo.Tag)"
Write-Host "Prerelease:    $($PluginInfo.IsPrerelease)"
Write-Host "Upload:        $UploadToGitHub"
Write-Host "Platforms:     $TargetPlatforms"

Test-Prerequisites
Clear-OutputFolders

$BuildResults = @()

foreach ($Engine in $UnrealEngines.GetEnumerator()) {
    $BuildResults += Build-Plugin `
        -EngineVersion $Engine.Key `
        -EnginePath $Engine.Value `
        -PluginInfo $PluginInfo
}

if ($UploadToGitHub) {
    Publish-GitHubRelease `
        -PluginInfo $PluginInfo `
        -BuildResults $BuildResults
}
else {
    Write-UploadSkipped -BuildResults $BuildResults
}

Write-BuildSummary `
    -PluginInfo $PluginInfo `
    -BuildResults $BuildResults