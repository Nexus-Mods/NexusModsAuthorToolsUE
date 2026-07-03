# Nexus Mods Author Tool Help

The Nexus Mods Author Tool helps Unreal Engine mod authors create, package, and upload mod files to Nexus Mods without leaving the editor.

---

## Quick Start

1. Enter the game's Nexus Mods **Game Domain**.
   - Example: `gothic1remake`
   - This is the short name used in the game's Nexus Mods URL.

2. Enter your **Nexus Mods API Key**.
   - The API key is stored securely using Unreal's platform credential storage.
   - Use the visibility toggle if you need to check or replace the key.

3. Click **Save Config**.

4. Create or add a mod entry.
   - Use **Add Mod** if you already have mod files ready.
   - Use **Use Mod Template** if you want the tool to generate starter mod content.

5. Edit the mod entry, select your files, then upload.

---

## Main Window

### Config

The **Config** section stores the shared settings used by the tool.

- **Game Domain**: The Nexus Mods game domain used for creating mod pages and uploading files.
- **API Key**: Your Nexus Mods API key, used to authenticate uploads.
- **Save Config**: Saves the current game domain and API key.

### Mods

The **Mods** section lists the mod upload entries you have configured for the project.

Each mod entry can be:

- **Uploaded** to Nexus Mods.
- **Edited** to change its display name, upload code, and selected files.
- **Deleted** from the tool's local configuration.

Deleting a mod entry only removes it from this tool. It does not delete anything from Nexus Mods or from your project files.

---

## Creating a Mod Page

Click **Create Mod Page** to open the Nexus Mods website for the configured game.

This is useful when you need to create the mod page before uploading files from the tool.

You must enter a valid **Game Domain** before this button can be used.

---

## Using Mod Templates

Click **Use Mod Template** to create starter mod content inside your Unreal project.

The tool supports two template sources:

- **Built-in templates** supplied by the plugin.
- **Project templates** found under `/Game/ModTemplates`.

### Built-in UE4SS Logic Mod Template

The built-in **UE4SS Logic Mod** template creates a `ModActor` blueprint under `/Game/Mods/{ModName}`.

The generated blueprint includes standard UE4SS-style variables and events:

- `ModAuthor`
- `ModVersion`
- `ModDescription`
- `ModButtons`
- `PrintToModLoader`
- `PreBeginPlay`
- `PostBeginPlay`
- `ModMenuButtonPressed`

### Project Folder Templates

Project folder templates are copied from `/Game/ModTemplates` into a new destination under `/Game`.

The template system skips template preview image assets named:

- `TemplateImage`
- `T_TemplateImage`

These images are used as template previews and are not copied into the created mod folder.

---

## Adding or Editing a Mod Entry

Use **Add Mod** or **Edit** to configure a mod upload entry.

### Display Name

The display name is the local name shown in the tool's mod list and upload screen.

### Upload Code

The upload code identifies the Nexus Mods file target for this mod entry.

This value is treated as sensitive and can be hidden or shown with the visibility toggle.

### Mod Files

You can select either:

- A pre-built `.zip` archive, or
- Loose Unreal mod files that the tool will package into a `.zip` archive before upload.

Supported loose file types:

- `.pak`
- `.utoc`
- `.ucas`

Supported archive type:

- `.zip`

Do not mix a pre-built archive and loose mod files in the same upload. Clear one selection before uploading.

---

## Uploading a Mod

Click **Upload** on a mod entry to open the upload screen.

Before uploading, review the upload options.

### Version

The version is sent to Nexus Mods when creating the uploaded file version.

Use standard semantic-style versions where possible, for example:

- `0.1.0`
- `1.0.0`
- `1.2.3`

### Auto-Increment Version

When enabled, the tool increments the patch version before upload.

Example:

- `1.2.3` becomes `1.2.4`

### Category

Choose the Nexus Mods file category for the upload:

- **Main**
- **Optional**
- **Misc**

### Archive Existing Version

When enabled, the existing file version on Nexus Mods is archived before the new version is created.

### Primary Mod Manager Download

Marks this upload as the primary mod manager download.

### Allow Mod Manager Download

Allows the uploaded file to be downloaded through mod managers.

### Show Requirements Pop-up

Shows the requirements pop-up before users download the file.

### Description

The description is sent with the uploaded file version.

---

## Automatic Archive Creation

If you select loose `.pak`, `.utoc`, or `.ucas` files instead of a `.zip`, the tool creates a zip archive automatically before upload.

The generated archive is saved under the project's `Saved/ArchivedMods` directory.

The archive name is based on the mod display name and upload version.

---

## Upload Progress and Logs

When an upload starts, the tool validates the configuration, prepares the archive, uploads the file in parts, finalises the upload, and creates the Nexus Mods file version.

The upload log shows each major step, including validation errors, archive creation, upload progress, and completion status.

If an upload fails, check the log for the first error message. Common causes include:

- Missing API key.
- Missing upload code.
- Missing game domain.
- Missing version.
- Selected archive file does not exist.
- Both a zip archive and loose mod files are selected.
- Nexus Mods API returned an error.

---

## Recommended Workflow

1. Create the mod page on Nexus Mods.
2. Copy the upload code for the file target.
3. Save your game domain and API key in the tool.
4. Add a mod entry.
5. Enter the display name and upload code.
6. Select a `.zip` archive or loose Unreal mod files.
7. Save the mod entry.
8. Upload the mod.
9. Check the upload log for confirmation.

---

## Notes

- The tool stores mod entries locally for the Unreal project.
- The API key and upload codes are stored separately through platform credential storage.
- The Nexus Mods logo button in the title bar opens the Nexus Mods website.
- The close button closes the tool window.
- Navigation is disabled while an upload is in progress.
