$ErrorActionPreference = "Stop"

$script:ScriptsDir = $PSScriptRoot
$script:RepoRoot = Resolve-Path (Join-Path $script:ScriptsDir "..")
$script:PluginPath = Join-Path $script:RepoRoot $PluginFileName
$script:OutputRoot = Join-Path $script:RepoRoot "Output"
$script:BuildRoot = Join-Path $script:OutputRoot "Build"
$script:DistRoot = Join-Path $script:OutputRoot "Dist"
$script:LogsRoot = Join-Path $script:OutputRoot "Logs"

function Initialize-OutputFolders {
    New-Item -ItemType Directory -Path $script:OutputRoot -Force | Out-Null
    New-Item -ItemType Directory -Path $script:BuildRoot -Force | Out-Null
    New-Item -ItemType Directory -Path $script:DistRoot -Force | Out-Null
    New-Item -ItemType Directory -Path $script:LogsRoot -Force | Out-Null
}

function Start-BuildToolsTranscript {
    param([Parameter(Mandatory = $true)][string]$Name)

    Initialize-OutputFolders

    $Timestamp = Get-Date -Format "yyyy-MM-dd--HH-mm-ss"
    $LogPath = Join-Path $script:LogsRoot "$Name-$Timestamp.log"

    Start-Transcript -Path $LogPath -Force | Out-Null
    return $LogPath
}

function Stop-BuildToolsTranscript {
    try {
        Stop-Transcript | Out-Null
    }
    catch {
        # No active transcript; ignore.
    }
}

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
    if (!(Test-Path $script:PluginPath)) {
        throw "Could not find plugin file: $script:PluginPath"
    }

    $PluginJson = Get-Content $script:PluginPath -Raw | ConvertFrom-Json

    if ([string]::IsNullOrWhiteSpace($PluginJson.VersionName)) {
        throw "VersionName is missing from $script:PluginPath"
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

function Test-BuildPrerequisites {
    Write-Section "Checking build prerequisites"

    if (!(Test-Path $script:RepoRoot)) {
        throw "Repository root does not exist: $script:RepoRoot"
    }

    if (!(Test-Path $script:PluginPath)) {
        throw "Plugin file does not exist: $script:PluginPath"
    }

    Write-Step "Plugin file found: $script:PluginPath"

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

function Test-ReleasePrerequisites {
    Write-Section "Checking release prerequisites"

    if (!(Test-CommandAvailable "git")) {
        throw "Git is not installed or not available in PATH."
    }

    if (!(Test-CommandAvailable "gh")) {
        throw "GitHub CLI is not installed or not available in PATH."
    }

    git -C $script:RepoRoot rev-parse --is-inside-work-tree *> $null
    if ($LASTEXITCODE -ne 0) {
        throw "Repository root does not appear to be inside a git repository: $script:RepoRoot"
    }

    gh auth status *> $null
    if ($LASTEXITCODE -ne 0) {
        throw "GitHub CLI is not authenticated. Run: gh auth login"
    }

    Write-Step "Git found."
    Write-Step "GitHub CLI found and authenticated."
}

function Clear-BuildOutputFolders {
    Write-Section "Preparing build output folders"

    Remove-Item $script:BuildRoot -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item $script:DistRoot -Recurse -Force -ErrorAction SilentlyContinue

    New-Item -ItemType Directory -Path $script:BuildRoot -Force | Out-Null
    New-Item -ItemType Directory -Path $script:DistRoot -Force | Out-Null
    New-Item -ItemType Directory -Path $script:LogsRoot -Force | Out-Null

    Write-Step "Build folder: $script:BuildRoot"
    Write-Step "Dist folder: $script:DistRoot"
    Write-Step "Logs folder: $script:LogsRoot"
}

function Get-ReleaseArchives {
    param([object]$PluginInfo)

    $Pattern = "$ArchiveName-$($PluginInfo.Tag)-UE-*.zip"
    return @(Get-ChildItem -Path $script:DistRoot -Filter $Pattern -File -ErrorAction SilentlyContinue | Sort-Object Name)
}

function Confirm-Action {
    param([Parameter(Mandatory = $true)][string]$Message)

    $Answer = Read-Host "$Message [Y/N]"
    return $Answer -match "^(y|yes)$"
}

function Remove-OldLogs {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [int]$Keep = 5
    )

    Initialize-OutputFolders

    if ($Keep -le 0) {
        Write-WarningStep "Log retention is disabled because Keep is set to $Keep. No logs will be deleted."
        return
    }

    $Pattern = "$Name-*.log"
    $Logs = @(Get-ChildItem -Path $script:LogsRoot -Filter $Pattern -File -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending)

    if ($Logs.Count -le $Keep) {
        Write-Step "Log retention: keeping $($Logs.Count) $Name log(s). No cleanup needed."
        return
    }

    $LogsToRemove = @($Logs | Select-Object -Skip $Keep)

    foreach ($Log in $LogsToRemove) {
        Remove-Item -Path $Log.FullName -Force
    }

    Write-Step "Log retention: removed $($LogsToRemove.Count) old $Name log(s), keeping newest $Keep."
}
