using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Exceptions
{
    public class ABParserTextNotInitialized : Exception
    {
        public ABParserTextNotInitialized() : base("ABParser was used with uninitialized text, please remember to call 'SetText' before running an ABParser.") { }
    }
}
