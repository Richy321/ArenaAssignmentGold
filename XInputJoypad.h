#pragma once
#include <Xinput.h>

namespace Arena
{
	///Handles pooling of a single Xbox controller
	class XInputJoypad
	{
	private:
		unsigned int joypadIndex;
		XINPUT_STATE state;
	public:

		XInputJoypad() : joypadIndex(0) {}
		XInputJoypad(unsigned int JoypadIndex) : joypadIndex(JoypadIndex) {}
		virtual ~XInputJoypad(){ }

		unsigned int GetIndex() { return joypadIndex; }
		XINPUT_STATE GetState(){ return state; }
		
		bool Connected()
		{
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			DWORD Result = XInputGetState(joypadIndex, &state);

			if (Result == ERROR_SUCCESS)
				return true;  //connected
			else
				return false; //non connected
		}

		void Update()
		{
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			XInputGetState(joypadIndex, &state);
		}

		bool LStickInDeadzone()
		{
			short sX = state.Gamepad.sThumbLX;
			short sY = state.Gamepad.sThumbLY;

			if (sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
				sX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				return false;

			if (sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
				sY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				return false;

			return true;
		}

		bool RStickInDeadzone()
		{
			short sX = state.Gamepad.sThumbRX;
			short sY = state.Gamepad.sThumbRY;

			if (sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
				sX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
				return false;

			if (sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
				sY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
				return false;

			return true;
		}

		float LeftStick_X()
		{
			short sX = state.Gamepad.sThumbLX;
			return (static_cast<float>(sX) / 32768.0f);
		}

		float LeftStick_Y()
		{
			short sY = state.Gamepad.sThumbLY;
			return ((float)(sY) / 32768.0f);
		}

		float RightStick_X()
		{
			short sX = state.Gamepad.sThumbRX;
			return ((float)(sX) / 32768.0f);
		}

		float RightStick_Y()
		{
			short sY = state.Gamepad.sThumbRY;
			return ((float)(sY) / 32768.0f);
		}

		float LeftTrigger()
		{
			BYTE Trigger = state.Gamepad.bLeftTrigger;
			if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
				return Trigger / 255.0f;

			return 0.0f;
		}

		float RightTrigger()
		{
			BYTE Trigger = state.Gamepad.bRightTrigger;
			if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
				return Trigger / 255.0f;

			return 0.0f;
		}

		// Vibrate the gamepad (0.0f cancel, 1.0f max speed)
		void Rumble(float a_fLeftMotor = 0.0f, float a_fRightMotor = 0.0f)
		{
			XINPUT_VIBRATION vibrationState;
			ZeroMemory(&vibrationState, sizeof(XINPUT_VIBRATION));

			int iLeftMotor = int(a_fLeftMotor * 65535.0f);
			int iRightMotor = int(a_fRightMotor * 65535.0f);

			vibrationState.wLeftMotorSpeed = iLeftMotor;
			vibrationState.wRightMotorSpeed = iRightMotor;

			XInputSetState(joypadIndex, &vibrationState);
		}
	};
}
