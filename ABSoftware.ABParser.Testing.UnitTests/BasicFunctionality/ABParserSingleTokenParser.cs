using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.BasicFunctionality
{
    public class ABParserSingleTokenParser : TrackingParser
    {
        static readonly ABParserTokensArray ParserTokens = ABParserTokensArray.GenerateTokensArray(new ABParserToken[] {
            new ABParserToken(new ABParserText("A")),
            new ABParserToken(new ABParserText("BCD"))
        });
        
        public ABParserSingleTokenParser() : base(ParserTokens) { }

    }
}
