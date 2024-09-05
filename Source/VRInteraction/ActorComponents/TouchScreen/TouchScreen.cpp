#include "TouchScreen.h"

#include <Components/ArrowComponent.h>
#include <Components/WidgetComponent.h>
#include <Components/WidgetInteractionComponent.h>
#include <Kismet/KismetSystemLibrary.h>

#include "TraceTool.h"
#include "VRInteraction/ActorComponents/Interfaces/IrwinoTouchableInterface.h"

// Sets default values for this component's properties
ATouchScreen::ATouchScreen(const FObjectInitializer& Initializer): Super(Initializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = false;

	TouchScreenSurface         = CreateDefaultSubobject<UBoxComponent>(TEXT("TouchScreenSurface"));
	RootComponent              = TouchScreenSurface;
	WidgetComponent            = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Widget Interaction Component"));

	TouchScreenSurface->SetCollisionProfileName("FingerInteract");
	WidgetInteractionComponent->bShowDebug          = false;
	WidgetInteractionComponent->InteractionDistance = 15.f;
	ReleaseTimeInSeconds                            = 1.0f;

	WidgetComponent->SetupAttachment(RootComponent);
	WidgetInteractionComponent->SetupAttachment(WidgetComponent);

	if (UArrowComponent* ComponentByClass = FindComponentByClass<UArrowComponent>())
	{
		ComponentByClass->DestroyComponent(); // idk why the editor creates this by default but it's annoying
	}
}

void ATouchScreen::OnTouch_Implementation(AActor* InitiatingTouchActor,const FVector ImpactPoint)
{
	IIrwinoTouchableInterface::OnTouch_Implementation(InitiatingTouchActor,ImpactPoint);
	// DrawDebugSphere(GetWorld(),ImpactPoint,1,3,FColor::Orange,true,5,0,0.1);


	TRACE_WARN("Authority %s", HasAuthority()?TEXT("true"):TEXT("false"))
	SetOwner(InitiatingTouchActor);

	if (ReleaseHandle.IsValid())
	{
		return;
	}

	FVector       SurfacePoint;
	const FVector Normal = WidgetComponent->GetForwardVector();

	TouchScreenSurface->GetClosestPointOnCollision(ImpactPoint, SurfacePoint);
	//we try to place the widget component to hit the ImpactPoint
	//we place it allong the normal from the screen at half the interaction distance so it will hit
	const FVector Placement = SurfacePoint + Normal * (WidgetInteractionComponent->InteractionDistance/2);

	// DrawDebugSphere(GetWorld(),Placement,1,10,FColor::White,true,5,0,0.1);
	WidgetInteractionComponent->SetWorldLocation(Placement);
	WidgetInteractionComponent->SetWorldRotation((-Normal).Rotation());
	
	FTimerDelegate ClickDelegate;
	ClickDelegate.BindLambda([this, Normal, Placement]
	{
		WidgetInteractionComponent->PressPointerKey(PointerKey);

		// UKismetSystemLibrary::DrawDebugLine(this,
		//                                     Placement,
		//                                     Placement + -Normal * WidgetInteractionComponent->InteractionDistance,
		//                                     FColor::Cyan,
		//                                     1
		//                                    );

		FTimerDelegate ReleaseDelegate;
		ReleaseDelegate.BindLambda([this, Normal]
		{
			WidgetInteractionComponent->ReleasePointerKey(PointerKey);
			WidgetInteractionComponent->SetWorldRotation(Normal.Rotation());
			GetWorldTimerManager().ClearTimer(ReleaseHandle);
		});

		GetWorldTimerManager().SetTimer(ReleaseHandle, ReleaseDelegate, ReleaseTimeInSeconds, false);
	});
	
	GetWorldTimerManager().SetTimerForNextTick(ClickDelegate);
}

// Called when the game starts
void ATouchScreen::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void ATouchScreen::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// ...
}
