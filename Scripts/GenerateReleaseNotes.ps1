param(
    [Parameter(Mandatory = $true)][string]$OutputPath
)

$ErrorActionPreference = "Stop"

. "$PSScriptRoot\BuildConfig.ps1"
. "$PSScriptRoot\BuildHelpers.ps1"

function New-CommitReleaseNotes {
    $PreviousTag = $null

    try {
        $PreviousTag = git -C $script:RepoRoot describe --tags --abbrev=0 2>$null
    }
    catch {
        $PreviousTag = $null
    }

    if ([string]::IsNullOrWhiteSpace($PreviousTag)) {
        Write-WarningStep "No previous tag found. Using recent commits instead."

        try {
            $RecentCommits = @(git -C $script:RepoRoot log -10 --pretty=format:"- %s (%h)" 2>$null)
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
        $CommitNotes = @(git -C $script:RepoRoot log "$PreviousTag..HEAD" --pretty=format:"- %s (%h)" 2>$null)
    }
    catch {
        $CommitNotes = @()
    }

    if ($CommitNotes.Count -eq 0) {
        Write-WarningStep "No commits found since $PreviousTag. Using recent commits instead."

        try {
            $RecentCommits = @(git -C $script:RepoRoot log -10 --pretty=format:"- %s (%h)" 2>$null)
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

$PluginInfo = Get-PluginInfo
$Archives = Get-ReleaseArchives -PluginInfo $PluginInfo
$CommitNotes = New-CommitReleaseNotes

if ([string]::IsNullOrWhiteSpace($CommitNotes)) {
    $CommitNotes = "- No commit notes found."
}

$IncludedBuilds = if ($Archives.Count -gt 0) {
    ($Archives | ForEach-Object {
        if ($_.BaseName -match "UE-(.+)$") {
            "- Unreal Engine $($Matches[1]) - $TargetPlatforms"
        }
        else {
            "- $($_.Name)"
        }
    }) -join "`n"
}
else {
    "- No package archives found."
}

$ReleaseNotes = @"
Packaged builds of $($PluginInfo.FriendlyName).

Version: $($PluginInfo.VersionName)

Included builds:
$IncludedBuilds

Changes:
$CommitNotes
"@

$OutputDir = Split-Path -Parent $OutputPath
if (![string]::IsNullOrWhiteSpace($OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

Set-Content -Path $OutputPath -Value $ReleaseNotes -Encoding UTF8
Write-Step "Release notes written to: $OutputPath"
