// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayerController.h"
#include "Blueprint/UserWidget.h"

ADEPlayerController::ADEPlayerController()
{
}

void ADEPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}