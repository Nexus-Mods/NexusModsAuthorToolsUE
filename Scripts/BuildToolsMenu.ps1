$ErrorActionPreference = "Stop"

. "$PSScriptRoot\BuildConfig.ps1"
. "$PSScriptRoot\BuildHelpers.ps1"

function Show-Menu {
    Clear-Host

    $PluginSummary = $null
    try {
        $PluginSummary = Get-PluginInfo
    }
    catch {
        $PluginSummary = $null
    }

    Write-Host "============================================================"
    Write-Host " Nexus Mods Author Tools Build Utility"
    Write-Host "============================================================"

    if ($PluginSummary -ne $null) {
        Write-Host "Plugin:      $($PluginSummary.FriendlyName)"
        Write-Host "Version:     $($PluginSummary.VersionName)"
        Write-Host "Release Tag: $($PluginSummary.Tag)"
    }
    else {
        Write-Host "Plugin:      $PluginFileName"
        Write-Host "Version:     unavailable until plugin file is readable"
    }

    Write-Host "Output:      $script:OutputRoot"
    Write-Host ""
    Write-Host "What would you like to do?"
    Write-Host ""
    Write-Host "1: Build Packages"
    Write-Host "2: Release Packages"
    Write-Host "3: Build then Release"
    Write-Host "4: Exit"
    Write-Host ""
}

function Invoke-Script {
    param([Parameter(Mandatory = $true)][string]$ScriptName)

    $ScriptPath = Join-Path $PSScriptRoot $ScriptName

    if (!(Test-Path $ScriptPath)) {
        throw "Script not found: $ScriptPath"
    }

    & $ScriptPath

    if (!$?) {
        throw "$ScriptName failed."
    }
}

while ($true) {
    Show-Menu
    $Selection = Read-Host "Selection"

    switch ($Selection) {
        "1" {
            Invoke-Script "BuildPackages.ps1"
            Write-Host ""
            Read-Host "Build complete. Press Enter to return to the menu"
        }
        "2" {
            Invoke-Script "ReleasePackages.ps1"
            Write-Host ""
            Read-Host "Release flow complete. Press Enter to return to the menu"
        }
        "3" {
            Invoke-Script "BuildPackages.ps1"
            Invoke-Script "ReleasePackages.ps1"
            Write-Host ""
            Read-Host "Build and release flow complete. Press Enter to return to the menu"
        }
        "4" {
            exit 0
        }
        default {
            Write-Host ""
            Write-WarningStep "Invalid selection. Choose 1, 2, 3, or 4."
            Read-Host "Press Enter to try again"
        }
    }
}
