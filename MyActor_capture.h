// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Async/Async.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Networking.h"

#include "Misc/FileHelper.h"
#include "IPAddress.h"
#include "HAL/ThreadSafeBool.h"

#include "MyActor_capture.generated.h"


UCLASS()
class ML_TEST01_API AMyActor_capture : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor_capture();

	UPROPERTY(EditAnywhere)
	USceneCaptureComponent2D* Camera;
	UTextureRenderTarget2D* RenderTarget;
	UTexture2D* Texture2D;
	UMaterial* mSnapshotMaterial;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* cubetto;

	const uint8* PIPPO;
	UTexture2D* TextureStatica;

	TArray<uint8> FrameMandabile;

	FTexturePlatformData* Data;

protected:
	// Called when the game starts or when spawned
	
	FSocket* ClientSocket;

	bool ConnessioneAttiva = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	TArray<uint8> fStringToBytes(FString InString); // per mandare stringe

	UFUNCTION(BlueprintCallable, Category = "AA")
	bool Emit(const TArray<uint8>& Bytes);		//funzione che manda bytes

	UFUNCTION(BlueprintCallable, Category = "AA")
	bool EmitStr(FString str);					//funzione che manda stringhe

};
