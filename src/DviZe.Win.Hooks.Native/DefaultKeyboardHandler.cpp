#include "pch.h"
#include "main.h"
#include "InputHandler.h"
#include <msclr/gcroot.h>
#include <Windows.h>

using namespace msclr;
using namespace System;
using namespace Kwerty::DviZe::Win;
using namespace Kwerty::DviZe::Win::Hooks;

namespace
{
    const Key nullKey = (Key)-1;
    const KeyState nullKeyState = (KeyState)-1;

    struct HandlerState
    {
        Key key;
        KeyState keyState;
        gcroot<Action<KeyboardEvent^>^> callback;
    };
}

static bool KeyboardProc(UINT msg, KBDLLHOOKSTRUCT* param, void* state)
{
    if (param->flags & LLKHF_INJECTED)
    {
        return false;
    }

    auto s = static_cast<HandlerState*>(state);

    auto keyState = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) ? KeyState::Down : KeyState::Up;

    if (s->keyState != nullKeyState
        && keyState != s->keyState)
    {
        return false;
    }

    auto key = (Key)param->vkCode;

    if (s->key != nullKey
        && key != s->key)
    {
        return false;
    }

    auto evt = gcnew KeyboardEvent(key, keyState);

    s->callback->Invoke(evt);

    return evt->Cancel;
}

namespace Kwerty::DviZe::Win::Hooks
{
    public ref class DefaultKeyboardHandler sealed : public InputHandler
    {
    private:
        initonly Nullable<Key> key;
        initonly Nullable<KeyState> keyState;
        initonly Action<KeyboardEvent^>^ callback;
        HandlerState* pState;

    public:
        DefaultKeyboardHandler(Nullable<Key> key, Nullable<KeyState> keyState, Action<KeyboardEvent^>^ callback)
            : InputHandler(::KeyboardProc, nullptr), key(key), keyState(keyState), callback(callback)
        {
        }

    protected:
        virtual void* OnInitialising() override
        {
            pState = new HandlerState
            {
                .key = key.GetValueOrDefault(nullKey),
                .keyState = keyState.GetValueOrDefault(nullKeyState),
                .callback = (gcroot<Action<KeyboardEvent^>^>)callback,
            };
            return pState;
        }

        virtual void OnDeinitialising() override
        {
            delete pState;
        }
    };
}
