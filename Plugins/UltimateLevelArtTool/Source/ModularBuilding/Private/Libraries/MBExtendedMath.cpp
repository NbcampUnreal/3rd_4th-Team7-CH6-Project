// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Libraries/MBExtendedMath.h"


float UMBExtendedMath::GetAxisOfVector(const EMBAxis InAxis, const FVector& InVector)
{		if(InAxis == EMBAxis::AxisX)
	{
		return InVector.X;
	}
	else if(InAxis == EMBAxis::AxisY)
	{
		return InVector.Y;
	}
	else
	{
		return InVector.Z;
	}
}

void UMBExtendedMath::SetAxisOfVector(const EMBAxis InAxis, FVector& InVector, const float& InNewValue)
{
	if(InAxis == EMBAxis::AxisX)
	{
		InVector.X = InNewValue;
	}
	else if(InAxis == EMBAxis::AxisY)
	{
		InVector.Y = InNewValue;
	}
	else
	{
		InVector.Z = InNewValue;
	}
}

void UMBExtendedMath::GetHighestAxisAndDirectionOfVector(const FVector& InVector, EMBAxis& OutAxis, bool& OutDirection)
{
	TArray<float> Values;
	Values.Add(InVector.X);
	Values.Add(InVector.Y);
	Values.Add(InVector.Z);

	float HValue = 0;
	uint8 FoundIndex = 0;
	
	for(uint8 i  = 0 ; i < 3 ; i++)
	{
		if(FMath::Abs(Values[i]) > HValue)
		{
			FoundIndex = i;
			HValue = FMath::Abs(Values[i]);
		}
	}
	if(FoundIndex == 0)
	{
		OutAxis = EMBAxis::AxisX;
	}
	else if(FoundIndex == 1)
	{
		OutAxis = EMBAxis::AxisY;
	}
	else
	{
		OutAxis = EMBAxis::AxisZ;
	}
	OutDirection = Values[FoundIndex] > 0;
}

EMBAxis UMBExtendedMath::GetSecAxis(const EMBAxis& InWorkingAxis, const EMBAxis& InBlockedAxis)
{
	TArray<EMBAxis> AxisTable;
	AxisTable.Add(EMBAxis::AxisX);
	AxisTable.Add(EMBAxis::AxisY);
	AxisTable.Add(EMBAxis::AxisZ);
	
	for(const auto Current :AxisTable)
	{
		if((Current != InWorkingAxis) && (Current != InBlockedAxis))
		{
			return Current;
		}
	}
	return EMBAxis::None;
}


FVector UMBExtendedMath::GetAxisVectorWithEnumValue(const EMBAxis& InAxis)
{
	switch (InAxis) {
	case EMBAxis::None:  return FVector::ZeroVector;
	case EMBAxis::AxisX: return FVector::XAxisVector;
	case EMBAxis::AxisY: return FVector::YAxisVector;
	case EMBAxis::AxisZ: return FVector::ZAxisVector;
	default:			 return FVector::ZeroVector;
	}
}

EMBAxis UMBExtendedMath::GetAxisEnumOfDirectionVector(const FVector& InVector)
{
	if(InVector.X == 1)
	{
		return EMBAxis::AxisX;
	}
	if(InVector.Y == 1)
	{
		return EMBAxis::AxisY;
	}
	if(InVector.Z == 1)
	{
		return EMBAxis::AxisZ;
	}
	return EMBAxis::None;
}

void UMBExtendedMath::GetNearestAxisOfVector(const FVector& InVector, EMBAxis& OutAxis,FVector& OutDirection)
{
	const float MaxAbs = InVector.GetAbs().GetMax();
	if(FMath::IsNearlyEqual(MaxAbs, InVector.GetAbs().X,0.001f))
	{
		OutDirection = FVector::XAxisVector * FMath::Sign(InVector.X);
		OutAxis = EMBAxis::AxisX;
	}
	else if(FMath::IsNearlyEqual(MaxAbs, InVector.GetAbs().Y,0.001f))
	{
		OutDirection = FVector::YAxisVector * FMath::Sign(InVector.Y);
		OutAxis = EMBAxis::AxisY;
	}
	else
	{
		OutDirection = FVector::ZAxisVector * FMath::Sign(InVector.Z);;
		OutAxis = EMBAxis::AxisZ;
	}
}
