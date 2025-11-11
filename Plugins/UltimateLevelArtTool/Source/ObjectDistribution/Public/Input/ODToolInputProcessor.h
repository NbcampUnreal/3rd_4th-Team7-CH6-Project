// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Framework/Application/IInputProcessor.h"
#include "Input/Events.h"

class FODKeyInputPreProcessor : public IInputProcessor
{
public:
	FODKeyInputPreProcessor()
	{
		
	}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		const bool KeyReturn = HandleKey(InKeyEvent.GetKey(),false);
		return KeyReturn;
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		const bool KeyReturn = HandleKey(InKeyEvent.GetKey(),true);
		return KeyReturn;
	}

	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		//HandleKey(MouseEvent.GetEffectingButton());
		return false;
	}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		const bool KeyReturn = HandleKey(MouseEvent.GetEffectingButton(),true);
		return KeyReturn;
	}

	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		const bool KeyReturn = HandleKey(MouseEvent.GetEffectingButton(),false);
		return KeyReturn;
	}

	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override
	{
		if (InWheelEvent.GetWheelDelta() != 0)
		{
			const FKey Key = InWheelEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;
			const bool KeyReturn = HandleKey(Key,true);
			return KeyReturn;
		}
		return false;
	}

	DECLARE_DELEGATE_RetVal_OneParam(bool,FSettingsPressAnyKeyInputPreProcessorKeySelected, FKey);
	FSettingsPressAnyKeyInputPreProcessorKeySelected OnKeySelected;

	DECLARE_DELEGATE_RetVal_OneParam(bool,FSettingsPressAnyKeyInputPreProcessorCanceled, FKey);
	FSettingsPressAnyKeyInputPreProcessorCanceled OnKeyCanceled;

private:
	bool HandleKey(const FKey& Key, const bool bIsPressed) const
	{
		if(bIsPressed)
		{
			return OnKeySelected.Execute(Key);
		}
		else
		{
			return OnKeyCanceled.Execute(Key);
		}
	}
};
