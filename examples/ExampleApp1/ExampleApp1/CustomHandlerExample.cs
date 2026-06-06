using Kwerty.DviZe.Win.Hooks;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ExampleApp1;

public class CustomHandlerExample(InputHook inputHook, ILogger<CustomHandlerExample> logger) : IHostedService
{
    IDisposable subscription;

    public async Task StartAsync(CancellationToken cancellationToken)
    {
        try
        {
            subscription = await inputHook.RegisterHandlerAsync(new CustomInputHandler(), cancellationToken);

            logger.LogInformation("Custom handler installed; hold left mouse button to disable keyboard events.");
        }
        catch (InvalidOperationException)
        {
            logger.LogError("Hook not installed, or both hooks not enabled.");
        }
    }

    public Task StopAsync(CancellationToken cancellationToken)
    {
        subscription?.Dispose();
        return Task.CompletedTask;
    }
}
