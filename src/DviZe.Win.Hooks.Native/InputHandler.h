#pragma once

namespace Kwerty::DviZe::Win::Hooks
{
	public ref class InputHandler abstract
	{
	private:
		static unsigned int nextId;

	internal:
		initonly unsigned int id = nextId++;
		initonly void* keyboardProc;
		initonly void* mouseProc;
		bool initialised;

	protected:
		InputHandler(void* keyboardProc, void* mouseProc)
			: keyboardProc(keyboardProc), mouseProc(mouseProc)
		{
		}

		virtual void* OnInitialising()
		{
			return nullptr;
		}

		virtual void OnDeinitialising()
		{
		}

	public:
		property bool IsKeyboardHandler
		{
			bool get()
			{
				return keyboardProc != nullptr;
			}
		}

		property bool IsMouseHandler
		{
			bool get()
			{
				return mouseProc != nullptr;
			}
		}

	internal:
		void* Initialise()
		{
			return OnInitialising();
		}

		void Deinitialise()
		{
			OnDeinitialising();
		}
	};
}
