using Kwerty.DviZe.Win;
using Kwerty.DviZe.Win.Hooks;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ExampleApp1;

public class KeyboardExample(InputHook inputHook, ILogger<KeyboardExample> logger) : IHostedService
{
    IDisposable subscription;

    public async Task StartAsync(CancellationToken cancellationToken)
    {
        try
        {
            subscription = await inputHook.SubscribeAsync(Key.K, KeyState.Up, HandleKeyboardEvent, cancellationToken);

            logger.LogInformation("Subscribed to 'K' keyup events.");
        }
        catch (InvalidOperationException)
        {
            logger.LogError("Hook not installed, or keyboard hook not enabled.");
        }
    }

    void HandleKeyboardEvent(KeyboardEvent evt)
    {
        logger.LogInformation("Keyboard event! {key} {keyState}.", evt.Key, evt.KeyState);
    }

    public Task StopAsync(CancellationToken cancellationToken)
    {
        subscription?.Dispose();
        return Task.CompletedTask;
    }
}
