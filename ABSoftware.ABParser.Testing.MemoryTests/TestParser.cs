using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ABSoftware.ABParser.Events;

namespace ABSoftware.ABParser.Testing.MemoryTests
{
    public class TestParser : ABParser
    {
        static readonly ABParserTokensContainer ParserTokens = ABParserTokensContainer.GenerateTokensArray(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("the"), new ABParserText("the")),
            new ABParserToken(new ABParserText("they"), new ABParserText("they")),
            new ABParserToken(new ABParserText("theyare"), new ABParserText("theyare"))
        });

        public TestParser() : base(ParserTokens) { }

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
