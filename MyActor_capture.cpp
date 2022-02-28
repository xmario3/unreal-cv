// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor_capture.h"
#include "ML_Test01.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Components/SphereComponent.h"
#include "Components/SceneCaptureComponent2d.h"


// Sets default values
AMyActor_capture::AMyActor_capture()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	
	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));

	ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetAsset(TEXT("/Game/MyLittleRenderTarget"));

	RenderTarget = RenderTargetAsset.Object;

	//RenderTarget = DuplicateObject(RenderTargetAsset.Object, NULL);
	//RenderTarget->InitAutoFormat(1024, 1024);
	//RenderTarget->ClearColor = FLinearColor::Blue;

	Camera->TextureTarget = RenderTarget;

	Camera->ProjectionType = ECameraProjectionMode::Type::Perspective;
	Camera->FOVAngle = 120.0f;
	Camera->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	Camera->CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;
	Camera->bAutoActivate = true;
	Camera->bCaptureEveryFrame = true;
	Camera->bCaptureOnMovement = true;
/*
	
	// Retrieving the render target
	UTextureRenderTarget2D* renderTarget2D = Camera->TextureTarget;

	// Path to the material
	ConstructorHelpers::FObjectFinder<UMaterial> snapshotMaterialFinder(TEXT("Material'/Game/MyLittleRenderTarget_Mat.MyLittleRenderTarget_Mat'"));

	// Need to cast the pointer. The mSnapshotMaterial has type UMaterial*
	mSnapshotMaterial = (UMaterial*)snapshotMaterialFinder.Object;

	// Creating a new material from the base one and setting the render target as texture
	UMaterialInstanceDynamic* dynamicMaterial = UMaterialInstanceDynamic::Create(mSnapshotMaterial, this, FName("MaterialeDinamico"));
	dynamicMaterial->SetTextureParameterValue(FName("Texture"), RenderTarget);

	*/
	cubetto = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("cubetto"));
	this->SetRootComponent(cubetto);

	UStaticMesh* cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;
	cubetto->SetStaticMesh(cubeMesh);

}

// Called when the game starts or when spawned
void AMyActor_capture::BeginPlay()
{
	Super::BeginPlay();
	
	Camera->TextureTarget = RenderTarget;
	//what is interesting, it lets method GetRenderTargetResource to work. Here, in BeginPlay. Try to comment it out to see crash of Editor.

	int X = RenderTarget->GetSurfaceHeight();
	int Y = RenderTarget->GetSurfaceWidth();
	GLog->Logf(TEXT("Size: %d %d"), X, Y);
	Texture2D = RenderTarget->ConstructTexture2D(this, FString("Tex2D"), EObjectFlags::RF_NoFlags);
	//FTextureRenderTargetResource *Resource = RenderTarget->GetRenderTargetResource();
	int xx = Texture2D->GetSizeX();
	int yy = Texture2D->GetSizeY();
	GLog->Logf(TEXT("Texture size: %d %d"), xx, yy);

	FTexturePlatformData* Data = Texture2D->PlatformData;
	EPixelFormat Format = Data->PixelFormat;
	GLog->Logf(TEXT("Pixel format: %d"), (uint8)(Format));
	//format of pixel is PFloatRGBA

	int size = Data->Mips[0].BulkData.GetElementSize();
	int N = Data->Mips[0].BulkData.GetElementCount();
	GLog->Logf(TEXT("Number of elements: %d, size of one element: %d"), N, size);
	//i've got 1 byte size of element

	//const void* Table = Data->Mips[0].BulkData.LockReadOnly();
	PIPPO = static_cast<const uint8*>(Data->Mips[0].BulkData.LockReadOnly());
	Data->Mips[0].BulkData.Unlock();

	
	
/*
	Tab2 = StaticCast<const uint16*>(Table);
	//ok, quick calculation. I get 8*1024*1024 bytes from 1024*1024 pixels, so one pixel got 8 bytes of data. Format is RGBA, so you can figure it yourself :)
	for (int i = 0; i < xx; i++)
		for (int j = 0; j < yy; j++) {
			int k = 4 * (i * yy + j);
			int R = Tab2[k];
			int G = Tab2[k + 1];
			int B = Tab2[k + 2];
			int A = Tab2[k + 3];
		}
*/	
// SOCKET PART
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	
	FIPv4Address ip;
	FIPv4Address::Parse(TEXT("127.0.0.1"), ip);
	RemoteAddress->SetIp(ip.Value);
	RemoteAddress->SetPort(4502);

	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	if (ClientSocket->Connect(*RemoteAddress))
	{
		GLog->Logf(TEXT(" Connessione avvenuta"));
		ConnessioneAttiva = true;
		return ;
	}
	else
	{
		GLog->Logf(TEXT(" Connessione NON avvenuta"));
		ConnessioneAttiva = false;

		return ;
	}

	


}

// Called every frame
void AMyActor_capture::Tick(float DeltaTime)
{
	if (ConnessioneAttiva)
	{


	}

	if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
	{	
		int32 BytesSent = 0;
		ClientSocket->Send(PIPPO, 1024, BytesSent);
	}

	Super::Tick(DeltaTime);
}


/**
 * Invia byte al server
 * */
bool AMyActor_capture::Emit(const TArray<uint8>& Bytes)
{
	bool resp = false;

	// проверяем есть ли подлючение
	if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
	{
		int32 BytesSent = 0; // quanti byte sono stati inviati
		resp = ClientSocket->Send(Bytes.GetData(), Bytes.Num(), BytesSent);
	}
	return resp;
}

/**
* Converti stringhe in byte
*/
TArray<uint8> AMyActor_capture::fStringToBytes(FString InString)
{
	TArray<uint8> ResultBytes;
	ResultBytes.Append((uint8*)TCHAR_TO_UTF8(*InString), InString.Len());
	return ResultBytes;
}

/**
 * 
 * */
bool AMyActor_capture::EmitStr(FString str)
{
	bool resp = false;

	
	if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
	{
		int32 BytesSent = 0; 
		
		TArray<uint8> Bytes = fStringToBytes(str);
		resp = ClientSocket->Send(Bytes.GetData(), Bytes.Num(), BytesSent);
	}
	return resp;
}