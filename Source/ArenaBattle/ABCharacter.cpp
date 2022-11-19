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

	// 캐릭터 무브먼트 컴포넌트의 점프 힘 
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
		// 스프링 암이 캐릭터로부터 떨어진 거리
		// SpringArm->TargetArmLength = 450.f;
		// SpringArm->SetRelativeRotation(FRotator::ZeroRotator);

		ArmLengthTo = 450.f;

		// Controller를 기반으로 회전한다.
		SpringArm->bUsePawnControlRotation = true;

		// 부모의 Pitch, Yaw, Roll 회전값을 따라간다.
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;

		// 카메라가 충돌체에 닿으면 카메라를 줌인한다.
		SpringArm->bDoCollisionTest = true;

		// 캐릭터가 컨트롤러의 회전에 따라 회전하지 않도록 한다.
		// 카메라만 회전해야 하기 때문
		bUseControllerRotationYaw = false;

		// 캐릭터 무브먼트 컴포넌트에 정의됨 : 캐릭터가 현재 이동하는 방향으로 회전시켜준다.
		// RotationRate로 회전 속도 지정
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

		// 컨트롤러 회전이 가리키는 방향으로 플레이어가 부드럽게 회전하게 만들어주는 속성
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

	// 카메라까지의 거리를 입력 모드 전환에 따라 보간한다.
	// FInterpTo : float 보간 함수 (현재, 목표, deltaTime, 속도)
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength,
		ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo,
			DeltaTime, ArmRotationSpeed));

		// 이동 방향 벡터의 크기가 0 이상인 경우 (W,A,S,D 입력이 들어온 경우)
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			// FRotationMatrix::MakeFrom : 회전된 공간의 축 벡터를 지정하고, 그로부터 직교한 두 벡터를 구해 회전 행렬을 생성한다.
			// XY(DirectX XZ)평면으로 이동 벡터가 생성되고, 이 방향을 X(앞) 방향으로 해서 회전 행렬을 생성하므로
			// Z축으로 회전한다.
			// SetControlRotation을 통해 해당 방향으로 회전한다.
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

	// 애님인스턴스
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);

	// 몽타주 재생이 끝나면 호출되는 델리게이트를 바인딩
	// Dynamic 델리게이트 : 블루프린트와 연동되는 델리게이트 
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	// 람다 함수 [] : 캡쳐 (this이므로 이 인스턴스의 모든 변수),()인자,->반환값,{}선언
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

	// EInputEvent Enum : 버튼이 눌렸는지/떼졌는지에 대한 인자
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
		// 이동 방향 벡터의 X값을 -1 ~ 1사이로 받는다.
		DirectionToMove.X = NewAxisValue;
		break;
	case EControlMode::GTA:
		// GetControlRotation() : 컨트롤러 회전값 가져옴
		// FRotationMatrix() : 회전 행렬 생성
		// GetUnitAxis(EAxis::X) : X축 벡터 받아옴
		// 최종적으로 컨트롤러가 보고 있는 방향을 받아온다. (SpringArm의 bUsePawnControlRotation 으로 스프링암의 회전을 컨트롤러와 일치시켰으므로)
		// 플레이어가 보고 있는 방향으로의 Input Axis 값만큼 이동시킨다.
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
		// 이동 방향 벡터의 Y값을 -1 ~ 1사이로 받는다.
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
		// 마우스 축 입력에 따른 언리얼 Y축 회전
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
		// 마우스 축 입력에 따른 언리얼 X축 회전
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
	// 충돌 판정을 담아오는 구조체
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	// 플레이어의 전방 2M 범위에 판정
	// Start부터 End까지 구를 "쓸어서" 충돌판정 한다.
	// 캡슐모양 충돌판정이라고 생각하면 됨
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.f,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.f),
		Params);

#if ENABLE_DRAW_DEBUG
	// 디버그 드로잉
	// 캡슐의 원통 끝 위치
	FVector TraceVec = GetActorForwardVector() * AttackRange;

	// 플레이어 위치 + (원통 끝 위치 / 2) = 캡슐의 중앙
	FVector Center = GetActorLocation() + TraceVec * 0.5f;

	// 캡슐의 높이 지정
	float HalfHeight = AttackRange * 0.5f + AttackRadius;

	// 누운 상태로 그려야 함 
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

			// 액터에게 대미지 준다.
			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(50.0f, DamageEvent, GetController(), this);
		}
	}

}

// 언리얼 데미지 프레임워크에서 제공하는 함수
// AActor에 정의된 함수를 오버라이드 해서 사용
// 3번째 인자로 AController를 받는 이유 : 공격의 주체는 "플레이어 자신"이기 떄문
float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ABLOG(Warning, TEXT("Actor : %s took Damage %f"), *GetName(), FinalDamage);

	if (FinalDamage > 0.f)
	{
		ABAnim->SetDeadAnim();

		// 충돌판정 끈다.
		SetActorEnableCollision(false);
	}

	return FinalDamage;
}

