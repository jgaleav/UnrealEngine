// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "LoadingProfilerCommands.h"

#include "DesktopPlatformModule.h"
#include "EditorStyleSet.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

// Insights
#include "Insights/InsightsManager.h"
#include "Insights/LoadingProfiler/LoadingProfilerManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "FLoadingProfilerCommands"

////////////////////////////////////////////////////////////////////////////////////////////////////
// FLoadingProfilerMenuBuilder
////////////////////////////////////////////////////////////////////////////////////////////////////

void FLoadingProfilerMenuBuilder::AddMenuEntry(FMenuBuilder& MenuBuilder, const TSharedPtr< FUICommandInfo >& UICommandInfo, const FUIAction& UIAction)
{
	MenuBuilder.AddMenuEntry
	(
		UICommandInfo->GetLabel(),
		UICommandInfo->GetDescription(),
		UICommandInfo->GetIcon(),
		UIAction,
		NAME_None,
		UICommandInfo->GetUserInterfaceType()
	);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// FLoadingProfilerCommands
////////////////////////////////////////////////////////////////////////////////////////////////////

FLoadingProfilerCommands::FLoadingProfilerCommands()
	: TCommands<FLoadingProfilerCommands>(
		TEXT("LoadingProfilerCommand"), // Context name for fast lookup
		NSLOCTEXT("Contexts", "LoadingProfilerCommand", "Asset Loading Insights"), // Localized context name for displaying
		NAME_None, // Parent
		FEditorStyle::GetStyleSetName() // Icon Style Set
	)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// UI_COMMAND takes long for the compiler to optimize
PRAGMA_DISABLE_OPTIMIZATION
void FLoadingProfilerCommands::RegisterCommands()
{
	UI_COMMAND(ToggleTimingViewVisibility, "Timing", "Toggles the visibility of the main Timing view", EUserInterfaceActionType::ToggleButton, FInputChord(EModifierKey::Control, EKeys::T));
	UI_COMMAND(ToggleEventAggregationTreeViewVisibility, "Event Aggregation", "Toggles the visibility of the Event Aggregation table/tree view", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ToggleObjectTypeAggregationTreeViewVisibility, "Object Type Aggregation", "Toggles the visibility of the Object Type Aggregation table/tree view", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(TogglePackageDetailsTreeViewVisibility, "Package Details", "Toggles the visibility of the Package Details table/tree view", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ToggleExportDetailsTreeViewVisibility, "Export Details", "Toggles the visibility of the Export Details table/tree view", EUserInterfaceActionType::ToggleButton, FInputChord());
}
PRAGMA_ENABLE_OPTIMIZATION

////////////////////////////////////////////////////////////////////////////////////////////////////
// Toggle Commands
////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_TOGGLE_COMMAND(CmdName, IsEnabled, SetIsEnabled) \
	\
	void FLoadingProfilerActionManager::Map_##CmdName##_Global()\
	{\
		This->CommandList->MapAction(This->GetCommands().CmdName, CmdName##_Custom());\
	}\
	\
	const FUIAction FLoadingProfilerActionManager::CmdName##_Custom() \
	{\
		FUIAction UIAction;\
		UIAction.ExecuteAction = FExecuteAction::CreateRaw(this, &FLoadingProfilerActionManager::CmdName##_Execute);\
		UIAction.CanExecuteAction = FCanExecuteAction::CreateRaw(this, &FLoadingProfilerActionManager::CmdName##_CanExecute);\
		UIAction.GetActionCheckState = FGetActionCheckState::CreateRaw(this, &FLoadingProfilerActionManager::CmdName##_GetCheckState);\
		return UIAction;\
	}\
	\
	void FLoadingProfilerActionManager::CmdName##_Execute()\
	{\
		const bool b##IsEnabled = !This->IsEnabled();\
		This->SetIsEnabled(b##IsEnabled);\
	}\
	\
	bool FLoadingProfilerActionManager::CmdName##_CanExecute() const\
	{\
		return FInsightsManager::Get()->GetSession().IsValid();\
	}\
	\
	ECheckBoxState FLoadingProfilerActionManager::CmdName##_GetCheckState() const\
	{\
		const bool b##IsEnabled = This->IsEnabled();\
		return b##IsEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;\
	}

IMPLEMENT_TOGGLE_COMMAND(ToggleTimingViewVisibility, IsTimingViewVisible, ShowHideTimingView)
IMPLEMENT_TOGGLE_COMMAND(ToggleEventAggregationTreeViewVisibility, IsEventAggregationTreeViewVisible, ShowHideEventAggregationTreeView)
IMPLEMENT_TOGGLE_COMMAND(ToggleObjectTypeAggregationTreeViewVisibility, IsObjectTypeAggregationTreeViewVisible, ShowHideObjectTypeAggregationTreeView)
IMPLEMENT_TOGGLE_COMMAND(TogglePackageDetailsTreeViewVisibility, IsPackageDetailsTreeViewVisible, ShowHidePackageDetailsTreeView)
IMPLEMENT_TOGGLE_COMMAND(ToggleExportDetailsTreeViewVisibility, IsExportDetailsTreeViewVisible, ShowHideExportDetailsTreeView)

#undef IMPLEMENT_TOGGLE_COMMAND

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE