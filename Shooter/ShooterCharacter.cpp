// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Item.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapon.h"
#include "Ammo.h"


// Sets default values
AShooterCharacter::AShooterCharacter() :
	//Bae rates for turning/looking up
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bAiming(false),
	//Camera field of view valuew
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(35.f),
	ZoomInterpSpeed(20.f),
	//Crosshair Spread Factor
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	//Turn rate for aiming/not aiming
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	//Mouse Look sensisitivy scale factors
	MouseHipTurnRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingLookUpRate(0.6f),
	MouseAimingTurnRate(0.6f),
	//Bullet fire timer variables
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	//Automatic gun fire rate
	 AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false),
	/*Item Trace Variables*/
	bShouldTraceForItems(false),
	OverlappedItemCount(0),
	//Camera Interp location variables
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	//Starting Ammount
	Starting9mmAmmo(700),
	StartingARAmmo(175),
	//Combat Variables
	CombatState(ECombatState::ECS_Unoccupied),
	bCrouching(false),
	BaseMovementSpeed(800.f),
	CrouchMovementSpeed(300.f),
	CrouchingCapsuleHalfHeight(44.f),
    StandingCapsuleHalfHeight(88.f),
	BaseGroundFriction(1.f),
	CrouchhGroundFriction(100.f),
	bAimingButtonPressed(false),
	//Pickup sound timer properties
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2),
	HighlightedSlot(-1)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a camera boom (pulls in towards the Character if there is collison)
	CameraBoom = CreateAbstractDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; //The camera follow this distance behind the camera
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	//Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName); //Attach t
	FollowCamera->bUsePawnControlRotation = false;//Camera doesn't rotate relative to arm

	//Don't rotate when the controller rotates. Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; //character move in directionof input
	GetCharacterMovement()->RotationRate = FRotator(0.f,540.f,0.f); //....at this rotation rate 
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Create Hand Scene Component
	HandSceneComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));
	
	//Create Interpolation Components
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1 "));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2 "));
	InterpComp2->SetupAttachment(GetFollowCamera());


	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3 "));
	InterpComp3->SetupAttachment(GetFollowCamera());


	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4 "));
	InterpComp4->SetupAttachment(GetFollowCamera());


	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5 "));
	InterpComp5->SetupAttachment(GetFollowCamera());


	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6 "));
	InterpComp6->SetupAttachment(GetFollowCamera());

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera) 
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	//Spawn the default weapon and  Equip it 
	 EquipWeapon(SpawnDefaultWeapon());
	 EquippedWeapon->SetSlotIndex(0);
	 Inventory.Add(EquippedWeapon);
	 EquippedWeapon->SetCharacter(this);


	 InitializeAmmoMap();
	 GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	 InitializeInterpLocations();


	UE_LOG(LogTemp,Warning,TEXT("BegingPlay() Called "));
	
	int myInt{42};
	UE_LOG(LogTemp, Warning, TEXT("int myInt: %d "),myInt);


}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f)) 
	{
	    //Find out which way is forwards
		const FRotator Rotation{ Controller->GetControlRotation() };
		
		//The current position/degree of rotation
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };
	   
		//Move the X vector
        const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
	  
		AddMovementInput(Direction, Value);
	}

}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//Find out which way is forwards
		const FRotator Rotation{ Controller->GetControlRotation() };

		//The current position/degree of rotation
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };

		//Move the Y Vector
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	//Calculate delta for this frame rate
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() ); //deg.sec * sec/frame
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() ); //deg.sec * sec/frame
}

void AShooterCharacter::Turn(float value)
{
	float TurnScaleFactor{};
	if (bAiming) 
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else 
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(value * TurnScaleFactor );
}

void AShooterCharacter::LookUp(float value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	LookUpScaleFactor = MouseHipTurnRate;

	AddControllerPitchInput(value * LookUpScaleFactor);

}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo()) 
	{
	  PlayFireSound();
	  SendBullet();
	  PlayGunFireMontage();
	  EquippedWeapon->DecrementAmmo();
	  StartFireTimer();
	}


	//Start bullet fire timer for crosshairs
	//StartCrosshairBulletFire();
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzlesSocketLocation, FVector& OutBeamLocation)
{
    FVector2D ViewportSize;
	
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}


	//Get Screen Space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	FHitResult CrosshairHitResult;
	bool  bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult,OutBeamLocation);

	if (bCrosshairHit) 
	{
		//Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else //no crosshair trace hit
	{

	}

	//Perform a second trace this from gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzlesSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzlesSocketLocation };
	const FVector  WeaponTraceEnd{ OutBeamLocation + StartToEnd * 1.25f};
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility
	);

	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
  

	return false;
}

void AShooterCharacter::AimingButtonPressed()
{

	bAimingButtonPressed = true;

	if (CombatState != ECombatState::ECS_Reloading) 
	{
		Aim();
	}

}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();

}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{

	//Set Current Camera Field of view
	if (bAiming)
	{
		/// Interploate Zoom view
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{  // Interploate Default view
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);

}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{0.f,600.f};
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector   Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,Velocity.Size());

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;

	//Calculate crosshair aiming
	if (bAiming)  //Are we aiming
	{
		//Shrink crosshairs a small amount very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,0.6f,DeltaTime,30.f );
	}
	else //Not Aiming
	{
		//Spread Crosshairs back to namral
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	//True 0.05 second after firing
	if (bFiringBullet) 
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else 
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}


	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor+CrosshairInAirFactor-CrosshairAimFactor+CrosshairShootingFactor;

}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairShootTimer,this,&AShooterCharacter::FinishCrosshairBulletFire,ShootTimeDuration);

}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;

}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
 
  	
}    

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;
    GetWorldTimerManager().SetTimer(AutoFireTimer
		                            ,this
		                            ,&AShooterCharacter::AutoFireReset
		                            ,AutomaticFireRate);
	
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo()) 
	{
		bShouldFire = true;
		if (bFireButtonPressed) 
		{
			FireWeapon();
		}
	}
	else 
	{
		ReloadWeapon();
	
	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation)
{

	//Get Viewport Size
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//Get Screen Space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;


	//Get World Position and Direction of Crosshairs 
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);


	if (bScreenToWorld) 
	{
	  //Trace from Crosshair world location outward
	  const FVector Start{ CrosshairWorldPosition };
	  const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
	  GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
	  OutHitLocation = End;
	  if (OutHitResult.bBlockingHit) 
	  {
		  OutHitLocation = OutHitResult.Location;
		  return true;
	  }

	}

	return false;
}

void AShooterCharacter::TraceForItems()
{

	if (bShouldTraceForItems)
	{

		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);

		if (ItemTraceResult.bBlockingHit)
		{
			 TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
			 
			 const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			 if (TraceHitItem) 
			 {
				 if (HighlightedSlot == -1) 
				 {
					 HighlightInventorySlot();
				 }
			    
			 }
			 else 
			 {
			   //IS a slow being highlighted
				 if (HighlightedSlot != -1) 
				 {
                   //UnHighlight the slot
					UnHighlightInventorySlot();
				    
				 }
			 
			 
			 }


			 if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping) 
			 {
				 TraceHitItem = nullptr;
			 }


			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				//Show Item's Pickup Widget
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

			}
		    
			//We hit an AItem last from
			if (TraceHitItemLastFrame) 
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//We are hitting a different AItem this from last frame
					// Or AItem is null. 
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();

				}
			
			}


			//Store a reference to HitItem for next frame
			TraceHitItemLastFrame = TraceHitItem;

		}
	}
	else if (TraceHitItemLastFrame) 
	{
	   //No Longer overlapping any items
	  // Item last frame not show widget
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	
	}

}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
    //Check the TSubclassOf variable
	if (DefaultWeaponClass) 
	{  
		//Spawn the Weapon
		return  GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);  
	}

	return nullptr;
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon) 
	{
		FDetachmentTransformRules DetachmentTranformRules(EDetachmentRule::KeepWorld,true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTranformRules);

	    EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}


}

void AShooterCharacter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this,true);
		TraceHitItem = nullptr;
	}
}

void AShooterCharacter::SelectButtonReleased()
{
	//DropWeapon();
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{

	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex()) 
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
	   	WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
		UE_LOG(LogTemp, Warning, TEXT("Slot Number is: %d "), EquippedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;

}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
	
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	 
	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
  //Play Fire Sound
  if (FireSound)
  {
    UGameplayStatics::PlaySound2D(this, FireSound);
  }
}

void AShooterCharacter::SendBullet()
{
	//Send Bullet
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}

		}
	}

}

void AShooterCharacter::PlayGunFireMontage()
{
  //Play Hip Fire Montage
   UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

   if (AnimInstance && HipFireMontage)
   {
	 AnimInstance->Montage_Play(HipFireMontage);
	 AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;



	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())//replace with carrying amo
	{  
		if (bAiming)
		{
			StopAiming();
		}


		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (ReloadMontage && AnimInstance) 
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}

}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	
	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType)) 
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	//Index for the clip bone on the equipped weapon
	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	
	//Store the transform of the clip
	EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);

}

void AShooterCharacter::ReleaseClip()
{
 EquippedWeapon->SetMovingClip(false);



}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling()) 
	{
	  bCrouching = !bCrouching;
	}
	if (bCrouching) 
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchhGroundFriction;

	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}

}

void AShooterCharacter::Jump()
{
	if (bCrouching) 
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else 
	{
		ACharacter::Jump();
	}

}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching) 
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else 
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),TargetCapsuleHalfHeight,DeltaTime,20.f) };
	
	//Negative value if crouching; Positive if Standing
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	
	const FVector MeshOffSet{ 0.f,0.f,-DeltaCapsuleHalfHeight };

	GetMesh()->AddLocalOffset(MeshOffSet);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;

}

void AShooterCharacter::StopAiming()
{
	bAiming = false;

	if (!bCrouching) {
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}

}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
   //check to see if AmmoMap contains Ammo's Ammotype
	if (AmmoMap.Find(Ammo->GetAmmoType()) )
	{
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();
		//Set the ammount of ammo in the map
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType()) 
	{
	  //Check to see if the gun is empty
		if (EquippedWeapon->GetAmmo() == 0) 
		{
			ReloadWeapon();

		}
	}

	Ammo->Destroy();
}


FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}

	return FInterpLocation();
}


void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{ WeaponInterpComp,0 };
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ InterpComp1, 0 };
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ InterpComp2, 0 };
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ InterpComp3, 0 };
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ InterpComp4, 0 };
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ InterpComp5, 0 };
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ InterpComp6, 0 };
	InterpLocations.Add(InterpLoc6);

}

void AShooterCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;

	UE_LOG(LogTemp, Warning, TEXT("1 Key is pressed"));
   
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);

}

void AShooterCharacter::TwoKeyPressed()
{

	if (EquippedWeapon->GetSlotIndex() == 2) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);

}

void AShooterCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);


}

void AShooterCharacter::FourKeyPressed()
{

	if (EquippedWeapon->GetSlotIndex() == 4) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);


}

void AShooterCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);


}

void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if ( (CurrentItemIndex == NewItemIndex) || (NewItemIndex >= Inventory.Num())) return;
	auto OldEquippedWeapon = EquippedWeapon;
	auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
	EquipWeapon(NewWeapon);

	OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
	NewWeapon->SetItemState(EItemState::EIS_Equipped);

	CombatState = ECombatState::ECS_Equipping;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	  if (AnimInstance && EquipMontage) 
	  {
		AnimInstance->Montage_Play(EquipMontage,1.0f );
		AnimInstance->Montage_JumpToSection(FName("Equip"));
		UE_LOG(LogTemp, Warning, TEXT("Equip Montage"));

	  }

	  NewWeapon->PlayEquipSound(true);

}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++) 
	{
		if (Inventory[i] == nullptr) 
		{
			return i;
		}
	}

	if (Inventory.Num() < INVENTORY_CAPACITY) 
	{
		return Inventory.Num();
	}
	
	return -1;//Inventor is full!
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot{ GetEmptyInventorySlot() };
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;

}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot,false);
	HighlightedSlot = -1;
}



int32 AShooterCharacter::GetInterpLocationIndex()
{
	
	int32 LowestIndex =  1;
	int32 LowestCount = INT_MAX;
	
	for (int32 i = 1; i < InterpLocations.Num(); i++) 
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	
	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{

	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}


}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::ResetPickupSoundTimer, PickupSoundResetTime);

}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip) 
	{
		
		//Get the Hand
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

		if (HandSocket)
		{
			//Attach the weapon to hand socket RightHandSocket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (EquippedWeapon == nullptr) 
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else 
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(),WeaponToEquip->GetSlotIndex());
		}

    	EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Handle interpoloation for zoom while aiming
	CameraInterpZoom(DeltaTime);

	//Change look sensitivity based on aiming
	SetLookRates();

	CalculateCrosshairSpread(DeltaTime);
	
	//Check OverlappedItemCount, then trace for items
	TraceForItems();

	//Interpolate the capsule half height based on crouching/standing
	InterpCapsuleHalfHeight(DeltaTime);
	
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
    PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
    PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AShooterCharacter::FKeyPressed);
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AShooterCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AShooterCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &AShooterCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &AShooterCharacter::FiveKeyPressed);
}

void AShooterCharacter::FinishReloading()
{
	//Update the Combat State
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed) 
	{
		Aim();
	}

	if (EquippedWeapon == nullptr) return;

	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	//Update the AmmoMap
	if (AmmoMap.Contains(EquippedWeapon->GetAmmoType())) 
	{
		
		//Amount of ammo the character is carrying of the EquippedWeapon Type
		int32 CarriedAmmo = AmmoMap[AmmoType];
	    
		//Space left in the magazine of EquippedWeapon
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
		  //Reload the magazine with all the ammo we are carrying
		  EquippedWeapon->ReloadAmmo(CarriedAmmo);
		  CarriedAmmo = 0;
		  AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else 
		{
		   //fill the magazine 
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}

}

void AShooterCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unoccupied;

}


void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

float AShooterCharacter::GetCrosshairSpreadMulitplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0) 
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;;
	}
	else 
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}

}


/* No Longer need; AItem as interp location
FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{ FollowCamera->GetForwardVector() };
	//Desired = CameraWorldLocation + Forward * A + Up * B
	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);


}

*/


void AShooterCharacter::GetPickupItem(AItem* Item)
{

	Item->PlayEquipSound();
	

	auto Weapon = Cast<AWeapon>(Item);

	if (Weapon) 
	{
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		} 
		else //Inventory is Full
		{
			SwapWeapon(Weapon);
		
		}
		//SwapWeapon(Weapon);
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo) 
	{
		PickupAmmo(Ammo);
	}
}

