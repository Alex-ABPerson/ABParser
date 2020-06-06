using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class NoTokensParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = ABParserConfiguration.Create(new ABParserToken[0]);

        public NoTokensParser() : base(ParserConfig) { }
    }
}
