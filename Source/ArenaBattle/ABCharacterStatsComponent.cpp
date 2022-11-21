// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatsComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatsComponent::UABCharacterStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	// 이 컴포넌트는 매 Tick마다 수행할 로직이 없음
	PrimaryComponentTick.bCanEverTick = false;

	// IntializedComponent를 호출하기 위해서는 true로 설정해줘야 함
	bWantsInitializeComponent = true;

	Level = 1;
}


// Called when the game starts
void UABCharacterStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SetNewLevel(Level);
}

void UABCharacterStatsComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UABCharacterStatsComponent::SetNewLevel(int32 NewLevel)
{
	// 월드로부터 게임 인스턴스를 얻어옴
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != ABGameInstance);

	// 레벨에 따른 정보
	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
	}
	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void UABCharacterStatsComponent::SetDamage(float NewDamage)
{
	ABCHECK(nullptr != CurrentStatData)

	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.f, CurrentStatData->MaxHP));
}

void UABCharacterStatsComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHpChange.Broadcast();

	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnHpIsZero.Broadcast();
	}
}

float UABCharacterStatsComponent::GetHPRatio()
{
	ABCHECK(nullptr != CurrentStatData, 0.f);

	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.f : (CurrentHP / CurrentStatData->MaxHP);
}

float UABCharacterStatsComponent::GetAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}

// Called every frame
void UABCharacterStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

