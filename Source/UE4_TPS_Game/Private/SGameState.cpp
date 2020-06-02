// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"

void ASGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(this->WaveState, OldState);

}

void ASGameState::SetWaveState(EWaveState NewState)
{
	if (GetLocalRole()==ROLE_Authority)
	{
		EWaveState OldState = this->WaveState;
		this->WaveState = NewState;
		WaveStateChanged(this->WaveState, OldState);
	}
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASGameState, WaveState);
}