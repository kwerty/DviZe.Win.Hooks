#include "pch.h"
#include "InputHandler.h"
#include "main.h"
#include <msclr/gcroot.h>
#include <Windows.h>

using namespace Kwerty::DviZe::Win::Hooks;
using namespace msclr;
using namespace System;

namespace
{
    const MouseAction nullAction = (MouseAction)-1;

    struct HandlerState
    {
        MouseAction action;
        gcroot<Action<MouseEvent^>^> callback;
    };
}

static bool MouseProc(UINT msg, MSLLHOOKSTRUCT* param, void* state)
{
    if (param->flags & LLMHF_INJECTED)
    {
        return false;
    }

    MouseAction action;

    switch (msg)
    {
    case WM_LBUTTONDOWN:
        action = MouseAction::LeftButtonDown;
        break;
    case WM_LBUTTONUP:
        action = MouseAction::LeftButtonUp;
        break;
    case WM_RBUTTONDOWN:
        action = MouseAction::RightButtonDown;
        break;
    case WM_RBUTTONUP:
        action = MouseAction::RightButtonUp;
        break;
    case WM_MBUTTONDOWN:
        action = MouseAction::MiddleButtonDown;
        break;
    case WM_MBUTTONUP:
        action = MouseAction::MiddleButtonUp;
        break;
    case WM_XBUTTONDOWN:
        action = GET_XBUTTON_WPARAM(param->mouseData) == XBUTTON1
            ? MouseAction::XButton1Down
            : MouseAction::XButton2Down;
        break;
    case WM_XBUTTONUP:
        action = GET_XBUTTON_WPARAM(param->mouseData) == XBUTTON1
            ? MouseAction::XButton1Up
            : MouseAction::XButton2Up;
        break;
    case WM_MOUSEWHEEL:
        action = GET_WHEEL_DELTA_WPARAM(param->mouseData) > 0
            ? MouseAction::WheelUp
            : MouseAction::WheelDown;
        break;
    default:
        return false;
    }

    auto s = static_cast<HandlerState*>(state);

    if (s->action != nullAction
        && action != s->action)
    {
        return false;
    }

    auto evt = gcnew MouseEvent(action);

    s->callback->Invoke(evt);

    return evt->Cancel;
}

namespace Kwerty::DviZe::Win::Hooks
{
    public ref class DefaultMouseHandler sealed : public InputHandler
    {
    private:
        initonly Nullable<MouseAction> action;
        initonly Action<MouseEvent^>^ callback;
        HandlerState* pState;

    public:
        DefaultMouseHandler(Nullable<MouseAction> action, Action<MouseEvent^>^ callback)
            : InputHandler(nullptr, ::MouseProc), action(action), callback(callback)
        {
        }

    protected:
        virtual void* OnInitialising() override
        {
            pState = new HandlerState
            {
                .action = action.GetValueOrDefault(nullAction),
                .callback = (gcroot<Action<MouseEvent^>^>)callback,
            };
            return pState;
        }

        virtual void OnDeinitialising() override
        {
            delete pState;
        }
    };
}
