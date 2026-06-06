#include "pch.h"
#include "InputHandler.h"
#include "main.h"
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <Windows.h>

using namespace Kwerty::DviZe::Win;
using namespace Kwerty::DviZe::Win::Hooks;
using namespace Microsoft::Win32::SafeHandles;
using namespace System;

using KEYBOARDPROC = bool(*)(UINT msg, KBDLLHOOKSTRUCT* param, void* state);
using MOUSEPROC = bool(*)(UINT msg, MSLLHOOKSTRUCT* param, void* state);

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
    private ref class InputHookSafeHandle sealed : public SafeHandleZeroOrMinusOneIsInvalid
    {
    public:
        InputHookSafeHandle(HHOOK hook)
            : SafeHandleZeroOrMinusOneIsInvalid(true)
        {
            SetHandle(IntPtr(hook));
        }

    protected:
        virtual bool ReleaseHandle() override
        {
            auto h = reinterpret_cast<HHOOK>(handle.ToPointer());
            return UnhookWindowsHookEx(h);
        }
    };

    private ref class InputHookNativeExtensions abstract sealed
    {
    public:
        static InputHookSafeHandle^ InstallKeyboardHook()
        {
            auto keyboardHookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);
            if (keyboardHookHandle == NULL)
            {
                throw Win32ExceptionExtensions::FromError(NAMEOF(SetWindowsHookEx), GetLastError());
            }
            return gcnew InputHookSafeHandle(keyboardHookHandle);
        }

        static InputHookSafeHandle^ InstallMouseHook()
        {
            auto mouseHookHandle = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);
            if (mouseHookHandle == NULL)
            {
                throw Win32ExceptionExtensions::FromError(NAMEOF(SetWindowsHookEx), GetLastError());
            }
            return gcnew InputHookSafeHandle(mouseHookHandle);
        }

        static void RegisterHandler(InputHandler^ handler, int sessionId, int priority)
        {
            if (handler->initialized)
            {
                throw gcnew InvalidOperationException();
            }

            handler->initialized = true;
            
            auto state = handler->Initialize();

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
            handler->Deinitialize();

            auto id = (handler->id);
            handlers.erase(id);

            RebuildFastIter();
        }
    };
}
