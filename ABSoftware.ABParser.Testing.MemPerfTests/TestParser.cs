using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ABSoftware.ABParser.Events;

namespace ABSoftware.ABParser.Testing.MemPerfTests
{
    public class TestParser : ABParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken("the", "the"),
            new ABParserToken("they", "they"),
            new ABParserToken("theyare", "theyare")
        });

        public TestParser() : base(ParserConfig) { }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            base.BeforeTokenProcessed(args);
        }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            base.OnTokenProcessed(args);
        }
    }
}
