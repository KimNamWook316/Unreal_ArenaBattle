// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatsComponent.generated.h"

// UI, Character등의 클래스에서 바인딩하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnHpIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHpChangeDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	void SetHP(float NewHP);
	float GetAttack();
	float GetHPRatio();

	FOnHpIsZeroDelegate OnHpIsZero;
	FOnHpChangeDelegate OnHpChange;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// Transeint 키워드 : 직렬화 Save 과정에서 제외함
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
};
