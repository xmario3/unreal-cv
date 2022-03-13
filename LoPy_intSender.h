// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Async/Async.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Networking.h"

#include "LoPy_intSender.generated.h"

UCLASS()
class ML_TEST01_API ALoPy_intSender : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALoPy_intSender();

	UPROPERTY(BlueprintReadWrite)
		int Numero;

	uint8 Messaggio_ToBe[1];

	const uint8* Messaggio;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FSocket* ClientSocket;
	bool ConnessioneAttiva = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
