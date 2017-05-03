/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

#include "VRCode.h"
#include "PickupCube.h"


// Sets default values
APickupCube::APickupCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMeshComponent" ) );
	StaticMeshComponent->SetSimulatePhysics( true );
	StaticMeshComponent->bGenerateOverlapEvents = true;

	StaticMeshComponent->SetCollisionProfileName( UCollisionProfile::PhysicsActor_ProfileName );
}

// Called when the game starts or when spawned
void APickupCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupCube::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void APickupCube::Pickup_Implementation( class USceneComponent *AttachTo )
{
	StaticMeshComponent->SetSimulatePhysics( false );
	USceneComponent *Root = GetRootComponent();

	FAttachmentTransformRules AttachmentTransformRules( EAttachmentRule::KeepWorld, false );
	Root->AttachToComponent( AttachTo, AttachmentTransformRules );
}

void APickupCube::Drop_Implementation()
{
	StaticMeshComponent->SetSimulatePhysics( true );

	FDetachmentTransformRules DetatchmentTransformRules( EDetachmentRule::KeepWorld, true );
	DetachFromActor( DetatchmentTransformRules );
}
