using Kwerty.DviZe.Workers;
using System.Threading.Tasks;

namespace Kwerty.DviZe.Win.Hooks;

internal sealed class InputHookSession(InputHookOptions options, IThreadAccessor threadAccessor) : Worker
{
    static int nextId;
    static int keyboardUserCount;
    static int mouseUserCount;
    static InputHookSafeHandle keyboardHookHandle;
    static InputHookSafeHandle mouseHookHandle;
    readonly int id = nextId++;

    protected override async Task OnStartingAsync(WorkerStartingContext startingContext)
    {
        if (!options.InstallOnDemand)
        {
            await threadAccessor.UIThread;

            if (options.Hooks.HasFlag(InputHookTypes.Keyboard))
            {
                UseKeyboardHook();
            }

            if (options.Hooks.HasFlag(InputHookTypes.Mouse))
            {
                UseMouseHook();
            }
        }
    }

    protected override async Task OnStoppingAsync()
    {
        if (!options.InstallOnDemand)
        {
            await threadAccessor.UIThread;

            if (options.Hooks.HasFlag(InputHookTypes.Keyboard))
            {
                ReleaseKeyboardHook();
            }

            if (options.Hooks.HasFlag(InputHookTypes.Mouse))
            {
                ReleaseMouseHook();
            }
        }
    }

    internal async Task RegisterHandlerAsync(InputHandler handler, int priority)
    {
        await threadAccessor.UIThread;

        if (options.InstallOnDemand)
        {
            if (handler.IsKeyboardHandler)
            {
                UseKeyboardHook();
            }

            if (handler.IsMouseHandler)
            {
                UseMouseHook();
            }
        }

        InputHookNativeExtensions.RegisterHandler(handler, id, priority);
    }

    internal async Task UnregisterHandlerAsync(InputHandler handler)
    {
        await threadAccessor.UIThread;

        InputHookNativeExtensions.UnregisterHandler(handler);

        if (options.InstallOnDemand)
        {
            if (handler.IsKeyboardHandler)
            {
                ReleaseKeyboardHook();
            }

            if (handler.IsMouseHandler)
            {
                ReleaseMouseHook();
            }
        }
    }

    static void UseKeyboardHook()
    {
        if (keyboardUserCount++ == 0)
        {
            keyboardHookHandle = InputHookNativeExtensions.InstallKeyboardHook();
        }
    }

    static void UseMouseHook()
    {
        if (mouseUserCount++ == 0)
        {
            mouseHookHandle = InputHookNativeExtensions.InstallMouseHook();
        }
    }

    static void ReleaseKeyboardHook()
    {
        if (--keyboardUserCount == 0)
        {
            keyboardHookHandle.Dispose();
            keyboardHookHandle = null;
        }
    }

    static void ReleaseMouseHook()
    {
        if (--mouseUserCount == 0)
        {
            mouseHookHandle.Dispose();
            mouseHookHandle = null;
        }
    }
}
