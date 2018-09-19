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

	// Look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
		// Handle is found
		
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s->%s PhysicsHandle is missing"), *GetOwner()->GetName(), *GetName());
	}

	// Look for attached input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		// InputComponent is found
		UE_LOG(LogTemp, Warning, TEXT("%s->%s InputComponent found!"), *GetOwner()->GetName(), *GetName());

		// bind action Grab
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s->%s InputComponent is missing"), *GetOwner()->GetName(), *GetName());
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Get player point of view at this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	/// Draw a line trace
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	DrawDebugLine(
		GetWorld(), PlayerViewPointLocation, LineTraceEnd, FColor(255, 0, 0), false, 0.0, 0, 10
	);

	
	/// Check for hitting any PhysicsBody object
	FHitResult HitResult;
	FCollisionQueryParams TraceParam = FCollisionQueryParams(FName(TEXT("")), false);// , GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		HitResult, PlayerViewPointLocation, LineTraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParam
	);

	if (HitResult.bBlockingHit) {
		AActor* ActorHit = HitResult.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("HitResult: %s"), *(ActorHit->GetName()));
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));
}

