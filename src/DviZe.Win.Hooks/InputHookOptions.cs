using Kwerty.DviZe.Workers;
using System;

namespace Kwerty.DviZe.Win.Hooks;

public sealed class InputHookOptions
{
    /// <summary>
    /// Specifies which hooks to install.
    /// </summary>
    public required InputHookTypes Hooks { get; init; }

    /// <summary>
    /// Set to <c>false</c> to manage installation manually via <c>InputHook.InstallAsync</c>.
    /// </summary>
    public bool InstallOnDemand { get; init; } = true;

    /// <summary>
    /// Controls the session lifecycle when <see cref="InstallOnDemand" /> is <c>true</c>.
    /// </summary>
    public OnDemandOptions OnDemandOptions
    {
        get;
        init => field = value ?? throw new ArgumentNullException(nameof(value));
    } = OnDemandOptions.Default;

    public static InputHookOptions Default { get; } = new InputHookOptions
    {
        Hooks = InputHookTypes.Keyboard | InputHookTypes.Mouse,
        InstallOnDemand = true,
    }; 
}
