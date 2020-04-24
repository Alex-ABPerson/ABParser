using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class ABParserNoTokensParser : TrackingParser
    {
        static readonly ABParserTokensArray ParserTokens = ABParserTokensArray.GenerateTokensArray(new ABParserToken[] { });

        public ABParserNoTokensParser() : base(ParserTokens) { }
    }
}
