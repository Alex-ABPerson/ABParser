using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class TheyMiddleParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("theya")),
            new ABParserToken(new ABParserText("heya")),
            new ABParserToken(new ABParserText("thata")),
            new ABParserToken(new ABParserText("hat")),
        });

        public TheyMiddleParser() : base(ParserConfig) { }
    }
}
