using System;
using System.Collections.Generic;
using System.Text;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class ImpossibleVerifyParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[] {
            new ABParserToken("they"),
            new ABParserToken("heya")
        });

        public ImpossibleVerifyParser() : base(ParserConfig) { }
    }
}
