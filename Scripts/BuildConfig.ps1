# Nexus Mods Author Tools build configuration.
# Paths are resolved relative to the repository root, not this Scripts folder.

$PluginFileName = "NexusModsAuthorTools.uplugin"
$ArchiveName = "NexusModsAuthorTools"
$TargetPlatforms = "Win64"

# Set to $true if you want newly-created GitHub releases to be drafts by default.
$CreateDraftGitHubRelease = $false

# Git remote used for publishing release tags.
$GitRemoteName = "origin"

# Maximum number of timestamped log files to keep for each log type.
# For example, with a value of 5, the tool keeps the newest 5 Build logs and newest 5 Release logs.
$MaxLogFilesPerType = 1

# Different unreal engine versions we want to build the plugin for. 
$UnrealEngines = [ordered]@{
    ## UE 4.0+
    # "4.0"  = ""
    # "4.1"  = ""
    # "4.2"  = ""
    # "4.3"  = ""
    # "4.4"  = ""
    # "4.5"  = ""
    # "4.6"  = ""
    # "4.7"  = ""
    # "4.8"  = ""
    # "4.9"  = ""
    # "4.10" = ""
    # "4.11" = ""
    # "4.12" = ""
    # "4.13" = ""
    # "4.14" = ""
    # "4.15" = ""
    # "4.16" = ""
    # "4.17" = ""
    # "4.18" = ""
    # "4.19" = ""
    # "4.20" = ""
    # "4.21" = ""
    # "4.22" = ""
    # "4.23" = ""
    # "4.24" = ""
    # "4.25" = ""
    "4.26" = "S:\UE_4.26"
    # "4.26-Chaos" = ""
    "4.27" = "S:\UE_4.27"
    # "4.27-Chaos" = ""

    ## UE 5.0+
    "5.0" = "S:\UE_5.0"
    "5.1" = "S:\UE_5.1"
    "5.2" = "S:\UE_5.2"
    "5.3" = "S:\UE_5.3"
    "5.4" = "S:\UE_5.4"
    "5.5" = "S:\UE_5.5"
    "5.6" = "S:\UE_5.6"
    "5.7" = "S:\UE_5.7"
    "5.8" = "S:\UE_5.8"
}

