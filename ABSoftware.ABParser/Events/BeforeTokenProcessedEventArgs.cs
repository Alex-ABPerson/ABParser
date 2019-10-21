using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Events
{
    public class BeforeTokenProcessedEventArgs : TokenProcessedEventArgs
    {
        internal BeforeTokenProcessedEventArgs(ABParser parser) : base(parser) { }
    }
}
