# Using Nexus Mods Author Tools

> [!NOTE]
> This guide provides an overview of the plugin's primary features and typical authoring workflow.

---

## Getting Started

Once installed, Nexus Mods Author Tools becomes available directly within the Unreal Editor.

The plugin is designed to streamline common mod authoring tasks by bringing them together into a single integrated workflow.

---

## Plugin Overview

The plugin currently provides tools for:

- Creating new mod projects
- Managing existing mods
- Packaging mod archives
- Uploading mods to Nexus Mods
- Viewing documentation and release notes

---

## Typical Workflow

A common workflow looks like this:

1. Create or open a mod project.
2. Configure the mod metadata.
3. Build the mod.
4. Package the archive.
5. Upload directly to Nexus Mods.
6. Publish your release.

Each of these steps can be completed from within the Unreal Editor.


---

## Opening The Plugin

After the plugin has been added, your Unreal Engine interface should show a shiny new Nexus Mods button.

If you can see this button, then the plugin has been added properly!

![PluginButton](/.github/Images/PluginButton.png)

---

## Navigating the UI

Before you start using the plugin properly, you should fill in some basic details to enable all features of the plugin. 

![MainUI](/.github/Images/MainUI.png)

**Game Domain:** visit the https://www.nexusmods.com/games/ page for your desired game. Copy the game identifier, also known as the game `slug` into the `Game Domain` section within the plugin. This tells the plugin which page to open when you click the `Create Mod Page` button. 

![GameDomain](/.github/Images/GameDomain.png)


**API Key:** Visit the https://www.nexusmods.com/settings/api-keys page, scroll all the way to the bottom, and request/copy your personal API Key. This is the `API Key` that you would paste into the plugin settings. This allows the plugin to upload mods to your account - after a page has been created for the mod. Note: You need to be logged in to access this page. 

After you have entered the details into these two sections, remember to click the `Save Config` button. This should cause the `Create Mod Page` button to become available. 

---

## Adding Your Mod

Click on the `Add Mod` button to open the Mod Details window. 

From here, you are able to alter the `Display Name`, `Upload Code`, and select the `Mod Files` that should be packaged when you later upload the mod. 

![ModDetails](/.github/Images/ModDetails.png)


**Display Name:** This name is used for both the name of the mod within the plugin window, and as the name given to your packaged mod files when uploading. 

**Upload Code:** This can be obtained from a previously created mod page on the main Nexus Mods website. After you have uploaded your first file this will be available on the files section of your mod page by clicking the `API Info` button. 

![UploadCode](/.github/Images/UploadCode.png)

Yes, this does mean you have to have created a mod page and uploaded a file before you can automate uploads for it. This is a limitation we are aware of, and are working on providing solutions for. 

**Mod Files:** Clicking the `Add Files` button will allow you to select either a prepackaged zip archive, or pak/utoc/ucas files from your computer. These are the files that will be automatically packaged into a zip archive and uploaded when you later use the `Upload` button. 

Typically, these would be files from your games `~mods` folder, as you would want to validate their functionality before uploading. 


---

## Using Mod Templates

By clicking the `Use Mod Template` button, you can create files based on existing mod templates. 

Mod Templates allow you to create the internal Unreal Engine files based on previously defined templates. By default, the only template that will be available is for creating a `UE4SS Logic Mod`. If there are common things you need to create when making a new mod, then you can easily add your own templates (detailed later).

For now we will create a new mod using the built-in UE4SS Logic Mod template. 

![Templates](/.github/Images/Templates.png)

Select the `UE4SS Logic Mod` template from the list, and you will be prompted to enter the name for your new mod. The name you enter will be used when creating the mod folder. eg: `/Game/Mods/{ModName}`.

After you are happy with the name, click `Create Mod`, and the plugin will populate the desired folder with a `ModActor` blueprint - used by UE4SS for Logic Mods. The `ModActor` blueprint will contain the various events avaiable specifically from the UE4SS modloading system, such as the `PrintToModLoader` event, and some variables used for mod metadata within UE4SS. 

![Templates2](/.github/Images/Templates2.png)


## Creating New Templates

Within your projects `Content` folder (inside Unreal Engine). If you create a new folder named `ModTemplates`, and then another subfolder within that named `Template01` (or whatever you want your template to be named), then the plugin will automatically consider the files within that folder as a custom template. 

![NewTemplates](/.github/Images/NewTemplates.png)

When this template is selected from the plugin interface's `Use Mod Template` button, all files (except `TemplateImage` or `T_TemplateImage`) will be copied into your desired location. All referenced paths will remain correct to the new location, similar to the `Advanced Copy` function Unreal Engine has by default. 

**TemplateImage:** This asset is what defines the image shown for your template within the plugin interface. It can be named either `T_TemplateImage` or `TemplateImage`. This assets should be a square image as the plugin will render it at 32x32 pixels. This asset **will not** be copied over when you create a mod from this template, and is purely used for the plugin interface. 

**NOTE:** Any `Data Asset` of the type `Primary Asset Label` will be renamed to whatever you named your mod after copying to the new location. Eg, if you named your mod `TheBestThingEver`, and used a template that contained a `Primary Asset Label`, the copied "PAL" would be named `DA_TheBestThingEver`. This is done because no two "PAL" files can have the same name without Unreal Engine showing warning messages. 

---

## Uploading to Nexus Mods

After you have added a mod to the plugin, you are able to click the `Upload` button, which will attempt to package the files you have selected in the `Mod Details` section, and use your `Upload Code` and `API Key` to authenticate with the Nexus Mods servers, and automatically package and upload your mod files.  

![ModUpload](/.github/Images/ModUpload.png)

You are able to select the version, file category, file description, and some other options from the upload screen. Once you are satisfied, press the `Start Upload` button for your mod to be automatically uploaded. 


---

## Packaging Mods

Coming Soon...



---

## Best Practices

- Test packaged builds before uploading!!
- Keep changelogs (file descriptions) meaningful.
- Use proper version identifiers (eg: 4.2.0, 6.7, etc).


---

## Related Documentation

- 🚀 Installation
- 🛠️ Building From Source
- 📦 Release Tools