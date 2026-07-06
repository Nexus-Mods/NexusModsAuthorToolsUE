$ErrorActionPreference = "Stop"

. "$PSScriptRoot\BuildConfig.ps1"
. "$PSScriptRoot\BuildHelpers.ps1"

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

function Convert-ArchivesToBuildResults {
    param(
        [Parameter(Mandatory = $true)][array]$Archives
    )

    return @($Archives | ForEach-Object {
        $EngineVersion = $_.BaseName

        if ($_.BaseName -match "UE-(.+)$") {
            $EngineVersion = $Matches[1]
        }

        [PSCustomObject]@{
            EngineVersion = $EngineVersion
            ZipPath       = $_.FullName
        }
    })
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

function Write-ReleaseSummary {
    param(
        [object]$PluginInfo,
        [array]$BuildResults,
        [string]$LogPath
    )

    Write-Section "Release summary"

    Write-Host "Plugin:        $($PluginInfo.FriendlyName)"
    Write-Host "Version:       $($PluginInfo.VersionName)"
    Write-Host "Release Tag:   $($PluginInfo.Tag)"
    Write-Host "Prerelease:    $($PluginInfo.IsPrerelease)"
    Write-Host "Archives:      $($BuildResults.Count)"
    Write-Host "Log:           $LogPath"
    Write-Host ""

    foreach ($Result in $BuildResults) {
        Write-Host "UE $($Result.EngineVersion):"
        Write-Host "  $($Result.ZipPath)"
    }

    Write-Host ""
    Write-Host "Done."
}

$LogPath = Start-BuildToolsTranscript -Name "Release"

try {
    $PluginInfo = Get-PluginInfo

    Write-Section "Starting package release"

    Write-Host "Plugin:        $($PluginInfo.FriendlyName)"
    Write-Host "Version:       $($PluginInfo.VersionName)"
    Write-Host "Release Tag:   $($PluginInfo.Tag)"
    Write-Host "Prerelease:    $($PluginInfo.IsPrerelease)"
    Write-Host "Dist folder:   $script:DistRoot"
    Write-Host "Log:           $LogPath"

    Test-ReleasePrerequisites

    $Archives = Get-ReleaseArchives -PluginInfo $PluginInfo

    if ($Archives.Count -eq 0) {
        throw "No package archives found in $script:DistRoot for tag $($PluginInfo.Tag). Run Build Packages first."
    }

    Write-Section "Package archives found"
    foreach ($Archive in $Archives) {
        Write-Host "  $($Archive.FullName)"
    }

    $BuildResults = Convert-ArchivesToBuildResults -Archives $Archives

    Publish-GitHubRelease `
        -PluginInfo $PluginInfo `
        -BuildResults $BuildResults

    Write-ReleaseSummary `
        -PluginInfo $PluginInfo `
        -BuildResults $BuildResults `
        -LogPath $LogPath
}
finally {
    Stop-BuildToolsTranscript
    Remove-OldLogs -Name "Release" -Keep $MaxLogFilesPerType
}
