// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"

AABGameMode::AABGameMode()
{
	// ĳ���͸� �⺻ ������ ����
	DefaultPawnClass = AABCharacter::StaticClass();

	// �÷��̾� ��Ʈ�ѷ� ����
	PlayerControllerClass = AABPlayerController::StaticClass();

 //	static ConstructorHelpers::FClassFinder<APawn> BP_PAWN_C(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
 //
 //	if (BP_PAWN_C.Succeeded())
 //		DefaultPawnClass = BP_PAWN_C.Class;
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	ABLOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	ABLOG(Warning, TEXT("PostLogin End"));
}