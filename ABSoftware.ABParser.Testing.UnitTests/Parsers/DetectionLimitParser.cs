using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class DetectionLimitParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("ABC")).SetDetectionLimits('d', 'j'),
            new ABParserToken(new ABParserText("DEF")).SetDetectionLimits(' ', '\t', '\r', '\n'),
        });

        public DetectionLimitParser() : base(ParserConfig) { }
    }
}
