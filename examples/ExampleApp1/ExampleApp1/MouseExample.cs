using Kwerty.DviZe.Win.Hooks;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ExampleApp1;

public class MouseExample(InputHook inputHook, ILogger<MouseExample> logger) : IHostedService
{
    IDisposable subscription;

    public async Task StartAsync(CancellationToken cancellationToken)
    {
        try
        {
            subscription = await inputHook.SubscribeAsync(MouseAction.RightButtonUp, HandleMouseEvent, cancellationToken);

            logger.LogInformation("Subscribed to right button up events.");
        }
        catch (InvalidOperationException)
        {
            logger.LogError("Hook not installed, or mouse hook not enabled.");
        }
    }

    void HandleMouseEvent(MouseEvent evt)
    {
        logger.LogInformation("Mouse event! {action}", evt.Action);
    }

    public Task StopAsync(CancellationToken cancellationToken)
    {
        subscription?.Dispose();
        return Task.CompletedTask;
    }
}
