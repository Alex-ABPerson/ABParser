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
            new ABParserToken("theya"),
            new ABParserToken("heya"),
            new ABParserToken("thata"),
            new ABParserToken("hat"),
        });

        public TheyMiddleParser() : base(ParserConfig) { }
    }
}
