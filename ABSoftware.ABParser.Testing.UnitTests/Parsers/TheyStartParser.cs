using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    /// <summary>
    /// Has "the", "they" and "theyare" tokens - a good test for verification.
    /// </summary>
    public class TheyParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = ABParserConfiguration.Create(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("the")),
            new ABParserToken(new ABParserText("they")),
            new ABParserToken(new ABParserText("theyare")),
        });

        public TheyParser() : base(ParserConfig) { }
    }
}
