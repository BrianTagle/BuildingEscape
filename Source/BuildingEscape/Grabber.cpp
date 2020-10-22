// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	//Check for physics handle component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		/* code */
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s does not have a physics handle component"), *GetOwner()->GetName());
	}
	
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s has an input component"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab",IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab",IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is missing an input component"), *GetOwner()->GetName());
	}
	
}
void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Grabber Pressed"), *GetOwner()->GetName());	

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	//ray cast out to a certain distance (reach)
	//see what it hits
	
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() *Reach;
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	if(HitResult.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			LineTraceEnd
		);
	}
}
void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Grabber Released"), *GetOwner()->GetName());	
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	//ray cast out to a certain distance (reach)
	//see what it hits
	
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() *Reach;

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
	

}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{

	
	// get players viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	//ray cast out to a certain distance (reach)
	//see what it hits
	
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() *Reach;
	/*
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0,255,0),
		false,
		0.f,
		0,
		5.f
	);
	*/

	FHitResult Hit;
	FCollisionQueryParams  TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	AActor* ActorHit = Hit.GetActor();

	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace has hit: %s"), *ActorHit->GetName());
	}
	return Hit;
}

