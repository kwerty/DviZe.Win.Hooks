#pragma once

#include "windows.h"

#define NAMEOF(name) #name

using KEYBOARDPROC = bool(*)(UINT msg, KBDLLHOOKSTRUCT* param, void* state);
using MOUSEPROC = bool(*)(UINT msg, MSLLHOOKSTRUCT* param, void* state);

namespace Kwerty::DviZe::Win::Hooks
{
    public enum class KeyState
    {
        Down,
        Up,
    };

    public enum class MouseAction
    {
        LeftButtonDown,
        LeftButtonUp,
        RightButtonDown,
        RightButtonUp,
        MiddleButtonDown,
        MiddleButtonUp,
        XButton1Down,
        XButton1Up,
        XButton2Down,
        XButton2Up,
        WheelUp,
        WheelDown,
    };

    public ref class KeyboardEvent sealed
    {
    private:
        initonly Key key;
        initonly KeyState keyState;

    public:
        KeyboardEvent(Key key, KeyState keyState)
            : key(key), keyState(keyState)
        {
        }

        property Key Key
        {
            Kwerty::DviZe::Win::Key get()
            {
                return key;
            }
        }

        property KeyState KeyState
        {
            Kwerty::DviZe::Win::Hooks::KeyState get()
            {
                return keyState;
            }
        }

        property bool Cancel;
    };

    public ref class MouseEvent sealed
    {
    private:
        initonly MouseAction action;

    public:
        MouseEvent(MouseAction action)
            : action(action)
        {
        }

        property MouseAction Action
        {
            MouseAction get()
            {
                return action;
            }
        }

        property bool Cancel;
    };
}