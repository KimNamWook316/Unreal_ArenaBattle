// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "ABWeapon.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));

	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	SetControlMode(EControlMode::DIABLO);

	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;

	// ĳ���� �����Ʈ ������Ʈ�� ���� �� 
	GetCharacterMovement()->JumpZVelocity = 800.f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 200.f;
	AttackRadius = 50.f;

 // 	FName WeaponSocket(TEXT("hand_rSocket"));
 //
 //	auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
 //
 //	if (nullptr != CurWeapon)
 //	{
 //		CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
 //	}

 //	if (GetMesh()->DoesSocketExist(WeaponSocket))
 //	{
 //		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
 //		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("SkeletalMesh'/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight'"));
 //		if (SK_WEAPON.Succeeded())
 //		{
 //			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
 //		}
 //
 //		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
 //	}
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;
	
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		// ������ ���� ĳ���ͷκ��� ������ �Ÿ�
		// SpringArm->TargetArmLength = 450.f;
		// SpringArm->SetRelativeRotation(FRotator::ZeroRotator);

		ArmLengthTo = 450.f;

		// Controller�� ������� ȸ���Ѵ�.
		SpringArm->bUsePawnControlRotation = true;

		// �θ��� Pitch, Yaw, Roll ȸ������ ���󰣴�.
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;

		// ī�޶� �浹ü�� ������ ī�޶� �����Ѵ�.
		SpringArm->bDoCollisionTest = true;

		// ĳ���Ͱ� ��Ʈ�ѷ��� ȸ���� ���� ȸ������ �ʵ��� �Ѵ�.
		// ī�޶� ȸ���ؾ� �ϱ� ����
		bUseControllerRotationYaw = false;

		// ĳ���� �����Ʈ ������Ʈ�� ���ǵ� : ĳ���Ͱ� ���� �̵��ϴ� �������� ȸ�������ش�.
		// RotationRate�� ȸ�� �ӵ� ����
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

		// ��Ʈ�ѷ� ȸ���� ����Ű�� �������� �÷��̾ �ε巴�� ȸ���ϰ� ������ִ� �Ӽ�
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		break;
	case EControlMode::DIABLO:
		// SpringArm->TargetArmLength = 800.f;
		// SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
		ArmLengthTo = 800.f;
		ArmRotationTo = FRotator(-45.f, 0.f, 0.f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	}
}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ī�޶������ �Ÿ��� �Է� ��� ��ȯ�� ���� �����Ѵ�.
	// FInterpTo : float ���� �Լ� (����, ��ǥ, deltaTime, �ӵ�)
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength,
		ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo,
			DeltaTime, ArmRotationSpeed));

		// �̵� ���� ������ ũ�Ⱑ 0 �̻��� ��� (W,A,S,D �Է��� ���� ���)
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			// FRotationMatrix::MakeFrom : ȸ���� ������ �� ���͸� �����ϰ�, �׷κ��� ������ �� ���͸� ���� ȸ�� ����� �����Ѵ�.
			// XY(DirectX XZ)������� �̵� ���Ͱ� �����ǰ�, �� ������ X(��) �������� �ؼ� ȸ�� ����� �����ϹǷ�
			// Z������ ȸ���Ѵ�.
			// SetControlRotation�� ���� �ش� �������� ȸ���Ѵ�.
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	case EControlMode::GTA:
		break;
	default:
		break;
	}
}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// �ִ��ν��Ͻ�
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);

	// ��Ÿ�� ����� ������ ȣ��Ǵ� ��������Ʈ�� ���ε�
	// Dynamic ��������Ʈ : �������Ʈ�� �����Ǵ� ��������Ʈ 
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	// ���� �Լ� [] : ĸ�� (this�̹Ƿ� �� �ν��Ͻ��� ��� ����),()����,->��ȯ��,{}����
	ABAnim->OnNextAttackCheck.AddLambda([this]()->void
		{
			ABLOG(Warning, TEXT("OnNextAttackCheck"));
			CanNextCombo = false;

			if (IsComboInputOn)
			{
				AttackStartComboState();
				ABAnim->JumpToAttackMontageSection(CurrentCombo);
			}
		}
	);

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EInputEvent Enum : ��ư�� ���ȴ���/���������� ���� ����
	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &AABCharacter::Turn);
}

void AABCharacter::UpDown(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		// �̵� ���� ������ X���� -1 ~ 1���̷� �޴´�.
		DirectionToMove.X = NewAxisValue;
		break;
	case EControlMode::GTA:
		// GetControlRotation() : ��Ʈ�ѷ� ȸ���� ������
		// FRotationMatrix() : ȸ�� ��� ����
		// GetUnitAxis(EAxis::X) : X�� ���� �޾ƿ�
		// ���������� ��Ʈ�ѷ��� ���� �ִ� ������ �޾ƿ´�. (SpringArm�� bUsePawnControlRotation ���� ���������� ȸ���� ��Ʈ�ѷ��� ��ġ�������Ƿ�)
		// �÷��̾ ���� �ִ� ���������� Input Axis ����ŭ �̵���Ų��.
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	default:
		break;
	}
}

void AABCharacter::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		// �̵� ���� ������ Y���� -1 ~ 1���̷� �޴´�.
		DirectionToMove.Y = NewAxisValue;
		break;
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	default:
		break;
	}
}

void AABCharacter::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		// ���콺 �� �Է¿� ���� �𸮾� Y�� ȸ��
		AddControllerPitchInput(NewAxisValue);
		break;
	default:
		break;
	}
}

void AABCharacter::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		// ���콺 �� �Է¿� ���� �𸮾� X�� ȸ��
		AddControllerYawInput(NewAxisValue);
		break;
	default:
		break;
	}
}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	case EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
		break;
	default:
		break;
	}
}

void AABCharacter::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::AttackCheck()
{
	// �浹 ������ ��ƿ��� ����ü
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	// �÷��̾��� ���� 2M ������ ����
	// Start���� End���� ���� "���" �浹���� �Ѵ�.
	// ĸ����� �浹�����̶�� �����ϸ� ��
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.f,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.f),
		Params);

#if ENABLE_DRAW_DEBUG
	// ����� �����
	// ĸ���� ���� �� ��ġ
	FVector TraceVec = GetActorForwardVector() * AttackRange;

	// �÷��̾� ��ġ + (���� �� ��ġ / 2) = ĸ���� �߾�
	FVector Center = GetActorLocation() + TraceVec * 0.5f;

	// ĸ���� ���� ����
	float HalfHeight = AttackRange * 0.5f + AttackRadius;

	// ���� ���·� �׷��� �� 
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);
#endif

	if (bResult)
	{
		if (IsValid(HitResult.GetActor()))
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

			// ���Ϳ��� ����� �ش�.
			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(50.0f, DamageEvent, GetController(), this);
		}
	}

}

// �𸮾� ������ �����ӿ�ũ���� �����ϴ� �Լ�
// AActor�� ���ǵ� �Լ��� �������̵� �ؼ� ���
// 3��° ���ڷ� AController�� �޴� ���� : ������ ��ü�� "�÷��̾� �ڽ�"�̱� ����
float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ABLOG(Warning, TEXT("Actor : %s took Damage %f"), *GetName(), FinalDamage);

	if (FinalDamage > 0.f)
	{
		ABAnim->SetDeadAnim();

		// �浹���� ����.
		SetActorEnableCollision(false);
	}

	return FinalDamage;
}

