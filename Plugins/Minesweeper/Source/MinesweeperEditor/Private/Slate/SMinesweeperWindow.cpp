// Copyright 2022 Brad Monahan. All Rights Reserved.
#include "Slate/SMinesweeperWindow.h"
#include "MinesweeperEditorModule.h"
#include "MinesweeperStyle.h"
#include "MinesweeperSettings.h"
#include "MinesweeperDifficulty.h"
#include "MinesweeperGame.h"
#include "Slate/SMinesweeper.h"
#include "Slate/SMinesweeperHighScores.h"
#include "Editor.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Images/SImage.h"
#include "ISettingsModule.h"


#define LOCTEXT_NAMESPACE "SMinesweeperWindow"




FMinesweeperDifficulty const SMinesweeperWindow::DefaultDifficulty = FMinesweeperDifficulty::Beginner();


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMinesweeperWindow::Construct(const FArguments& InArgs)
{
	SetCanTick(false);


	Settings = UMinesweeperSettings::Get();


	// main window widget layout
	const int32 difficultyButtonWidth = 100;

	ChildSlot
	[
		SNew(SWidgetSwitcher)
		.WidgetIndex_Lambda([&]() { return ActiveMainPanel; })

		// NEW GAME PANEL
		+ SWidgetSwitcher::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Left).VAlign(VAlign_Top)
			.Padding(0.0f, 0.0f, 0.0f, 0.0f)
			[
				// SWITCH WINDOW MODE BUTTON
				SNew(SBox).WidthOverride(8).HeightOverride(60)
				.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					SNew(SButton)
					.ContentPadding(0)
					.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
					.ToolTipText(LOCTEXT("ToggleWindowModeTooltip", "Toggle between standalone and docking tab window modes."))
					//.ButtonStyle(FEditorStyle::Get(), "FlatButton")
					.OnClicked(this, &SMinesweeperWindow::OnSwitchWindowModeClick)
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)

				// TITLE AREA
				+ SVerticalBox::Slot().AutoHeight()
				.HAlign(HAlign_Center).VAlign(VAlign_Center)
				.Padding(0.0f, 3.0f, 0.0f, 0.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth()
					.HAlign(HAlign_Center).VAlign(VAlign_Center)
					[
						SNew(SImage).Image(FMinesweeperStyle::GetBrush("Mine"))
					]
					+ SHorizontalBox::Slot().AutoWidth()
					.HAlign(HAlign_Center).VAlign(VAlign_Center)
					.Padding(22.0f, 0.0f, 18.0f, 0.0f)
					[
						SNew(SBox).WidthOverride(230)
						.HAlign(HAlign_Left).VAlign(VAlign_Center)
						[
							SNew(SRichTextBlock)
							.DecoratorStyleSet(&FMinesweeperStyle::Get())
							.TextStyle(FMinesweeperStyle::Get(), "Text.Title")
							.Text(this, &SMinesweeperWindow::GetTitleText)
						]
					]
					+ SHorizontalBox::Slot().AutoWidth()
					.HAlign(HAlign_Center).VAlign(VAlign_Center)
					[
						SNew(SImage).Image(FMinesweeperStyle::GetBrush("Mine"))
					]
				]

				// NEW GAME SETUP / HIGH SCORES BUTTONS
				+ SVerticalBox::Slot().AutoHeight()
				.HAlign(HAlign_Fill).VAlign(VAlign_Center)
				.Padding(5.0f, 10.0f, 5.0f, 0.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill).VAlign(VAlign_Center)
					.Padding(0, 0, 1, 0)
					[
						SNew(SBorder).BorderImage(FMinesweeperStyle::GetBrush("RoundedPanel"))
						[
							SNew(SCheckBox)
							.HAlign(HAlign_Center)
							.Style(FEditorStyle::Get(), "ToggleButtonCheckbox")
							.BorderBackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f))
							.IsChecked_Lambda([&]() { return ActiveGameSetupPanel == 0 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
							.OnCheckStateChanged_Lambda([&](ECheckBoxState NewCheckState) { if (NewCheckState == ECheckBoxState::Checked) ActiveGameSetupPanel = 0; })
							[
								SNew(STextBlock)
								.Margin(FMargin(0, 2))
								.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
								.Text(LOCTEXT("PlayLabel", "Play"))
							]
						]
					]
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill).VAlign(VAlign_Center)
					.Padding(1, 0, 0, 0)
					[
						SNew(SBorder).BorderImage(FMinesweeperStyle::GetBrush("RoundedPanel"))
						[
							SNew(SCheckBox)
							.HAlign(HAlign_Center)
							.Style(FEditorStyle::Get(), "ToggleButtonCheckbox")
							.IsChecked_Lambda([&]() { return ActiveGameSetupPanel == 1 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
							.OnCheckStateChanged_Lambda([&](ECheckBoxState NewCheckState) { if (NewCheckState == ECheckBoxState::Checked) ActiveGameSetupPanel = 1; })
							[
								SNew(STextBlock)
								.Margin(FMargin(0, 2))
								.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
								.Text(LOCTEXT("HighScoresLabel", "High Scores"))
							]
						]
					]
				]
				
				+ SVerticalBox::Slot().AutoHeight()
				.HAlign(HAlign_Fill).VAlign(VAlign_Center)
				.Padding(5.0f, 5.0f, 5.0f, 0.0f)
				[
					SNew(SBox)
					.WidthOverride(360).HeightOverride(270)
					[
						SNew(SWidgetSwitcher)
						.WidgetIndex_Lambda([&]() { return ActiveGameSetupPanel; })
					
						// NEW GAME SETTINGS
						+ SWidgetSwitcher::Slot()//.AutoWidth()
						.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
						.Padding(5.0f, 5.0f, 5.0f, 0.0f)
						[
							SNew(SBorder)
							//.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
							.BorderImage(FMinesweeperStyle::GetBrush("RoundedPanel"))
							.HAlign(HAlign_Fill).VAlign(VAlign_Center)
							.Padding(10.0f)
							[
								SNew(SVerticalBox)

								// GRID SETTINGS PANEL
								+ SVerticalBox::Slot().AutoHeight()
								.HAlign(HAlign_Fill).VAlign(VAlign_Center)
								.Padding(5.0f, 0.0f, 5.0f, 0.0f)
								[
									SNew(SBox).WidthOverride(300)
									[
										SNew(SVerticalBox)

										// USERNAME
										+ SVerticalBox::Slot().AutoHeight()
										.HAlign(HAlign_Fill).VAlign(VAlign_Center)
										.Padding(10.0f, 0.0f, 10.0f, 10.0f)
										[
											SNew(SHorizontalBox)
											+ SHorizontalBox::Slot().FillWidth(0.5f).VAlign(VAlign_Center)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
												.Text(LOCTEXT("PlayerNameLabel", "Player Name:"))
											]
											+ SHorizontalBox::Slot().FillWidth(0.5f).HAlign(HAlign_Fill).VAlign(VAlign_Center)
											[
												SAssignNew(NameTextBox, SEditableTextBox)
												//.Text(FText::FromString(Settings->LastPlayerName))
												.Text(this, &SMinesweeperWindow::GetPlayerName)
												.OnTextChanged(this, &SMinesweeperWindow::OnPlayerNameChanged)
												.OnTextCommitted(this, &SMinesweeperWindow::OnPlayerNameCommitted)
											]
										]

										// NEW GAME GRID WIDTH
										+ SVerticalBox::Slot().AutoHeight()
										.HAlign(HAlign_Fill).VAlign(VAlign_Center)
										.Padding(10.0f, 0.0f, 10.0f, 5.0f)
										[
											SNew(SHorizontalBox)
											+ SHorizontalBox::Slot().FillWidth(0.5f).VAlign(VAlign_Center)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
												.Text(LOCTEXT("NewGridWidthLabel", "Width:"))
											]
											+ SHorizontalBox::Slot().FillWidth(0.5f).HAlign(HAlign_Fill).VAlign(VAlign_Center)
											[
												SNew(SNumericEntryBox<int32>)
												.AllowSpin(true)
												.MinSliderValue(UMinesweeperGame::MinGridSize).MaxSliderValue(UMinesweeperGame::MaxGridSize)
												.MinValue(UMinesweeperGame::MinGridSize).MaxValue(UMinesweeperGame::MaxGridSize)
												.Value_Lambda([&] { return Settings->LastDifficulty.Width; })
												.OnValueChanged_Lambda([&](int32 InNewValue)
													{
														Settings->LastDifficulty.Width = InNewValue;
														MaxMineCount = FMath::FloorToInt32(Settings->LastDifficulty.TotalCells() * 0.25f);
													})
											]
										]

										// NEW GAME GRID HEIGHT
										+ SVerticalBox::Slot().AutoHeight()
										.HAlign(HAlign_Fill).VAlign(VAlign_Center)
										.Padding(10.0f, 0.0f, 10.0f, 5.0f)
										[
											SNew(SHorizontalBox)
											+ SHorizontalBox::Slot().FillWidth(0.5f).VAlign(VAlign_Center)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
												.Text(LOCTEXT("NewGridHeightLabel", "Height:"))
											]
											+ SHorizontalBox::Slot().FillWidth(0.5f).HAlign(HAlign_Fill).VAlign(VAlign_Center)
											[
												SNew(SNumericEntryBox<int32>)
												.AllowSpin(true)
												.MinSliderValue(UMinesweeperGame::MinGridSize).MaxSliderValue(UMinesweeperGame::MaxGridSize)
												.MinValue(UMinesweeperGame::MinGridSize).MaxValue(UMinesweeperGame::MaxGridSize)
												.Value_Lambda([&] { return Settings->LastDifficulty.Height; })
												.OnValueChanged_Lambda([&](int32 InNewValue)
													{
														Settings->LastDifficulty.Height = InNewValue;
														MaxMineCount = FMath::FloorToInt32(Settings->LastDifficulty.TotalCells() * 0.25f);
													})
											]
										]

										// NEW GAME GRID MINE COUNT
										+ SVerticalBox::Slot().AutoHeight()
										.HAlign(HAlign_Fill).VAlign(VAlign_Center)
										.Padding(10.0f, 0.0f, 10.0f, 5.0f)
										[
											SNew(SHorizontalBox)
											+ SHorizontalBox::Slot().FillWidth(0.5f).VAlign(VAlign_Center)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
												.Text(LOCTEXT("NewGridMineCountLabel", "Mines:"))
											]
											+ SHorizontalBox::Slot().FillWidth(0.5f).HAlign(HAlign_Fill).VAlign(VAlign_Center)
											[
												SNew(SNumericEntryBox<int32>)
												.AllowSpin(true)
												.MinSliderValue(UMinesweeperGame::MinMineCount)
												.MaxSliderValue_Lambda([&]() { return MaxMineCount; })
												.MinValue(UMinesweeperGame::MinMineCount)
												.MaxValue_Lambda([&]() { return MaxMineCount; })
												.Value_Lambda([&] { return Settings->LastDifficulty.MineCount; })
												.OnValueChanged_Lambda([&](int32 InNewValue) { Settings->LastDifficulty.MineCount = InNewValue; })
											]
										]
									]
								]

								// DIFFICULTY SETTING
								+ SVerticalBox::Slot().AutoHeight()
								.HAlign(HAlign_Fill).VAlign(VAlign_Center)
								.Padding(0.0f, 5.0f, 0.0f, 10.0f)
								[
									SNew(SHorizontalBox)
									+ SHorizontalBox::Slot().FillWidth(1.0f)
									.HAlign(HAlign_Center).VAlign(VAlign_Center)
									.Padding(1.0f, 0.0f, 1.0f, 0.0f)
									[
										SNew(SBox).WidthOverride(difficultyButtonWidth)
										.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
										[
											SNew(SButton)
											.HAlign(HAlign_Center).VAlign(VAlign_Center)
											.ButtonColorAndOpacity(this, &SMinesweeperWindow::GetDifficultyButtonColor, 0)
											.OnClicked(this, &SMinesweeperWindow::OnDifficultyClick, 0)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Small")
												.Text(LOCTEXT("BeginnerDifficultyLabel", "Beginner"))
											]
										]
									]
									+ SHorizontalBox::Slot().FillWidth(1.0f)
									.HAlign(HAlign_Center).VAlign(VAlign_Center)
									.Padding(1.0f, 0.0f, 1.0f, 0.0f)
									[
										SNew(SBox).WidthOverride(difficultyButtonWidth)
										.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
										[
											SNew(SButton)
											.HAlign(HAlign_Center).VAlign(VAlign_Center)
											.ButtonColorAndOpacity(this, &SMinesweeperWindow::GetDifficultyButtonColor, 1)
											.OnClicked(this, &SMinesweeperWindow::OnDifficultyClick, 1)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Small")
												.Text(LOCTEXT("IntermediateDifficultyLabel", "Intermediate"))
											]
										]
									]
									+ SHorizontalBox::Slot().FillWidth(1.0f)
									.HAlign(HAlign_Center).VAlign(VAlign_Center)
									.Padding(1.0f, 0.0f, 1.0f, 0.0f)
									[
										SNew(SBox).WidthOverride(difficultyButtonWidth)
										.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
										[
											SNew(SButton)
											.HAlign(HAlign_Center).VAlign(VAlign_Center)
											.ButtonColorAndOpacity(this, &SMinesweeperWindow::GetDifficultyButtonColor, 2)
											.OnClicked(this, &SMinesweeperWindow::OnDifficultyClick, 2)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Small")
												.Text(LOCTEXT("ExpertDifficultyLabel", "Expert"))
											]
										]
									]
								]

								// START NEW GAME BUTTON
								+ SVerticalBox::Slot().AutoHeight()
								.HAlign(HAlign_Center).VAlign(VAlign_Center)
								.Padding(0.0f, 10.0f, 0.0f, 0.0f)
								[
									SNew(SHorizontalBox)
									+ SHorizontalBox::Slot().AutoWidth().Padding(0.0f, 0.0f, 10.0f, 0.0f)
									[
										SNew(SBox).WidthOverride(30)
										.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
										[
											SNew(SButton).HAlign(HAlign_Fill)
											.HAlign(HAlign_Center).VAlign(VAlign_Center)
											.ToolTipText(LOCTEXT("SettingsButtonTooltip", "Open the Minesweeper editor settings window."))
											.OnClicked(this, &SMinesweeperWindow::OnSettingsClick)
											[
												SNew(SImage).Image(FMinesweeperStyle::GetBrush("Settings")).DesiredSizeOverride(FVector2D(16.0f))
											]
										]
									]
									+ SHorizontalBox::Slot().AutoWidth()
									[
										SNew(SBox).WidthOverride(180)
										.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
										[
											SNew(SButton).HAlign(HAlign_Fill)
											.HAlign(HAlign_Center).VAlign(VAlign_Center)
											.OnClicked(this, &SMinesweeperWindow::OnStartNewGameClick)
											[
												SNew(STextBlock)
												.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
												.Text(LOCTEXT("StartNewGameLabel", "Start New Game"))
											]
										]
									]
								]

								// CONTINUE GAME BUTTON
								+ SVerticalBox::Slot().AutoHeight()
								.HAlign(HAlign_Center).VAlign(VAlign_Center)
								.Padding(0.0f, 10.0f, 0.0f, 0.0f)
								[
									SNew(SBox).WidthOverride(180)
									.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
									.Visibility_Lambda([&]() { return GameWidget->IsGameActive() ? EVisibility::Visible : EVisibility::Collapsed; })
									[
										SNew(SButton).HAlign(HAlign_Fill)
										.HAlign(HAlign_Center).VAlign(VAlign_Center)
										.OnClicked(this, &SMinesweeperWindow::OnContinueGameClick)
										[
											SNew(STextBlock)
											.TextStyle(FMinesweeperStyle::Get(), "Text.Normal")
											.Text(LOCTEXT("ContinueGameLabel", "Continue Game"))
										]
									]
								]
							]
						]

						// HIGH SCORES LIST
						+ SWidgetSwitcher::Slot()
						.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
						.Padding(5.0f, 5.0f, 5.0f, 0.0f)
						[
							SNew(SBorder)
							.BorderImage(FMinesweeperStyle::GetBrush("RoundedPanel"))
							.HAlign(HAlign_Fill).VAlign(VAlign_Center)
							.Padding(10.0f)
							[
								SAssignNew(HighScoresList, SMinesweeperHighScores)
								.HighScores(&Settings->HighScores)
							]
						]
					]
				]
			]
		]

		// GAME PANEL
		+ SWidgetSwitcher::Slot()
		[
			SAssignNew(GameWidget, SMinesweeper)
			.CellDrawSize(Settings->CellDrawSize)
			.UseGridCanvas(Settings->UseGridCanvas)
			.PlayerName(this, &SMinesweeperWindow::GetPlayerName)
			.OnGameSetupClick(this, &SMinesweeperWindow::GotoNewGamePanel)
			.OnGameOver(this, &SMinesweeperWindow::OnGameOverCallback)
		]
	];


	//GameWidget->GetGame()->OnGameOvered.AddRaw(this, &SMinesweeperWindow::OnGameOverCallback);


	ActiveTimerHandle = RegisterActiveTimer(0.07f, FWidgetActiveTimerDelegate::CreateSP(this, &SMinesweeperWindow::UpdateGameTick));
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


EActiveTimerReturnType SMinesweeperWindow::UpdateGameTick(double InCurrentTime, float InDeltaTime)
{
	if (++TitleTextAnimIndex > 600) TitleTextAnimIndex = 0;

	return EActiveTimerReturnType::Continue;
}


FString SanitizePlayerName(const FString InName)
{
	FString outName = "";

	const auto nameCharArray = InName.GetCharArray();
	const auto validCharArray = FString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_").GetCharArray();

	FString badChars = "";

	for (int32 i = 0; i < nameCharArray.Num(); ++i)
	{
		if (i >= 20) break; // limit to 20 characters
		if (validCharArray.Contains(nameCharArray[i]))
		{
			outName += nameCharArray[i];
		}
	}

	return outName;
}

FText SMinesweeperWindow::GetPlayerName() const
{
	return FText::FromString(Settings->LastPlayerName);
}

void SMinesweeperWindow::OnPlayerNameChanged(const FText& NewText)
{
	Settings->LastPlayerName = SanitizePlayerName(NewText.ToString());
	NameTextBox->SetText(FText::FromString(Settings->LastPlayerName));
}

void SMinesweeperWindow::OnPlayerNameCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	Settings->LastPlayerName = SanitizePlayerName(NewText.ToString());
}


FText SMinesweeperWindow::GetTitleText() const
{
	const FText minesweeperText = FMinesweeperEditorModule::GetMinesweeperLabel();
	FString text = minesweeperText.ToString();
	int32 textLen = text.Len();

	const FString titleTag = "<Text.Title>";
	const FString endTag = "</>";

	if (TitleTextAnimIndex < textLen)
	{
		const FString redTag = "<Text.Title.Red>";

		// create separate sections of the string to apply the styling tags to
		FString startText = TitleTextAnimIndex == 0 ? "" : text.Left(TitleTextAnimIndex);
		FString redText = text.Chr(text[TitleTextAnimIndex]);
		FString endText = TitleTextAnimIndex == textLen - 1 ? "" : text.Right(textLen - (TitleTextAnimIndex + 1));

		// add the styling tags to each string
		if (startText != "") startText = titleTag + startText + endTag;
		redText = redTag + redText + endTag;
		if (endText != "") endText = titleTag + endText + endTag;

		return FText::FromString(startText + redText + endText);
	}
	
	return FText::Format(LOCTEXT("TitleTextLabel", "{0}{1}{2}"), FText::FromString(titleTag), minesweeperText, FText::FromString(endTag));
}

FSlateColor SMinesweeperWindow::GetDifficultyButtonColor(const int32 InDifficultyLevel) const
{
	const FLinearColor transparentColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	const FLinearColor selectedColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.5f);

	switch (InDifficultyLevel)
	{
	case 0: return Settings->LastDifficulty.IsBeginner() ? selectedColor : transparentColor;
	case 1: return Settings->LastDifficulty.IsIntermediate() ? selectedColor : transparentColor;
	case 2: return Settings->LastDifficulty.IsExpert() ? selectedColor : transparentColor;
	}

	return transparentColor;
}


FReply SMinesweeperWindow::OnDifficultyClick(const int32 InDifficultyLevel)
{
	switch (InDifficultyLevel)
	{
	case 0: Settings->LastDifficulty = FMinesweeperDifficulty::Beginner(); break;
	case 1: Settings->LastDifficulty = FMinesweeperDifficulty::Intermediate(); break;
	case 2: Settings->LastDifficulty = FMinesweeperDifficulty::Expert(); break;
	}
	return FReply::Handled();
}

FReply SMinesweeperWindow::OnStartNewGameClick()
{
	// Attempt to hide the game until the creation of slots and widgets. Trying to hide view of stretching controls during short lag spike.
	SetVisibility(EVisibility::Hidden);

	GEditor->GetTimerManager()->SetTimerForNextTick([&]()
		{
			GameWidget->StartNewGame(Settings->LastDifficulty);
		});
	

	ActiveMainPanel = 1;

	// Show the game again after all slots and widgets have been created
	SetVisibility(EVisibility::SelfHitTestInvisible);

	return FReply::Handled();
}

FReply SMinesweeperWindow::OnContinueGameClick()
{
	GameWidget->ContinueGame();

	ActiveMainPanel = 1;

	return FReply::Handled();
}

FReply SMinesweeperWindow::OnNewGameClick()
{
	GameWidget->PauseGame();

	ActiveMainPanel = 0;

	return FReply::Handled();
}

FReply SMinesweeperWindow::OnRestartGameClick()
{
	GameWidget->RestartGame();

	return FReply::Handled();
}

FReply SMinesweeperWindow::OnSwitchWindowModeClick()
{
	FMinesweeperEditorModule::Get().ToggleMinesweeperWindowMode();

	return FReply::Handled();
}

FReply SMinesweeperWindow::OnGotoNewGamePanel()
{
	GotoNewGamePanel();

	return FReply::Handled();
}

void SMinesweeperWindow::GotoNewGamePanel()
{
	GameWidget->PauseGame();

	ActiveMainPanel = 0;
}

FReply SMinesweeperWindow::OnSettingsClick()
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings")
		.ShowViewer(Settings->GetContainerName(), Settings->GetCategoryName(), Settings->GetSectionName());

	return FReply::Handled();
}


int32 SMinesweeperWindow::OnGameOverCallback(const bool InWon, const float InTime, const int32 InClicks)
{
	LastHighScoreRank = -1;

	if (InWon && GameWidget->GetGame()->GetDifficulty().IsExpert())
	{
		// calculate high score
		// less time is higher score, less clicks is higher score
		int32 score = FMath::FloorToInt32(((float)MaxScore / InTime) + ((float)MaxScore / (float)InClicks));

		// check for new high score
		for (int32 rank = 0; rank < Settings->HighScores.Num(); ++rank)
		{
			if (score > Settings->HighScores[rank].Score)
			{
				LastHighScoreRank = rank;
				break;
			}
		}

		// insert the new high score into the list and remove the last rank if we have a new high score
		if (LastHighScoreRank > -1)
		{
			Settings->HighScores.Insert(FMinesweeperHighScore(Settings->LastPlayerName, score, InTime, InClicks), LastHighScoreRank);
			Settings->HighScores.RemoveAt(Settings->HighScores.Num() - 1);

			HighScoresList->SetHighScores(&Settings->HighScores);
		}
	}

	return LastHighScoreRank;
}




#undef LOCTEXT_NAMESPACE
