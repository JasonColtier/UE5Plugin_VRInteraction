#include "IrwinoGrippablePhone.h"

#include "VRInteraction/ActorComponents/TouchScreen/TouchScreen.h"

// Sets default values
AIrwinoGrippablePhone::AIrwinoGrippablePhone(const FObjectInitializer& Initializer): Super(Initializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PhoneTouchScreen      = CreateDefaultSubobject<UChildActorComponent>(TEXT("TouchScreen"));
	PhoneCollisionHandler = CreateDefaultSubobject<UBoxComponent>(TEXT("TouchScreenCollision"));

	PhoneTouchScreen->SetupAttachment(RootComponent);
	PhoneCollisionHandler->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AIrwinoGrippablePhone::BeginPlay()
{
	Super::BeginPlay();
}

void AIrwinoGrippablePhone::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIrwinoGrippablePhone::PostInitProperties()
{
	Super::PostInitProperties();
}

// Called every frame
void AIrwinoGrippablePhone::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}
