// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// 멀티캐스트 델리게이트를 선언하는 매크로
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();

	// 애님 인스턴스에서 틱마다 호출되는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 newSection);

public:
	// 멀티캐스트 델리게이트 타입 변수
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

private:
	// 블루프린트와 연관된 함수는 UFUNCTION매크로로 정의해줘야 한다.
	// 애님 몽타주에서 설정한 노티파이가 AnimNotify_노티파이명 함수를 호출한다.
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int32 Section);

private:
	// Blueprint에서 접근하는 방법에 대한 키워드 -> BlueprintReadOnly, BlueprintReadWrite
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, META = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, META = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, META = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;
};
