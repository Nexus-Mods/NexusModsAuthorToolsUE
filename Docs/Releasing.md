
# Release Tooling

The repository includes optional PowerShell scripts used to automate building and releasing the plugin across multiple Unreal Engine versions.

## Available Actions

1. Build Packages
2. Release Packages
3. Build then Release
4. Exit

## Output Directories

| Folder | Purpose |
|---------|---------|
| `Output/Build` | Temporary packaged plugin builds |
| `Output/Dist` | Final distributable ZIP archives |
| `Output/Logs` | Build and release logs |

## Configuration

Build behaviour can be configured through:

```text
Scripts/BuildConfig.ps1
```

This includes:

- Unreal Engine installation paths
- Target platforms
- Plugin information
- Archive naming
- GitHub release behaviour
- Build output options

