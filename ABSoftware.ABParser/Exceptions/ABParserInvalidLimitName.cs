using System;
using System.Collections.Generic;
using System.Text;

namespace ABSoftware.ABParser.Exceptions
{
    public class ABParserInvalidLimitName : Exception
    {
        public ABParserInvalidLimitName() : base("There are no limits with the name given! If it's a trivia limit, make sure the number of trivia limits is set correctly is set in the configuration!") { }
    }
}
