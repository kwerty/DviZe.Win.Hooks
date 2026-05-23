using Kwerty.DviZe.Workers;
using System.Threading.Tasks;

namespace Kwerty.DviZe.Win.Hooks;

internal sealed class InputHookSession(InputHookOptions options, IThreadAccessor threadAccessor) : Worker
{
    static int nextId;
    internal int id = nextId++;

    protected override async Task OnStartingAsync(WorkerStartingContext startingContext)
    {
        if (!options.InstallOnDemand)
        {
            await threadAccessor.UIThread;

            if (options.Hooks.HasFlag(InputHookTypes.Keyboard))
            {
                InputHookNativeExtensions.UseKeyboardHook();
            }

            if (options.Hooks.HasFlag(InputHookTypes.Mouse))
            {
                InputHookNativeExtensions.UseMouseHook();
            }
        }
    }

    protected override async Task OnStoppingAsync()
    {
        if (!options.InstallOnDemand)
        {
            await threadAccessor.UIThread;

            if (options.Hooks.HasFlag(InputHookTypes.Keyboard))
            {
                InputHookNativeExtensions.ReleaseKeyboardHook();
            }

            if (options.Hooks.HasFlag(InputHookTypes.Mouse))
            {
                InputHookNativeExtensions.ReleaseMouseHook();
            }
        }
    }
}
