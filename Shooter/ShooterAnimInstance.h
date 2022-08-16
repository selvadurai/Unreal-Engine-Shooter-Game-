// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming    UMETA(DisplayName = "Aiming"),
	EOS_Hip       UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir     UMETA(DisplayName = "InAir"),
	EOS_MAX       UMETA(DisplayName = "DefaultMAX")
};



UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

    
	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:

	void TurnInPlace();

	void Lean(float DeltaTime);

private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Movement,meta=(AllowPrivateAccess="true"))
	class AShooterCharacter* ShooterCharacter;

	/* The Speed of the character*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/* Whether the character is in the air*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement,  meta = (AllowPrivateAccess ="true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	
	/* Offset yaw used for strafing  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float MovementOffSetYaw;

	/*Offset yaw the frame we stopped before we stop moving*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffSetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// Yaw of the character this frame, only standing still
	float TIPCharacterYaw;

	// Yaw of the character the previous frame, previous frame. 
	float TIPCharacterYawLastFrame;


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=" Turn in Place",meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	//Rotation curve value this frame
	float RotationCurve;

	//Rotation curve value last frame
	float RotationCurveLastFrame;

	// The Pitch of the aim roation, used for Aim OffSet
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " Turn in Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	//True when reloading, used to prevent Aim Offset while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " Turn in Place", meta = (AllowPrivateAccess = "true"))
    bool bReloading;

	//OFFset state used to determine which offset to use. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " Turn in Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	// Character YAW this frame. 
	FRotator CharacterRotation;

	/// Character YAW  last frame
	FRotator CharacterRotationLastFrame;

	//YAW delta used for leaning in the running blendspace
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	//True when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
    bool bEquiping;

	//Change the recoil weight based on turing on weight and turning
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combar, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	//Change the recoil weight based on turing on weight and turning
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combar, meta = (AllowPrivateAccess = "true"))
	float bTuringInPlace;
};
