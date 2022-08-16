// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"



USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

    //Scence Component to use for it's location interping		 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	//Number of items interping to/at this scene comp location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);


UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FInterpLocation GetInterpLocation(int32 Index);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*Called for forwards/backwards input*/
	void MoveForward(float Value);

	/**Called for  */
	void MoveRight(float Value);

	/*
	   Called via input to turn at a given rate
	   This is a normalization rate, i.e 1.0 means 100% of desired turn rage
	*/
	void TurnAtRate(float Rate);

	/*
	   Called via input to look up/down at a given rate
	   @param Rate. This is a normalized rate. 
	*/
	void LookUpAtRate(float Rate);

	/*
     Rotate controller based on mouse X movement
     @param Value The input value from mouse movement
     */
	void Turn(float value);

	/*
     Rotate controller based on mouse X movement
      @param Value The input value from mouse movement
     */
	void LookUp(float value);

	/**Called when fire button is pressed  */
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzlesSocketLocation, FVector& OutBeamLocation);

	/* Set bAiming to true or false with button Pressed */
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);
	
	/* Set BaseTurnRate and BaseLookUpRate based on aiming */
	void SetLookRates();


	void  CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();
	
	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/* Line trace for items under the crosshair**/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/** Trace for items if OverlappedItemCount>0 */
	void TraceForItems();

	/* Takes a weapon and attaches it to mesh*/
	void EquipWeapon(class AWeapon* WeaponToEquip);

	/* Spawns a default weapon equips it*/
	AWeapon* SpawnDefaultWeapon();


	/*Detach Weapon and let it fall to the ground*/
	void DropWeapon();


	void SelectButtonPressed();
	void SelectButtonReleased();
	
	/* Drops currently equipeed Weapon and Equips TraceItem */
	void SwapWeapon(AWeapon* WeaponToSwap);

	void InitializeAmmoMap();

	//*Check if weapon has ammo*/
	bool WeaponHasAmmo();

	/* FireWeapon functions*/
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();
	
	// Bound to the R key and Gamepad Face button left
	void ReloadButtonPressed();
    void ReloadWeapon();

	//Check to see if we have ammo of the EquippedWeapon's Ammo Type
	bool CarryingAmmo();

	// Called from Animation Blueprint with Grab Clip Notify
	UFUNCTION(BlueprintCallable)
    void GrabClip();

	//Called From Animation Blueprint with Reload
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

	virtual void Jump() override;

	//Interps capsule half height when crouching/standing
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();

	void StopAiming();

	void PickupAmmo(class AAmmo* Ammo);

	void InitializeInterpLocations();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex,int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	void HighlightInventorySlot();


private:
	/* Camera Boom Positioning the Camera behing the Character*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera,meta= (AllowPrivateAccess="true") );
	class USpringArmComponent* CameraBoom;
	
	/*Camera that follow the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseLookUpRate;

	/* Randomize gunshot sound cue*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;

	/*  Flash spawn at Barrel socket  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash;

    /* Montage for firing weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontage;

	/*Particles spawend uping Bullet*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UParticleSystem* ImpactParticles;

	/* Smoke trail for bullets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UParticleSystem* BeamParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	//Camera Default Camera field of view value /*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CameraDefaultFOV;

    //Camera view when zooming in/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CameraZoomedFOV;

	//Current Field of view this frame
	float CameraCurrentFOV;

	/* Interp speed for zooming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	
	/* Velocity component for crosshairs spread*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/*In air component for crossshairs spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/*In air component for crossshairs spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	
	/* Aim Component for crosshairs spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/* Shooting Component for crosshairs spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/* Turn rate while not aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float HipTurnRate;

	/* Look up rate when not aiming*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float HipLookUpRate;

	/*Turn rare when aiming   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float AimingTurnRate;

	/*Look Up rate when aiming   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float AimingLookUpRate;

	/*Scale Factor for mouse look sensitivity. Turn rate when not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"),meta =(ClampMin ="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float MouseHipTurnRate;

	/*Scale Factor for mouse look sensitivity. Turn rate when  aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	/*Scale Factor for mouse look sensitivity. Turn rate when not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	/*Scale Factor for mouse look sensitivity. Turn rate when  aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	/* Left mouse buttom or right console trigger pressed*/
	bool bFireButtonPressed;

    /* Rate of automatic gun fire*/
	float AutomaticFireRate;

	/* True when we can fire. False when waiting for the timer*/
	bool bShouldFire;

	 /*Sets a time between gunshots*/
    FTimerHandle AutoFireTimer;


	/* True if we should trave every frame for items*/
	bool bShouldTraceForItems;

	/*Numbwe of overlapped AItems */
	int8 OverlappedItemCount;

	/* The AItem we hit last frame */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category =Items, meta = (AllowPrivateAccess="true"))
	class AItem* TraceHitItemLastFrame;

	/*Currently equiiped Weapon*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/*Set this in Blueprint for the defauly Weapon Class*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true") )
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/* The Item current hit by our trace in TraceForItems (could be null)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/*Distance outwaerd from the camera for the interp distance*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	/**Dii*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	/* Map to keep track of ammo of different ammo types */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/*Starting ammount of 9mm ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	//Starting amount of AR ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	// Combat State, can only fire or reload if unoccupied
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	//Montage for Reload Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();


	UFUNCTION(BlueprintCallable)
	void FinishEquipping();


	//Transform of the clop when first grab the clip
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	//Scene Component to attach to the character hand during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	//True when crouching 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;


	//Regular Movement speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	//Crouch Movement Speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float CrouchMovementSpeed;


	//Current half height of the capsule
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CurrentCapsuleHalfHeight;

	// Half height of the capsule when not crouching 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;


	// Half height of the capsule when crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	//Ground Friction while not crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;


	//Ground Friction while crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchhGroundFriction;

	//Used for knowing when aiming is being pressed
	bool bAimingButtonPressed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;


	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	//Time to wait before we can play another pickup sound
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;
	
	//Time to wait before we can play another equip sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	//An array of AItems for out inventory
	UPROPERTY(VisibleAnyWhere,BlueprintReadOnly,Category= Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*>  Inventory;

	const int32 INVENTORY_CAPACITY{ 6 };

	//Delegate for sending slot information to IventoryBar when equipping
	UPROPERTY(BlueprintAssignable,Category =Delegates, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;
	
	//Delegate for sending slot information for playing the icon animation
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	// The Index for the currently highlighted slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot;

public:
	/* Returns CameraBoom subobject*/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/* Returns FollowCamera subobject*/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const{ return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMulitplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount;  }

	/** Adds/sbuttacts to/from itemCount */
	void IncrementOverlappedItemCount(int8 Amount);

	//No longer needed
	//FVector GetCameraInterpLocation();

	void GetPickupItem(AItem * Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE  bool     GetCrouching()	const{ return bCrouching; }

	//Returns the index in the interplocation
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);


	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();


	void UnHighlightInventorySlot();

};
