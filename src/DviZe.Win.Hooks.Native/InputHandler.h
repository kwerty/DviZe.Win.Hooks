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
		bool initialized;

	protected:
		InputHandler(void* keyboardProc, void* mouseProc)
			: keyboardProc(keyboardProc), mouseProc(mouseProc)
		{
		}

		virtual void* OnInitializing()
		{
			return nullptr;
		}

		virtual void OnDeinitializing()
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
		void* Initialize()
		{
			return OnInitializing();
		}

		void Deinitialize()
		{
			OnDeinitializing();
		}
	};
}
