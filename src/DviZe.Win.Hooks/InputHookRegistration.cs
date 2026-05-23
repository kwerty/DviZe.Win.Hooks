using Kwerty.DviZe.Workers;
using System;
using System.Threading.Tasks;

namespace Kwerty.DviZe.Win.Hooks;

internal sealed class InputHookRegistration(InputHandler handler, int priority, InputHookOptions options, IWorkerProvider<InputHookSession> sessionProvider, IThreadAccessor threadAccessor)
    : Worker, IDisposable
{
    InputHookSession session;
    IDisposable sessionReleaser;

    protected override async Task OnStartingAsync(WorkerStartingContext startingContext)
    {
        if (handler.IsKeyboardHandler
            && !options.Hooks.HasFlag(InputHookTypes.Keyboard))
        {
            throw new InvalidOperationException();
        }

        if (handler.IsMouseHandler
            && !options.Hooks.HasFlag(InputHookTypes.Mouse))
        {
            throw new InvalidOperationException();
        }

        (session, sessionReleaser) = await sessionProvider.LeaseAsync(startingContext.CancellationToken).ConfigureAwait(false);

        try
        {
            await threadAccessor.UIThread;

            if (options.InstallOnDemand)
            {
                if (handler.IsKeyboardHandler)
                {
                    InputHookNativeExtensions.UseKeyboardHook();
                }

                if (handler.IsMouseHandler)
                {
                    InputHookNativeExtensions.UseMouseHook();
                }
            }

            InputHookNativeExtensions.RegisterHandler(handler, session.id, priority);
        }
        catch
        {
            sessionReleaser.Dispose();
            throw;
        }
    }

    protected override async Task OnStoppingAsync()
    {
        try
        {
            await threadAccessor.UIThread;

            InputHookNativeExtensions.UnregisterHandler(handler);

            if (options.InstallOnDemand)
            {
                if (handler.IsKeyboardHandler)
                {
                    InputHookNativeExtensions.ReleaseKeyboardHook();
                }

                if (handler.IsMouseHandler)
                {
                    InputHookNativeExtensions.ReleaseMouseHook();
                }
            }
        }
        finally
        {
            sessionReleaser.Dispose();
        }
    }

    void IDisposable.Dispose() => Context.TryStop();
}
