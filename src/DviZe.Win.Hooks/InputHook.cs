using Kwerty.DviZe.Workers;
using Microsoft.Extensions.Logging;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace Kwerty.DviZe.Win.Hooks;

public sealed class InputHook : IAsyncDisposable
{
    readonly InputHookOptions options;
    readonly IThreadAccessor threadAccessor;
    readonly IWorkerProvider<InputHookSession> sessionProvider;
    readonly Runner<InputHookRegistration> registrationRunner;

    public InputHook(IThreadAccessor threadAccessor, ILoggerFactory loggerFactory)
        : this(InputHookOptions.Default, threadAccessor, loggerFactory)
    {
    }

    public InputHook(InputHookOptions options, IThreadAccessor threadAccessor, ILoggerFactory loggerFactory)
    {
        ArgumentNullException.ThrowIfNull(options, nameof(options));
        ArgumentNullException.ThrowIfNull(threadAccessor, nameof(threadAccessor));
        ArgumentNullException.ThrowIfNull(loggerFactory, nameof(loggerFactory));

        this.options = options;
        this.threadAccessor = threadAccessor;
        sessionProvider = options.InstallOnDemand
            ? new OnDemand<InputHookSession>(CreateSession, loggerFactory)
            : new RunSingle<InputHookSession>(loggerFactory);
        registrationRunner = new Runner<InputHookRegistration>(loggerFactory);
    }

    /// <summary>
    /// Installs the keyboard and/or mouse hook. Only valid when <c>InputHookOptions.InstallOnDemand</c> is <c>false</c>.
    /// </summary>
    /// <exception cref="InvalidOperationException" />
    /// <exception cref="ObjectDisposedException" />
    public async Task InstallAsync(CancellationToken cancellationToken = default)
    {
        if (sessionProvider is not RunSingle<InputHookSession> sessionRunner)
        {
            throw new InvalidOperationException();
        }

        await sessionRunner.StartWorkerAsync(CreateSession(), cancellationToken).ConfigureAwait(false);
    }

    /// <summary>
    /// Registers a native handler for processing keyboard and/or mouse events.
    /// </summary>
    /// <remarks>
    /// The built-in handlers are <see cref="DefaultKeyboardHandler"/> and <see cref="DefaultMouseHandler"/>.
    /// Custom handlers deriving from <see cref="InputHandler"/> are also supported.
    /// </remarks>
    /// <returns>
    /// An <see cref="IDisposable"/> which should be disposed to unregister the handler.
    /// </returns>
    /// <exception cref="InvalidOperationException" />
    /// <exception cref="ObjectDisposedException" />
    public Task<IDisposable> RegisterHandlerAsync(InputHandler handler, CancellationToken cancellationToken = default)
        => RegisterHandlerAsync(handler, priority: 0, cancellationToken);

    /// <summary>
    /// Registers a native handler for processing keyboard and/or mouse events.
    /// </summary>
    /// <remarks>
    /// The built-in handlers are <see cref="DefaultKeyboardHandler"/> and <see cref="DefaultMouseHandler"/>.
    /// Custom handlers deriving from <see cref="InputHandler"/> are also supported.
    /// </remarks>
    /// <returns>
    /// An <see cref="IDisposable"/> which should be disposed to unregister the handler.
    /// </returns>
    /// <exception cref="InvalidOperationException" />
    /// <exception cref="ObjectDisposedException" />
    public async Task<IDisposable> RegisterHandlerAsync(InputHandler handler, int priority, CancellationToken cancellationToken = default)
    {
        var registration = new InputHookRegistration(handler, priority, options, sessionProvider);
        await registrationRunner.StartWorkerAsync(registration, cancellationToken).ConfigureAwait(false);
        return registration;
    }

    /// <summary>
    /// Subscribes to keyboard events.
    /// </summary>
    /// <returns>
    /// An <see cref="IDisposable"/> which should be disposed to unsubscribe.
    /// </returns>
    /// <exception cref="InvalidOperationException" />
    /// <exception cref="ObjectDisposedException" />
    public Task<IDisposable> SubscribeAsync(Key key, Action<KeyboardEvent> callback, CancellationToken cancellationToken = default)
        => RegisterHandlerAsync(new DefaultKeyboardHandler(key, keyState: null, callback), cancellationToken);

    /// <summary>
    /// Subscribes to keyboard events.
    /// </summary>
    /// <returns>
    /// An <see cref="IDisposable"/> which should be disposed to unsubscribe.
    /// </returns>
    /// <exception cref="InvalidOperationException" />
    /// <exception cref="ObjectDisposedException" />
    public Task<IDisposable> SubscribeAsync(Key key, KeyState keyState, Action<KeyboardEvent> callback, CancellationToken cancellationToken = default)
        => RegisterHandlerAsync(new DefaultKeyboardHandler(key, keyState, callback), cancellationToken);

    /// <summary>
    /// Subscribes to mouse events.
    /// </summary>
    /// <returns>
    /// An <see cref="IDisposable"/> which should be disposed to unsubscribe.
    /// </returns>
    /// <exception cref="InvalidOperationException" />
    /// <exception cref="ObjectDisposedException" />
    public Task<IDisposable> SubscribeAsync(MouseAction mouseAction, Action<MouseEvent> callback, CancellationToken cancellationToken = default)
        => RegisterHandlerAsync(new DefaultMouseHandler(mouseAction, callback), cancellationToken);

    public async ValueTask DisposeAsync()
    {
        await registrationRunner.DisposeAsync().ConfigureAwait(false);
        await ((IAsyncDisposable)sessionProvider).DisposeAsync().ConfigureAwait(false); // Both OnDemand/RunSingle require async disposal.
    }

    InputHookSession CreateSession() => new(options, threadAccessor);
}