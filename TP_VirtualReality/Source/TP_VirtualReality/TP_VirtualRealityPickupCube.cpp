#include "TP_VirtualReality.h"
#include "TP_VirtualRealityPickupCube.h"


// Sets default values
ATP_VirtualRealityPickupCube::ATP_VirtualRealityPickupCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMeshComponent" ) );
	StaticMeshComponent->SetSimulatePhysics( true );
	StaticMeshComponent->bGenerateOverlapEvents = true;

	StaticMeshComponent->SetCollisionProfileName( UCollisionProfile::PhysicsActor_ProfileName );
}

// Called when the game starts or when spawned
void ATP_VirtualRealityPickupCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATP_VirtualRealityPickupCube::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ATP_VirtualRealityPickupCube::Pickup_Implementation( class USceneComponent *AttachTo )
{
	StaticMeshComponent->SetSimulatePhysics( false );
	USceneComponent *Root = GetRootComponent();

	FAttachmentTransformRules AttachmentTransformRules( EAttachmentRule::KeepWorld, false );
	Root->AttachToComponent( AttachTo, AttachmentTransformRules );
}

void ATP_VirtualRealityPickupCube::Drop_Implementation()
{
	StaticMeshComponent->SetSimulatePhysics( true );

	FDetachmentTransformRules DetatchmentTransformRules( EDetachmentRule::KeepWorld, true );
	DetachFromActor( DetatchmentTransformRules );
}
