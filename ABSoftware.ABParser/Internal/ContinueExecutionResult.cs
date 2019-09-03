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
        Stop = 1,
        BeforeTokenProcessed = 2,
        OnTokenProcessed = 3
    }
}
