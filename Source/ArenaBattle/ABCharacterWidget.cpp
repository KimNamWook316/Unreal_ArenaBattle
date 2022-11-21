// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatsComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatsComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;
	NewCharacterStat->OnHpChange.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget();
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
