// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
	IsDead = false;

	// �ִϸ��̼� ��Ÿ�� ������ �޾ƿ´�.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage'"));

	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TryGetPawnOwner : ���� ��ȿ���� �˻��ϴ� �Լ�
	// �Է� -> ���� -> �ִϸ��̼� ������Ʈ ������ ����ǹǷ�, �� �Լ��� ����� ���� ���� ��ȿ���� ���� �� �ִ�.
	auto Pawn = TryGetPawnOwner();
	if (!IsValid(Pawn))
		return;

	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			// ĳ���� �����Ʈ ������Ʈ���� ���� ���߿� �� �ִ� �������� ��ȯ
			IsInAir = Character->GetCharacterMovement()->IsFalling();
		}
	}

}

void UABAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead)
	// �ִԸ�Ÿ�ָ� �÷����Ѵ�.
	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 newSection)
{
	ABCHECK(!IsDead)
	ABCHECK(Montage_IsPlaying(AttackMontage));

	// �����̸�, �ִԸ�Ÿ�� ���ڷ�, ������ ������ ����Ѵ�.
	Montage_JumpToSection(GetAttackMontageSectionName(newSection), AttackMontage);
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	// ��Ƽĳ��Ʈ Ÿ�� ��������Ʈ����, ��ϵ� ��� �Լ��� ȣ���ϴ� �Լ�
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}
