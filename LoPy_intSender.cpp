// Fill out your copyright notice in the Description page of Project Settings.


#include "LoPy_intSender.h"

// Sets default values
ALoPy_intSender::ALoPy_intSender()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.33f;

}

// Called when the game starts or when spawned
void ALoPy_intSender::BeginPlay()
{
	Super::BeginPlay();

	Numero = 10;
	

// SOCKET PART
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address ip;
	FIPv4Address::Parse(TEXT("192.168.2.202"), ip);
	RemoteAddress->SetIp(ip.Value);
	RemoteAddress->SetPort(4888);

	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	if (ClientSocket->Connect(*RemoteAddress))
	{
		GLog->Logf(TEXT("LoPy: Connessione avvenuta"));
		ConnessioneAttiva = true;
		return;
	}
	else
	{
		GLog->Logf(TEXT("LoPy: Connessione NON avvenuta"));
		ConnessioneAttiva = false;

		return;
	}

}

// Called every frame
void ALoPy_intSender::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
	{
		int32 BytesSent = 0;

		Messaggio_ToBe[0] = Numero;

		Messaggio = static_cast<const uint8*>(Messaggio_ToBe);

		ClientSocket->Send(Messaggio, 1, BytesSent);
	}

}

