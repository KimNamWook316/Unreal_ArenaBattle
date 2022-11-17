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

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EInputEvent Enum : ��ư�� ���ȴ���/���������� ���� ����
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

