// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "ABGameInstance.generated.h"

// FTableRowBase : CSV포맷과 대응되는 언리얼 구조체
// UStruct와 Generate_body매크로 정의 필요
USTRUCT(BlueprintType)
struct FABCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterData() :
		Level(1),
		MaxHP(100.f),
		Attack(10.f),
		DropExp(10),
		NextExp(30)
	{
	}

	// 첫 번째 Column Name은 자동적으로 언리얼에서 Key 생성함

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 NextExp;
};

/**
 *
 */
UCLASS()
class ARENABATTLE_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UABGameInstance();

	virtual void Init() override;
	FABCharacterData* GetABCharacterData(int32 Level);

private:
	// 캐릭터 데이터 테이블 컴포넌트 
	UPROPERTY()
	class UDataTable* ABCharacterTable;
};

