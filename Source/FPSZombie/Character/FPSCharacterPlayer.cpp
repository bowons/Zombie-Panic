// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FPSCharacterPlayer.h"
#include "Animations/FPSPlayerAniminstance.h"
#include "Weapons/FPS_WeaponPistol.h"
#include "Weapons/FPS_Weapon_Rifle.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/FPSPlayerController.h"
#include "Engine/DamageEvents.h"
#include "Widget/FPSMainWidget.h"
#include "Game/FPSZombieSingleton.h"
#include "GameData/FPSZombieRoundInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FPSZombieGameMode.h"

AFPSCharacterPlayer::AFPSCharacterPlayer()
{
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	FPSCamera->bUsePawnControlRotation = true;
	FPSCamera->SetupAttachment(RootComponent);
	FPSCamera->SetRelativeLocation(FVector(16.0f, 0.0f, 75.0f));

	SetupMeshAndShadows();
	SetupDefaultMovementAndInput();
	SetupPlayerAudioCopmonents();

	PrimaryActorTick.bCanEverTick = true;
	bIsSprinting = false;
}

void AFPSCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add("Player");

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		EnableInput(PlayerController);

		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (true == IsValid(SubSystem)) {
			SubSystem->AddMappingContext(InputConfig->PlayerMappingContext,0);
		}
	}

	HideHeadandArmMesh();
	AttachWeapon();

	if (AFPSZombieGameMode* GameMode = Cast<AFPSZombieGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnRoundStarted.AddDynamic(this, &AFPSCharacterPlayer::HandleRoundStarted);
		GameMode->OnPlayerReady(this);
	}
}

void AFPSCharacterPlayer::Tick(float DeltaTime)
{
	float CharacterSpeed = GetCharacterMovement()->Velocity.Size2D();//Ver2
	float AnimSpeed = PlayerArmAnimInstance->GetCharacterMovementSpeed(); // ArmAnim GroundSpeed
	AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(GetController());


	if (CharacterSpeed > MovementWalkSpeed) {
		bIsSprinting = true;
	}
	else {
		bIsSprinting = false;
	}

	if (bJogCoolDown)
		bIsSprinting = false;

	// 총기 발사
	if (SelectedWeapon) {
		if (!(SelectedWeapon->isReloading() || AnimSpeed > (MovementWalkSpeed + 75.0f) || bIsSwaping)) {
			if (SelectedWeapon->isFiring() && SelectedWeapon->isAutomatic() && SelectedWeapon->isNotCoolDown()) {
				SelectedWeapon->Fire();
				//PlayerController->ShakeCamera();
			} else if (SelectedWeapon->isFiring() && !SelectedWeapon->isAutomatic()) {
				if (SelectedWeapon->isNotCoolDown()) {
					SelectedWeapon->Fire();
					//PlayerController->ShakeCamera();
					SelectedWeapon->StopFiring();
				}
			}
		}
	}

	// 스테미너 감소
	if (bIsSprinting) {
		if (StaminarPoint > 0) {
			StaminarPoint -= 0.5f;
			OnStaminarChanged.Execute(StaminarPoint);
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = MovementWalkSpeed;
			bJogCoolDown = true;
			bIsSprinting = false;
		}
	} else {
		if (StaminarPoint < 100) {
			StaminarPoint += 0.25f;
			OnStaminarChanged.Execute(StaminarPoint);
		}
	}

	if (StaminarPoint >= 100)
		bJogCoolDown = false;
	
	GetRebound(DeltaTime);
	AimingTimeline.TickTimeline(DeltaTime);
}

void AFPSCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Jump")], ETriggerEvent::Triggered, this, &ACharacter::Jump); 
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Jump")], ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Look")], ETriggerEvent::Triggered, this, &AFPSCharacterPlayer::CharacterLook);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Move")], ETriggerEvent::Triggered, this, &AFPSCharacterPlayer::CharacterMove);

		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Sprint")], ETriggerEvent::Triggered, this, &AFPSCharacterPlayer::StartSprint);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Sprint")], ETriggerEvent::Ongoing, this, &AFPSCharacterPlayer::StartSprint);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Sprint")], ETriggerEvent::Completed, this, &AFPSCharacterPlayer::StopSprint);

		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Fire")], ETriggerEvent::Started, this, &AFPSCharacterPlayer::Attack);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Fire")], ETriggerEvent::Completed, this, &AFPSCharacterPlayer::StopFiring);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Reload")], ETriggerEvent::Triggered, this, &AFPSCharacterPlayer::Reload);

		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Aim")], ETriggerEvent::Triggered, this, &AFPSCharacterPlayer::StartAiming);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Aim")], ETriggerEvent::Ongoing, this, &AFPSCharacterPlayer::StartAiming);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Aim")], ETriggerEvent::Completed, this, &AFPSCharacterPlayer::StopAiming);
		
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Num1")], ETriggerEvent::Triggered, this, &AFPSCharacterPlayer::PressNum1);
		EnhancedInputComponent->BindAction(InputConfig->CharacterActions[TEXT("Num2")], ETriggerEvent::Triggered, this, &AFPSCharacterPlayer::PressNum2);
	}
}

void AFPSCharacterPlayer::CharacterLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	LookAxisVector *= 0.5f;
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AFPSCharacterPlayer::CharacterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (TObjectPtr<class UFPSPlayerAniminstance> PlayerAnim = Cast<UFPSPlayerAniminstance>(GetMesh()->GetAnimInstance())) {
		PlayerAnim->SetCharacterMovementInputVector(MovementVector);
	}
	
	if (TObjectPtr<class UFPSPlayerAniminstance> ShadowAnim = Cast<UFPSPlayerAniminstance>(ShadowMesh->GetAnimInstance())) {
		ShadowAnim->SetCharacterMovementInputVector(MovementVector);
	}
	
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AFPSCharacterPlayer::StartSprint(const FInputActionValue& Value)
{
	if (SelectedWeapon->isReloading() || SelectedWeapon->isFiring() || bIsAiming || bIsSwaping)
		return;

	if (StaminarPoint <= 0 || bJogCoolDown)
		return;
	
	GetCharacterMovement()->MaxWalkSpeed = MovementJogSpeed;
}

void AFPSCharacterPlayer::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = MovementWalkSpeed;
}

void AFPSCharacterPlayer::Attack(const FInputActionValue& Value)
{
	if (!(SelectedWeapon->isReloading() || SelectedWeapon->isFiring())) {
		StopSprint(Value);
		SelectedWeapon->StartFiring();
	}
}

void AFPSCharacterPlayer::StopFiring(const FInputActionValue& Value) {
	SelectedWeapon->StopFiring();
}

void AFPSCharacterPlayer::Reload(const FInputActionValue& Value)
{	
	if (bIsSwaping)
		return;

	if (SelectedWeapon->isNoAmmo() || SelectedWeapon->isFullAmmo())
		return;

	StopSprint(Value);
	StopAiming(Value);
	SelectedWeapon->Reload();
	ResetRecoil();
}

void AFPSCharacterPlayer::StartAiming(const FInputActionValue& Value)
{
	if (SelectedWeapon->isReloading() || bIsSwaping)
		return;

	if (bIsAiming)
		return;

	bIsAiming = true;
	PlayerArmAnimInstance->Montage_Stop(0.15f, SelectedWeapon->GetFireAnim());
	GetCharacterMovement()->MaxWalkSpeed = MovementAimingSpeed;
	SelectedWeapon->SetAimed();
	AimingTimeline.PlayFromStart();
	
}

void AFPSCharacterPlayer::StopAiming(const FInputActionValue& Value)
{
	if (SelectedWeapon->isReloading())
		return;

	bIsAiming = false;
	PlayerArmAnimInstance->Montage_Stop(0.15f, SelectedWeapon->GetADSFireAnim());
	GetCharacterMovement()->MaxWalkSpeed = MovementWalkSpeed;
	SelectedWeapon->UnAim();
	AimingTimeline.Reverse();
}

void AFPSCharacterPlayer::PressNum1(const FInputActionValue& Value)
{
	if (SelectedWeapon->GetWeaponType() == EWeaponSlot::Primary || bIsSwaping)
		return;

	if (SelectedWeapon->isReloading())
		return;

	TObjectPtr<AFPSWeaponBase>* TargetWeapon = OwningWeapons.Find(EWeaponSlot::Primary);

	if (TargetWeapon) {
		if(bIsAiming)
			StopAiming(Value);

		bIsSwaping = true;
		PlayerArmAnimInstance->Montage_Play((*TargetWeapon)->GetEquipAnim(), 1.0f);
		FOnMontageEnded EquipEndDelegate;
		EquipEndDelegate.BindLambda(
			[this](UAnimMontage* Montage, bool bInterrupted)
			{
				AttachWeapon("RifleSocket");
				bIsSwaping = false;
			}
		);
		PlayerArmAnimInstance->Montage_SetEndDelegate(EquipEndDelegate, (*TargetWeapon)->GetEquipAnim());
	}
}

void AFPSCharacterPlayer::PressNum2(const FInputActionValue& Value)
{
	if (SelectedWeapon->GetWeaponType() == EWeaponSlot::Secondary || bIsSwaping)
		return;

	if (SelectedWeapon->isReloading())
		return;

	TObjectPtr<AFPSWeaponBase>* TargetWeapon = OwningWeapons.Find(EWeaponSlot::Secondary);
	
	if (TargetWeapon) {
		if (bIsAiming)
			StopAiming(Value);

		bIsSwaping = true;
		PlayerArmAnimInstance->Montage_Play((*TargetWeapon)->GetEquipAnim(), 1.0f);
		FOnMontageEnded EquipEndDelegate;
		EquipEndDelegate.BindLambda(
			[this](UAnimMontage* Montage, bool bInterrupted)
			{
				AttachWeapon("WeaponSocket");
				bIsSwaping = false;
			}
		);
		PlayerArmAnimInstance->Montage_SetEndDelegate(EquipEndDelegate, (*TargetWeapon)->GetEquipAnim());
	}
}

void AFPSCharacterPlayer::SwapWeapon(EWeaponSlot WeaponType)
{
	TObjectPtr<AFPSWeaponBase>* TargetWeapon = OwningWeapons.Find(WeaponType);

	if (TargetWeapon) {
		SelectedWeapon->OwnerReloadingAction.Unbind();
		
		//AnimMontage를 실행, Notify를 통해 캐릭터에 부착하고
		SelectedWeapon->SetActorHiddenInGame(true);
		SelectedWeapon = *TargetWeapon;
		
		USkeletalMeshComponent* NewWeaponMesh = SelectedWeapon->GetWeaponMesh();
		if (NewWeaponMesh) 
		{
			WeaponShadowMesh->SetSkeletalMesh(NewWeaponMesh->GetSkeletalMeshAsset());
			// 무기 그림자가 실제 무기 액터의 애니메이션을 따라가도록 설정
			WeaponShadowMesh->SetLeaderPoseComponent(NewWeaponMesh);
		}
		
		MainHUD->ChangeGunHUD((int32)WeaponType);
		if (WeaponType == EWeaponSlot::Primary) {
			AttachWeapon("RifleSocket");
		}
		else if (WeaponType == EWeaponSlot::Secondary) {
			AttachWeapon("WeaponSocket");
		}
		
		SelectedWeapon->SetupAmmoHUD(MainHUD);
		SelectedWeapon->OwnerReloadingAction.BindUObject(this, &AFPSCharacterPlayer::Reload);
		SelectedWeapon->SetActorHiddenInGame(false);
		ResetRecoil();

		AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(GetController());
		
		if (Controller) {
			FPSPlayerController->ChangeWeapon();
		}
		//End Event로 Rifle 소켓에 부착하는 처리를 한다.
	}
}

void AFPSCharacterPlayer::ReceiveGunRecoil(float VerticalRecoil, float HorizontalRecoil)
{
	TargetVerticalRecoil += VerticalRecoil;
	TargetHorizontalRecoil += HorizontalRecoil;
}

void AFPSCharacterPlayer::SetLevel(int32 Round)
{
	int CurrentRound = FMath::Clamp(Round, 1, UFPSZombieSingleton::Get().MaxRound);

	FFPSZombieRoundInfo RoundInfo = UFPSZombieSingleton::Get().GetRoundInfo(CurrentRound);
	
	TObjectPtr<AFPSWeaponBase> PrimaryWeapon = *OwningWeapons.Find(EWeaponSlot::Primary);
	PrimaryWeapon->GetAmmo(RoundInfo.PrimaryAmmo);
	
	TObjectPtr<AFPSWeaponBase> SecondaryWeapon = *OwningWeapons.Find(EWeaponSlot::Secondary);
	SecondaryWeapon->GetAmmo(RoundInfo.SecondaryAmmo);

	SelectedWeapon->ValidateHUD();
}

void AFPSCharacterPlayer::FiringCamShake()
{
	AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(GetController());

	if (Controller) {
		FPSPlayerController->FiringCamShake();
	}
}

void AFPSCharacterPlayer::SetupMeshAndShadows()
{
	ShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shadow"));
	ShadowMesh->SetupAttachment(RootComponent);

	WeaponShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponShadow"));
	WeaponShadowMesh->SetupAttachment(ShadowMesh, TEXT("WeaponSocket"));
	WeaponShadowMesh->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));
	WeaponShadowMesh->SetRenderInMainPass(false);

	GetMesh()->SetRelativeLocationAndRotation(FVector(-25.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	ShadowMesh->SetRelativeLocationAndRotation(FVector(-25.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

	GetMesh()->SetCastShadow(false);
	ShadowMesh->SetRenderInMainPass(false);

	PlayerArmSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerArm"));
	PlayerArmSkeletal->SetupAttachment(FPSCamera);
	PlayerArmSkeletal->SetRelativeLocationAndRotation(FVector(-5.0f, 0.0f, -167.5f), FRotator(0.0f, -90.0f, 0.0f));
	PlayerArmSkeletal->SetCastShadow(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/FPSZombie/Characters/Player/Modular_001_Bergs09/Mesh/SKM_Bergs09_1.SKM_Bergs09_1'"));
	if (PlayerMeshRef.Object) {
		GetMesh()->SetSkeletalMesh(PlayerMeshRef.Object);
		PlayerArmSkeletal->SetSkeletalMesh(PlayerMeshRef.Object);
		ShadowMesh->SetSkeletalMesh(PlayerMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> PlayerAnimRef(TEXT("/Script/Engine.AnimBlueprint'/Game/FPSZombie/Blueprints/BP_FPSPlayerAnim.BP_FPSPlayerAnim_C'"));
	if (PlayerAnimRef.Class) {
		GetMesh()->SetAnimInstanceClass(PlayerAnimRef.Class);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> PlayerArmAnimRef(TEXT("/Script/Engine.AnimBlueprint'/Game/FPSZombie/Blueprints/BP_FPSArmAnim.BP_FPSArmAnim_C'"));
	if (PlayerArmAnimRef.Class) {
		PlayerArmSkeletal->SetAnimInstanceClass(PlayerArmAnimRef.Class);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> ShadowAnimRef(TEXT("/Script/Engine.AnimBlueprint'/Game/FPSZombie/Blueprints/BP_ShadowAnim.BP_ShadowAnim_C'"));
	if (ShadowAnimRef.Class) {
		ShadowMesh->SetAnimInstanceClass(ShadowAnimRef.Class);
		GetMesh()->SetAnimInstanceClass(ShadowAnimRef.Class);
	}
}

void AFPSCharacterPlayer::SetupDefaultMovementAndInput()
{
	GetCharacterMovement()->GravityScale = MovementGravityScale;
	GetCharacterMovement()->JumpZVelocity = MovementJumpZVelocity;
	GetCharacterMovement()->MaxWalkSpeed = MovementWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MovementWalkSpeedCrouch;

	static ConstructorHelpers::FObjectFinder<UFPSCharacterInputData> InputConfigRef(TEXT("/Script/FPSZombie.FPSCharacterInputData'/Game/FPSZombie/GameData/FPSInputData.FPSInputData'"));
	if (nullptr != InputConfigRef.Object) {
		InputConfig = InputConfigRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> AimingCurveRef(TEXT("/Script/Engine.CurveFloat'/Game/FPSZombie/GameData/WeaponData/Timeline_CharacterAiming.Timeline_CharacterAiming'"));
	if (AimingCurveRef.Succeeded()) {
		AimingCurve = AimingCurveRef.Object;
	
		FOnTimelineFloat AimingCallback;
		AimingCallback.BindUFunction(this, FName("AimUpdate"));

		AimingTimeline.AddInterpFloat(AimingCurve, AimingCallback);
	}
}

void AFPSCharacterPlayer::AimUpdate(float Alpha)
{
	float CameraFOV = FMath::Lerp(90, 65, Alpha);
	FPSCamera->FieldOfView = CameraFOV;
}

void AFPSCharacterPlayer::ResetRecoil()
{
	CurrentVerticalRecoil = 0.0f;
	CurrentHorizontalRecoil = 0.0f;

	TargetVerticalRecoil = 0.0f;
	TargetHorizontalRecoil = 0.0f;
}

void AFPSCharacterPlayer::GetRebound(float DeltaTime)
{
	float TempVerticalRecoil = FMath::FInterpTo(CurrentVerticalRecoil, TargetVerticalRecoil, DeltaTime, 10.0f);
	float VerticalDelta = (TempVerticalRecoil - CurrentVerticalRecoil) * -1.0f;

	CurrentVerticalRecoil = TempVerticalRecoil;
	AddControllerPitchInput(VerticalDelta);

	float TempHorizontalRecoil = FMath::FInterpTo(CurrentHorizontalRecoil, TargetHorizontalRecoil, DeltaTime, 10.0f);
	float HorizontalDelta = TempHorizontalRecoil - CurrentHorizontalRecoil;
	CurrentHorizontalRecoil = TempHorizontalRecoil;
	AddControllerYawInput(HorizontalDelta);
}

void AFPSCharacterPlayer::HideHeadandArmMesh()
{
	GetMesh()->HideBoneByName(TEXT("spine_05"), EPhysBodyOp::PBO_None);

	PlayerArmSkeletal->HideBoneByName(TEXT("neck_01"), EPhysBodyOp::PBO_None);
	PlayerArmSkeletal->HideBoneByName(TEXT("thigh_l"), EPhysBodyOp::PBO_None);
	PlayerArmSkeletal->HideBoneByName(TEXT("thigh_r"), EPhysBodyOp::PBO_None);
	PlayerArmAnimInstance = Cast<UFPSPlayerAniminstance>(PlayerArmSkeletal->GetAnimInstance());
}

void AFPSCharacterPlayer::AttachWeapon()
{
	UWorld* World = GetWorld();
	if (World) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		FVector Location = GetActorLocation();
		FRotator Rotation = FRotator::ZeroRotator;

		TObjectPtr<class AFPS_Weapon_Rifle> PrimaryWeapon = GetWorld()->SpawnActor<AFPS_Weapon_Rifle>(AFPS_Weapon_Rifle::StaticClass(), Location, Rotation, SpawnParams);
		if (PrimaryWeapon) {
			PrimaryWeapon->AttachToComponent(PlayerArmSkeletal, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RifleSocket"));
			
			WeaponShadowMesh->SetSkeletalMesh(PrimaryWeapon->GetWeaponMesh()->GetSkeletalMeshAsset());
			// 리더 포즈를 스폰된 무기의 메쉬로 설정
			WeaponShadowMesh->SetLeaderPoseComponent(PrimaryWeapon->GetWeaponMesh());
			
			PrimaryWeapon->SetActorHiddenInGame(true);
			//SelectedWeapon = PrimaryWeapon;
			//SelectedWeapon->SetupAmmoHUD(MainHUD);
			//SelectedWeapon->OwnerReloadingAction.BindUObject(this, &AFPSCharacterPlayer::Reload);
		}

		OwningWeapons.Add(PrimaryWeapon->GetWeaponType(), PrimaryWeapon);

		TObjectPtr<class AFPS_WeaponPistol> SecondaryWeapon = GetWorld()->SpawnActor<AFPS_WeaponPistol>(AFPS_WeaponPistol::StaticClass(), Location, Rotation, SpawnParams);
		if (SecondaryWeapon) {
			SecondaryWeapon->AttachToComponent(PlayerArmSkeletal, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
			// 외형 설정
			WeaponShadowMesh->SetSkeletalMesh(SecondaryWeapon->GetWeaponMesh()->GetSkeletalMeshAsset());
			// 리더 포즈를 스폰된 무기의 메쉬로 설정
			WeaponShadowMesh->SetLeaderPoseComponent(SecondaryWeapon->GetWeaponMesh());

			SelectedWeapon = SecondaryWeapon;
			SelectedWeapon->SetupAmmoHUD(MainHUD);
			MainHUD->ChangeGunHUD((int32)EWeaponSlot::Secondary);
			SelectedWeapon->OwnerReloadingAction.BindUObject(this, &AFPSCharacterPlayer::Reload);
		}

		OwningWeapons.Add(SecondaryWeapon->GetWeaponType(), SecondaryWeapon);
		
	}

	World = nullptr;
}

void AFPSCharacterPlayer::HandleRoundStarted(int32 RoundIndex)
{
	SetLevel(RoundIndex);
}

void AFPSCharacterPlayer::SetupPlayerAudioCopmonents()
{
	PlayerAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerAudioComponent"));
	PlayerAudioComponent->SetupAttachment(RootComponent);
	PlayerAudioComponent->bAutoActivate = true;
	PlayerAudioComponent->bAllowSpatialization = true;
	PlayerAudioComponent->bOverrideAttenuation = true;

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SoundAttenuationDataRef(TEXT("/Script/Engine.SoundAttenuation'/Game/FPSZombie/GameData/SA_ImpactAttenuation.SA_ImpactAttenuation'"));
	if (SoundAttenuationDataRef.Succeeded()) {
		AudioSoundAttenuationSetting = SoundAttenuationDataRef.Object;
	}
	
	PlayerAudioComponent->AttenuationSettings = AudioSoundAttenuationSetting;
}

void AFPSCharacterPlayer::PlaySound(USoundBase* TargetSound, float StartTime)
{
	PlayerAudioComponent->SetSound(TargetSound);
	PlayerAudioComponent->Play(StartTime);
}

void AFPSCharacterPlayer::AttachWeapon(FName SocketName)
{
	SelectedWeapon->AttachToComponent(PlayerArmSkeletal, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AFPSCharacterPlayer::SetupMainHUD(UFPSMainWidget* Widget)
{
	UFPSMainWidget* MainHUDWidget = Cast<UFPSMainWidget>(Widget);
	if (MainHUDWidget) {
		// 체력과 스테미너, 탄약 HUD 바인딩
		OnStaminarChanged.BindUObject(MainHUDWidget, &UFPSMainWidget::UpdateStaminarHUD);
		MainHUD = MainHUDWidget;
	}
}

float AFPSCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HP > 0) {
		HP -= Damage;
		AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(GetController());
		if (PlayerController) {
			PlayerController->ShakeCamera();
		}

		if (MainHUD)
		{
			MainHUD->UpdateHealthHUD(HP);
		}

		if (HP <= 0.0f)
		{
			if (AFPSZombieGameMode* GameMode = Cast<AFPSZombieGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				GameMode->GameOver();
			}

			/*DisableInput(PlayerController);
			SetActorEnableCollision(false);*/
		}
	}

	return Damage;
}