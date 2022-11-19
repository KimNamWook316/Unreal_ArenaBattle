// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
	IsDead = false;

	// 애니메이션 몽타주 에셋을 받아온다.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage'"));

	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TryGetPawnOwner : 폰이 유효한지 검사하는 함수
	// 입력 -> 로직 -> 애니메이션 업데이트 순서로 진행되므로, 이 함수가 실행될 때는 폰이 유효하지 않을 수 있다.
	auto Pawn = TryGetPawnOwner();
	if (!IsValid(Pawn))
		return;

	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			// 캐릭터 무브먼트 컴포넌트에서 폰이 공중에 떠 있는 상태인지 반환
			IsInAir = Character->GetCharacterMovement()->IsFalling();
		}
	}

}

void UABAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead)
	// 애님몽타주를 플레이한다.
	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 newSection)
{
	ABCHECK(!IsDead)
	ABCHECK(Montage_IsPlaying(AttackMontage));

	// 섹션이름, 애님몽타주 인자로, 설정된 섹션을 재생한다.
	Montage_JumpToSection(GetAttackMontageSectionName(newSection), AttackMontage);
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	// 멀티캐스트 타입 델리게이트에서, 등록된 모든 함수를 호출하는 함수
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
