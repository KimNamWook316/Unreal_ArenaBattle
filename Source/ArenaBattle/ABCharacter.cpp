// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"

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

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EInputEvent Enum : 버튼이 눌렸는지/떼졌는지에 대한 인자
	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);

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

