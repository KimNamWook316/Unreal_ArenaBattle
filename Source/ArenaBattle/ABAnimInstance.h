// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// ��Ƽĳ��Ʈ ��������Ʈ�� �����ϴ� ��ũ��
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

	// �ִ� �ν��Ͻ����� ƽ���� ȣ��Ǵ� �Լ�
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 newSection);

public:
	// ��Ƽĳ��Ʈ ��������Ʈ Ÿ�� ����
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

private:
	// �������Ʈ�� ������ �Լ��� UFUNCTION��ũ�η� ��������� �Ѵ�.
	// �ִ� ��Ÿ�ֿ��� ������ ��Ƽ���̰� AnimNotify_��Ƽ���̸� �Լ��� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int32 Section);

private:
	// Blueprint���� �����ϴ� ����� ���� Ű���� -> BlueprintReadOnly, BlueprintReadWrite
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, META = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, META = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, META = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;
};
