using Kwerty.DviZe.Workers;
using System;
using System.Threading.Tasks;

namespace Kwerty.DviZe.Win.Hooks;

internal sealed class InputHookRegistration(InputHandler handler, int priority, InputHookOptions options, IWorkerProvider<InputHookSession> sessionProvider)
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

        await session.RegisterHandlerAsync(handler, priority).ConfigureAwait(false);
    }

    protected override async Task OnStoppingAsync()
    {
        await session.UnregisterHandlerAsync(handler).ConfigureAwait(false);

        sessionReleaser.Dispose();
    }

    void IDisposable.Dispose() => Context.TryStop();
}
