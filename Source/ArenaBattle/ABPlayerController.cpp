// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ����Ʈ Ŭ�� �� �ص� �÷����ϸ� ���� ȭ�鿡 �ٷ� ��Ŀ��
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
