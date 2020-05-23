using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class ABParserSingleTokenParser : TrackingParser
    {
        static readonly ABParserTokensContainer ParserTokens = ABParserTokensContainer.GenerateTokensArray(new ABParserToken[] {
            new ABParserToken(new ABParserText("A")),
            new ABParserToken(new ABParserText("BCD"))
        });
        
        public ABParserSingleTokenParser() : base(ParserTokens) { }

    }
}
