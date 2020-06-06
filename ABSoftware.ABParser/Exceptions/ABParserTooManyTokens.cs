using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Exceptions
{
    public class ABParserTooManyTokens : Exception
    {
        public ABParserTooManyTokens() : base("There are too many tokens, ABParser can only have up to 65,535 tokens, consider making two parsers instead.") { }
    }
}
