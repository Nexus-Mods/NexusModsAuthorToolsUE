// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FNexusModsStyle {

public:
	static void Initialize();
	static void Shutdown();
	/** reloads textures used by slate renderer */
	static void ReloadTextures();
	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();
	static FName GetStyleSetName();

	static const FLinearColor NexusOrange;
	static const FLinearColor NexusOrangeLight;

	static const FMargin SectionHeaderPadding;
	static const FMargin FormRowLabelPadding;
	static const FMargin FormRowValuePadding;
	static const FMargin InlineControlPadding;
	static const FMargin ButtonGroupPadding;
	static const FMargin ContentPadding;
	static const FMargin ModRowPadding;
	static const FMargin DividerPadding;
	static const FMargin TextBoxPadding;
	static const float DescriptionTextBoxHeight;

private:
	static TSharedRef< class FSlateStyleSet > Create();

private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;

};
