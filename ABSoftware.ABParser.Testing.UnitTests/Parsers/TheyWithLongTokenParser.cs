using System;
using System.Collections.Generic;
using System.Text;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class TheyWithLongTokenParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken("the"),
            new ABParserToken("they"),
            new ABParserToken("theyare"),
            new ABParserToken("AtheBtheyCtheyarDtheyareE"),
        });

        public TheyWithLongTokenParser() : base(ParserConfig) { }
    }
}
