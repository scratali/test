// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include <DrawDebugHelpers.h>
#include <CollisionQueryParams.h>


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

	FindAttachedPhysicsHandleComponent();
	SetupAttachedInputComponent();

}


void UGrabber::FindAttachedPhysicsHandleComponent()
{
	// Look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
		// Handle is found

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s->%s PhysicsHandle is missing"), *GetOwner()->GetName(), *GetName());
	}
}


void UGrabber::SetupAttachedInputComponent()
{
	// Look for attached input component (at runtime)
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		// InputComponent is found
		UE_LOG(LogTemp, Warning, TEXT("InputComponent found!"));

		// bind action Grab
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s->%s InputComponent is missing"), *GetOwner()->GetName(), *GetName());
	}
}



void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));

	FHitResult Hit = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = Hit.GetComponent();

	if (ComponentToGrab != NULL && Hit.bBlockingHit)
	{
		//PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), ComponentToGrab->GetOwner()->GetActorRotation());
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);
	}

}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab released"));

	PhysicsHandle->ReleaseComponent();
}


const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Get player point of view at this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);


	/// Check for hitting any PhysicsBody object
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	FHitResult HitResult;
	FCollisionQueryParams TraceParam = FCollisionQueryParams(FName(TEXT("")), false);// , GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		HitResult, PlayerViewPointLocation, LineTraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParam
	);

	if (HitResult.bBlockingHit) {
		AActor* ActorHit = HitResult.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("HitResult: %s"), *(ActorHit->GetName()));
	}

	return HitResult;
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the physics handle is attached, move the object that we're holding
	if (PhysicsHandle->GetGrabbedComponent())
	{
		FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);
		FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
	
}



