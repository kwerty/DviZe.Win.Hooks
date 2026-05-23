# DviZe.Win.Hooks

Low-latency keyboard and mouse hooks for .NET console and non-UI applications.

Events are processed in native code, only calling back to managed code when needed.

Custom handlers are first-class, enabling you to write your own native event handling code.

Targets .NET 10. Written in C# and C++/CLI.

```csharp
var subscription = await inputHook.SubscribeAsync(Key.K, KeyState.Up, evt =>
{
    Console.WriteLine($"Keyboard event! {evt.Key} {evt.KeyState}");
});

subscription.Dispose(); // Unsubscribe.
```

See [InputHook](docs/InputHook.md) for the full API, or [ExampleApp1](examples/ExampleApp1/) for more examples.
