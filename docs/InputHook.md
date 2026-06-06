# InputHook

**Namespace:** `Kwerty.DviZe.Win.Hooks`

The entry point for hooking keyboard and mouse events.

## Constructor

```csharp
public InputHook(IThreadAccessor threadAccessor, ILoggerFactory loggerFactory);
public InputHook(InputHookOptions options, IThreadAccessor threadAccessor, ILoggerFactory loggerFactory);
```

An `IThreadAccessor` from a running `MessagePump`\* is required.

### InputHookOptions

| Property          | Type                  | Default                                               | Description
| :--               | :--                   | :--                                                   | :--
| `Hooks`           | `InputHookTypes`      | `InputHookTypes.Keyboard` \| `InputHookTypes.Mouse`   | Specifies which hooks to install.
| `InstallOnDemand` | `bool`                | `true`                                                | Set to `false` to manage installation manually via `InstallAsync`.

\* Defined in `Kwerty.DviZe.Win` ([DviZe.Win.Common](https://github.com/kwerty/DviZe.Win.Common)).

## InstallAsync

```csharp
public Task InstallAsync(CancellationToken cancellationToken = default);
```

Installs the keyboard and/or mouse hook.

Only valid when `InputHookOptions.InstallOnDemand` is `false`.

## RegisterHandlerAsync

```csharp
public Task<IDisposable> RegisterHandlerAsync(InputHandler handler, CancellationToken cancellationToken = default);
public Task<IDisposable> RegisterHandlerAsync(InputHandler handler, int priority, CancellationToken cancellationToken = default);
```

Registers a native handler for processing keyboard and/or mouse events.

The built-in handlers are `DefaultKeyboardHandler` and `DefaultMouseHandler`. Custom handlers can be created by deriving from `InputHandler`.

Handlers with lower `priority` values execute first. Defaults to `0`.

Dispose the returned `IDisposable` to unregister the handler.

## SubscribeAsync

```csharp
public Task<IDisposable> SubscribeAsync(Key key, Action<KeyboardEvent> callback, CancellationToken cancellationToken = default);
public Task<IDisposable> SubscribeAsync(Key key, KeyState keyState, Action<KeyboardEvent> callback, CancellationToken cancellationToken = default);
public Task<IDisposable> SubscribeAsync(MouseAction mouseAction, Action<MouseEvent> callback, CancellationToken cancellationToken = default);
```

Convenience wrappers for `DefaultKeyboardHandler` and `DefaultMouseHandler`.

Dispose the returned `IDisposable` to unsubscribe.

### KeyboardEvent

| Property      | Type          | Description
| :--           | :--           | :--
| `Key`         | `Key`\*       | The key which triggered the event.
| `KeyState`    | `KeyState`    | `KeyState.Down` or `KeyState.Up`.
| `Cancel`      | `bool`        | Set to `true` to suppress the event.

\* Defined in `Kwerty.DviZe.Win` ([DviZe.Common](https://github.com/kwerty/DviZe.Common)).

### MouseEvent

| Property      | Type              | Description
| :--           | :--               | :--
| `Action`      | `MouseAction`     | The action which triggered the event.
| `Cancel`      | `bool`            | Set to `true` to suppress the event.

## DisposeAsync

```csharp
public ValueTask DisposeAsync();
```

Uninstalls the keyboard and/or mouse hook.
