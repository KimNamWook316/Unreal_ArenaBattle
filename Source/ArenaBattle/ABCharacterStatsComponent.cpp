// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatsComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatsComponent::UABCharacterStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	// �� ������Ʈ�� �� Tick���� ������ ������ ����
	PrimaryComponentTick.bCanEverTick = false;

	// IntializedComponent�� ȣ���ϱ� ���ؼ��� true�� ��������� ��
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
	// ����κ��� ���� �ν��Ͻ��� ����
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != ABGameInstance);

	// ������ ���� ����
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

