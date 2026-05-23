#include "pch.h"
#include "main.h"
#include "InputHandler.h"
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <algorithm>

using namespace Kwerty::DviZe::Win;
using namespace Kwerty::DviZe::Win::Hooks;
using namespace System;

namespace
{
    struct HandlerInfo
    {
        KEYBOARDPROC keyboardProc;
        MOUSEPROC mouseProc;
        void* state;
        int sessionId;
        int priority;
    };

    UINT keyboardUserCount;
    UINT mouseUserCount;
    HHOOK keyboardHookHandle;
    HHOOK mouseHookHandle;
    std::unordered_map<unsigned int, HandlerInfo> handlers;
    std::vector<const HandlerInfo*> keyboardFastIter;
    std::vector<const HandlerInfo*> mouseFastIter;
}

static LRESULT KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        auto p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        for (const auto* handler : keyboardFastIter)
        {
            if (handler->keyboardProc((UINT)wParam, p, handler->state))
            {
                return 1;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

static LRESULT MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        auto p = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        for (const auto* handler : mouseFastIter)
        {
            if (handler->mouseProc((UINT)wParam, p, handler->state))
            {
                return 1;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

static void RebuildFastIter()
{
    keyboardFastIter = {};
    mouseFastIter = {};

    for (const auto& [_, handler] : handlers)
    {
        if (handler.keyboardProc != nullptr)
        {
            keyboardFastIter.push_back(&handler);
        }

        if (handler.mouseProc != nullptr)
        {
            mouseFastIter.push_back(&handler);
        }
    }

    auto handlerCmp = [](const HandlerInfo* a, const HandlerInfo* b)
    {
        return a->sessionId != b->sessionId
            ? a->sessionId < b->sessionId
            : a->priority < b->priority;
    };

    std::sort(keyboardFastIter.begin(), keyboardFastIter.end(), handlerCmp);
    std::sort(mouseFastIter.begin(), mouseFastIter.end(), handlerCmp);
}

namespace Kwerty::DviZe::Win::Hooks
{
    public ref class InputHookNativeExtensions abstract sealed
    {
    public:
        static void UseKeyboardHook()
        {
            if (keyboardUserCount++ == 0)
            {
                keyboardHookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);
                if (keyboardHookHandle == NULL)
                {
                    throw Win32ExceptionExtensions::FromError(NAMEOF(SetWindowsHookEx), GetLastError());
                }
            }
        }

        static void UseMouseHook()
        {
            if (mouseUserCount++ == 0)
            {
                mouseHookHandle = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);
                if (mouseHookHandle == NULL)
                {
                    throw Win32ExceptionExtensions::FromError(NAMEOF(SetWindowsHookEx), GetLastError());
                }
            }
        }

        static void ReleaseKeyboardHook()
        {
            if (--keyboardUserCount == 0)
            {
                if (UnhookWindowsHookEx(keyboardHookHandle) == 0)
                {
                    throw Win32ExceptionExtensions::FromError(NAMEOF(UnhookWindowsHookEx), GetLastError());
                }
                
                keyboardHookHandle = NULL;
            }
        }

        static void ReleaseMouseHook()
        {
            if (--mouseUserCount == 0)
            {
                if (UnhookWindowsHookEx(mouseHookHandle) == 0)
                {
                    throw Win32ExceptionExtensions::FromError(NAMEOF(UnhookWindowsHookEx), GetLastError());
                }
                
                mouseHookHandle = NULL;
            }
        }

        static void RegisterHandler(InputHandler^ handler, int sessionId, int priority)
        {
            if (handler->initialised)
            {
                throw gcnew InvalidOperationException();
            }

            handler->initialised = true;
            
            auto state = handler->Initialise();

            auto id = handler->id;
            handlers[id] = HandlerInfo
            {
                .keyboardProc = (KEYBOARDPROC)handler->keyboardProc,
                .mouseProc = (MOUSEPROC)handler->mouseProc,
                .state = state,
                .sessionId = sessionId,
                .priority = priority,
            };

            RebuildFastIter();
        }

        static void UnregisterHandler(InputHandler^ handler)
        {
            handler->Deinitialise();

            auto id = (handler->id);
            handlers.erase(id);

            RebuildFastIter();
        }
    };
}
