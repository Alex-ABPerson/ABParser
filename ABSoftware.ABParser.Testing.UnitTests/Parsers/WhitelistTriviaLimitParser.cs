using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Text;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class WhitelistTriviaLimitParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken("A"),
            new ABParserToken("B"),
            new ABParserToken("C"),
        }, 1).AddTriviaLimit(true, "JustABCs", 'a', 'b', 'c');

        public WhitelistTriviaLimitParser() : base(ParserConfig) { }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            base.BeforeTokenProcessed(args);

            switch (args.CurrentToken.Token.Name)
            {
                case "A":
                    EnterTriviaLimit("JustABCs");
                    break;
                case "C":
                    ExitTriviaLimit();
                    break;
            }
        }
    }
}
