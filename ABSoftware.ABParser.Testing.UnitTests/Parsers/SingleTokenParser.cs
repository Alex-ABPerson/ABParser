using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class SingleTokenParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[] {
            new ABParserToken("A"),
            new ABParserToken("BCD")
        });
        
        public SingleTokenParser() : base(ParserConfig) { }

    }
}
