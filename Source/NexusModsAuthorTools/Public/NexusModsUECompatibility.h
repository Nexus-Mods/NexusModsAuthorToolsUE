#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"
#include "Runtime/Launch/Resources/Version.h"

#define UNREAL_ENGINE_VERSION_AT_LEAST(Major, Minor) \
	((ENGINE_MAJOR_VERSION > (Major)) || (ENGINE_MAJOR_VERSION == (Major) && ENGINE_MINOR_VERSION >= (Minor)))

#define UNREAL_ENGINE_VERSION_BELOW(Major, Minor) \
	(!UNREAL_ENGINE_VERSION_AT_LEAST(Major, Minor))

#define UNREAL_ENGINE_VERSION_EXACTLY(Major, Minor) \
	((ENGINE_MAJOR_VERSION == (Major)) && (ENGINE_MINOR_VERSION == (Minor)))

#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

namespace NexusModsUECompatibility {
	inline const ISlateStyle& GetEditorStyle() {
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
		return FAppStyle::Get();
#else
		return FEditorStyle::Get();
#endif
	}

	inline const FSlateBrush* GetEditorBrush(const FName BrushName) {
		return GetEditorStyle().GetBrush(BrushName);
	}

	inline FSlateFontInfo GetEditorFontStyle(const FName FontStyleName) {
		return GetEditorStyle().GetFontStyle(FontStyleName);
	}

	inline bool SupportsEditableTextBoxTextStyle() {
		return UNREAL_ENGINE_VERSION_AT_LEAST(5, 1);
	}

	inline bool SupportsAdvancedCopySeverity() {
		return UNREAL_ENGINE_VERSION_AT_LEAST(5, 1);
	}
}
