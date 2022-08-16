#pragma once



UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_AR UMETA(DisplayName = "Assualt Rifle"),
	EAT_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName ="Equipping"),


	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

