// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"

/**
 * 
 */

// UUsetWidget : 위젯 블루프린트의 Default 기반 클래스
UCLASS()
class ARENABATTLE_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindCharacterStat(class UABCharacterStatsComponent* NewCharacterStat);

protected:
	// UI 생성될 때 Call -> Player Controller의 BeginPlay() 시점
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

private:
	TWeakObjectPtr<class UABCharacterStatsComponent> CurrentCharacterStat;

	UPROPERTY()
	class UProgressBar* HPProgressBar;
};
