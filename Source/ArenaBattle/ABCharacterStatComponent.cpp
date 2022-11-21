// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"

// Sets default values
AABCharacterStatComponent::AABCharacterStatComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABCharacterStatComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

