// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "UnCommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")

};


UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Damaged"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling")

}; 

UENUM(BlueprintType)
enum class EItemType:uint8
{
  EIT_Ammo   UMETA(DisplayName = "Ammo"),
  EIT_Weapon UMETA(DisplayName=  "Weapon"),
  EIT_MAX    UMETA(DisplayName = "DefaultMax")

};


USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;
};



UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Called when Overlapping AreaSphjer*/
	UFUNCTION()
		void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	/* Called when Overlapping AreaSphjer*/
	UFUNCTION()
		void OnSphereEndOverLap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	/*Sets the ActiveStar array of bools based on rarity */
	void SetActiveStars();

	//Set Properties of the Item's Components based on state */
	virtual void SetItemProperties(EItemState State);

	//Called when ItemInterpTimer is finished 
	void FinishInterping();

	//Handles item interpolation when in the Interp State
	void ItemInterp(float DeltaTime);

	//Get Interp Location based on the item type
	FVector GetInterpLocation();

	void PlayPickupSound(bool bForcePlaySound = false);


	virtual void InitalizeCustomDepth();

	virtual void OnConstruction(const FTransform& Transform) override;

	void EnableGlowMaterial();
	void DisableGlowMaterial();

private:

	/* Line trace colliddes with box to show HUD Widgets*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionBox;

	/* Popup widget for the player looks at the item*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	/*Enables Item Tracing when Overlop*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AreaSphere;



	/**Skeletal Mesh For the Item*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ItemMesh;


	/* The name which appears on the pick widget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FString ItemName;

	/** Item Count **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 ItemCount;

	/* Item rarity - determines number of stars in Pickup Widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =Rarity, meta = (AllowPrivateAccess = "true"))
		EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		TArray<bool> ActiveStars;


	// State of the Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		EItemState ItemState;

	/* The curve asset to use for item's Z location when interping*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UCurveFloat* ItemZCurve;

	/*Starting Location When Interping begins*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FVector ItemInterpStartLocation;

	/** Target interp location in front of the camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FVector CameraTargetLocation;

	/** True when Interping*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		bool bInterping;

	/* Plays when we start interping */
	FTimerHandle ItemInterpTimer;

	//Duration of the curve and timer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		float ZCurveTime;


	/* Pointer to the character */
	class AShooterCharacter* Character;

	// X and Y for the Item while Interping in the EquipIterping state
	float ItemInterpX;
	float ItemInterpY;

	//Initial Yaw Offset between the camera and the interping item
	float InterpInitialYawOffset;

	//Curve used to scale the item when interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* ItemScaleCurve;

	//Sound played when Item is picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class USoundCue* PickupSound;

	//Sound played when the Item is equipped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class USoundCue* EquipSound;

	//Enum for the type of item this item is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		EItemType ItemType;

	//Index of the interp location this item is interping too
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 InterpLocIndex;


	//Background fro this Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconBackground;

	//Icon for this item in the inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconItem;

	//Ammo Icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		UTexture2D* AmmoItem;

	//Slot in the Inventory Array
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		int32 SlotIndex;

	//Index for the Material we'd life to change at Runtime
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 MaterialIndex;

	//Dynamic Instance used the Dynamic Material Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* DynamicMaterialInstance;

	//Material Instance used the Dynamic Materail Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* MaterialInstance;

	bool bCanChangeCustomDepth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	class UDataTable* ItemRarityDataTable;

	// color in the glow material 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Rarity, meta = (AllowPrivateAccess = "true"))
	FLinearColor GlowColor;

	//Light Color in the pickup widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	FLinearColor LightColor;


	//Light Color in the pickup widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	FLinearColor DarkColor;
   
	//Number of stars in the pick up widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	int32 NumberOfStars;

	//Background Icon for the inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemBackground;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisonBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	FORCEINLINE int32  GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void  SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void  SetCharacter(AShooterCharacter* Char) { Character = Char; }

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

	//Called from rhw ASHooterCharacter class
	void StartItemCurve(AShooterCharacter* Char,bool bForcePlaySound = false);

	//Called in ASHOOTER
	void PlayEquipSound(bool bForcePlaySound=false);

};
