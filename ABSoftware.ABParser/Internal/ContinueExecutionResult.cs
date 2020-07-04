using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Internal
{
    public enum ContinueExecutionResult
    {
        None = 0,
        StopAndFinalOnTokenProcessed = 1,
        FirstBeforeTokenProcessed = 2,
        OnThenBeforeTokenProcessed = 3,
        OnFirstUnlimitedCharacterProcessed = 4
    }
}
