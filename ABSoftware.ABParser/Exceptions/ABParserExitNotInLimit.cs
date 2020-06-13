using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Exceptions
{
    public class ABParserExitNotInLimit : Exception
    {
        public ABParserExitNotInLimit() : base("Cannot exit limit when not within limit.") { }
    }
}
