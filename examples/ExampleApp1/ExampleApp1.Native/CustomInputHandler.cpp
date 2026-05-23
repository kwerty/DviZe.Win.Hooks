#include "pch.h"
#include <Windows.h>

namespace
{
    struct HandlerState
    {
        bool leftButtonDown;
    };
}

static bool KeyboardProc(UINT msg, KBDLLHOOKSTRUCT* param, void* state)
{
    if (param->flags & LLKHF_INJECTED)
    {
        return false;
    }

    auto s = static_cast<HandlerState*>(state);

    return s->leftButtonDown;
}

static bool MouseProc(UINT msg, MSLLHOOKSTRUCT* param, void* state)
{
    if (param->flags & LLMHF_INJECTED)
    {
        return false;
    }

    auto s = static_cast<HandlerState*>(state);

    if (msg == WM_LBUTTONDOWN)
    {
        s->leftButtonDown = true;
    }
    else if (msg == WM_LBUTTONUP)
    {
        s->leftButtonDown = false;
    }

    return false;
}

namespace ExampleApp1
{
    public ref class CustomInputHandler sealed : public Kwerty::DviZe::Win::Hooks::InputHandler
    {
        HandlerState* pState;

    public:
        CustomInputHandler()
            : InputHandler(::KeyboardProc, ::MouseProc)
        {
        }

    protected:
        virtual void* OnInitialising() override
        {
            pState = new HandlerState
            {
                .leftButtonDown = false,
            };
            return pState;
        }

        virtual void OnDeinitialising() override
        {
            delete pState;
        }
    };
}
