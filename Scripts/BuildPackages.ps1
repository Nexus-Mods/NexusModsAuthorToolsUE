param(
    [switch]$NoClean
)

$ErrorActionPreference = "Stop"

. "$PSScriptRoot\BuildConfig.ps1"
. "$PSScriptRoot\BuildHelpers.ps1"

function Build-PluginPackage {
    param(
        [string]$EngineVersion,
        [string]$EnginePath,
        [object]$PluginInfo
    )

    $RunUAT = Join-Path $EnginePath "Engine\Build\BatchFiles\RunUAT.bat"
    $PackagePath = Join-Path $script:BuildRoot "UE-$EngineVersion"
    $ZipPath = Join-Path $script:DistRoot "$ArchiveName-$($PluginInfo.Tag)-UE-$EngineVersion.zip"

    Write-Section "Building $ArchiveName for UE $EngineVersion"

    Write-Step "Engine path:  $EnginePath"
    Write-Step "Package path: $PackagePath"
    Write-Step "Archive path: $ZipPath"

    $UatArgs = @(
        "BuildPlugin",
        "-Plugin=$script:PluginPath",
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

$LogPath = Start-BuildToolsTranscript -Name "Build"

try {
    $PluginInfo = Get-PluginInfo

    Write-Section "Starting package build"
    Write-Host "Plugin:        $($PluginInfo.FriendlyName)"
    Write-Host "Version:       $($PluginInfo.VersionName)"
    Write-Host "Release Tag:   $($PluginInfo.Tag)"
    Write-Host "Prerelease:    $($PluginInfo.IsPrerelease)"
    Write-Host "Platforms:     $TargetPlatforms"
    Write-Host "Log:           $LogPath"

    Test-BuildPrerequisites

    if ($NoClean) {
        Initialize-OutputFolders
        Write-WarningStep "NoClean enabled. Existing package output will not be removed."
    }
    else {
        Clear-BuildOutputFolders
    }

    $BuildResults = @()

    foreach ($Engine in $UnrealEngines.GetEnumerator()) {
        $BuildResults += Build-PluginPackage `
            -EngineVersion $Engine.Key `
            -EnginePath $Engine.Value `
            -PluginInfo $PluginInfo
    }

    Write-Section "Build summary"
    Write-Host "Plugin:        $($PluginInfo.FriendlyName)"
    Write-Host "Version:       $($PluginInfo.VersionName)"
    Write-Host "Release Tag:   $($PluginInfo.Tag)"
    Write-Host "Prerelease:    $($PluginInfo.IsPrerelease)"
    Write-Host "Platforms:     $TargetPlatforms"
    Write-Host "Log:           $LogPath"
    Write-Host ""

    foreach ($Result in $BuildResults) {
        Write-Host "UE $($Result.EngineVersion):"
        Write-Host "  $($Result.ZipPath)"
    }

    Write-Host ""
    Write-Host "Build completed successfully."
}
finally {
    Stop-BuildToolsTranscript
    Remove-OldLogs -Name "Build" -Keep $MaxLogFilesPerType
}
