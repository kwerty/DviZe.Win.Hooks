namespace Kwerty.DviZe.Win.Hooks;

public sealed class InputHookOptions
{
    /// <summary>
    /// Specifies which hooks to install.
    /// </summary>
    public InputHookTypes Hooks { get; init; } = InputHookTypes.Keyboard | InputHookTypes.Mouse;

    /// <summary>
    /// Set to <c>false</c> to manage installation manually via <c>InputHook.InstallAsync</c>.
    /// </summary>
    public bool InstallOnDemand { get; init; } = true;

    public static InputHookOptions Default { get; } = new();
}
