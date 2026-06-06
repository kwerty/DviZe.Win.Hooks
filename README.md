# DviZe.Win.Hooks

Low-latency keyboard and mouse hooks for .NET console and non-UI applications.

⚡ Keyboard and mouse events are processed natively, only crossing the native-to-managed boundary for matching subscriptions.

🙏 On-demand hook installation. System hooks are installed individually as needed.

🧩 Plug in your own native input handler to process keyboard events, mouse events, or both.

Targets .NET 10. Written in C# and C++/CLI.

```csharp
// Subscribe to the letter K.
var subscription = await inputHook.SubscribeAsync(Key.K, KeyState.Up, evt =>
{
    Console.WriteLine($"Keyboard event! {evt.Key} {evt.KeyState}");
});

subscription.Dispose(); // Unsubscribe.

// Subscribe to right button clicks.
using var _ = await inputHook.SubscribeAsync(MouseAction.RightButtonUp, evt =>
{
    Console.WriteLine($"Mouse event! {evt.Action}");
});

// Register your own native input handler.
using var _ = await inputHook.RegisterHandlerAsync(new CustomInputHandler());
```

See [InputHook](docs/InputHook.md) for the full API, or [ExampleApp1](examples/ExampleApp1/) for more examples.
