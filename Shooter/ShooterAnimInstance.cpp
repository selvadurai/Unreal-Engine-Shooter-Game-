// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"




UShooterAnimInstance::UShooterAnimInstance():
  Speed(0.f),
  bIsInAir(false),
  bIsAccelerating(false),
  MovementOffSetYaw(0.f),
  LastMovementOffSetYaw(0.f),
  bAiming(false),
  CharacterRotation(FRotator(0.f)),
  CharacterRotationLastFrame(FRotator(0.f)),
  TIPCharacterYaw(0.f),
  TIPCharacterYawLastFrame(0.f),
  YawDelta(0.f),
  RootYawOffset(0.f),
  Pitch(0.f),
  bReloading(false),
  OffsetState(EOffsetState::EOS_Hip),
  RecoilWeight(1.0f),
  bTuringInPlace(1.0f)
{
	

}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr) 
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter) 
	{
		bCrouching = ShooterCharacter->GetCrouching();
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
		bEquiping  = ShooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;

		// Get the lateral speed of the character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//IS the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//Is the character accelerating?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else 
		{
			bIsAccelerating = false;

		}

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffSetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation).Yaw;

		if (ShooterCharacter->GetVelocity().Size() > 0.f) 
		{
			LastMovementOffSetYaw = MovementOffSetYaw;
		}
	    
		bAiming = ShooterCharacter->GetAiming();

		if (bReloading) 
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming()) 
		{
			OffsetState = EOffsetState::EOS_Aiming;
			UE_LOG(LogTemp, Warning, TEXT("In Aiming State"));

		}
		else 
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	} 
	TurnInPlace();
	Lean(DeltaTime);

}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;



	if (Speed > 0 || bIsInAir) 
	{
		RootYawOffset = 0.f;
		TIPCharacterYaw  = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.0f;
		RotationCurve = 0.0f;
	}
	else 
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

		//Root Yaw Offset, updated and clamped to [-180,180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset-TIPYawDelta);

		const float Turning{ GetCurveValue(TEXT("Turning")) };
		
		// 1.0 if Turning, 0.0 if not
		if(Turning > 0)
		{
			bTuringInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			//RootYawOffSet > 0,-> Turn Left. RootYawOffSet < 0 Turn Right.
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };

			if(ABSRootYawOffset > 90.0f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.0f };
				ABSRootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}

		}
		else 
		{
			bTuringInPlace = false;
		}
	
	}

	//Set Recoil Weight
	if (bTuringInPlace)
	{
		if (bReloading || bEquiping)
		{
			RecoilWeight = 1.0f;
		}
		else
		{
			RecoilWeight = 0.1f;
		}


	}
	else //not turning in place
	{
		if (bCrouching)
		{
			if (bReloading || bEquiping || bReloading)
			{
				RecoilWeight = 1.0f;
			}
			else
			{
				RecoilWeight = 0.1f;
			}
		}
		else
		{
			if (bAiming || bReloading)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.5f;
			}
		}

	}
}


void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;
	
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame) };


	const float Target{ Delta.Yaw / DeltaTime };
	const float Interp{ FMath::FInterpTo(YawDelta,Target,DeltaTime,6.f) };
	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
}

