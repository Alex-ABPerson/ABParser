using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class BlacklistTriviaLimitParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken("A"),
            new ABParserToken("B"),
            new ABParserToken("C"),
        }, 2).AddTriviaLimit(false, "NoWhiteSpace", ' ', '\r', '\n', '\t').AddTriviaLimit(false, "NoABCs", 'a', 'b', 'c');

        public BlacklistTriviaLimitParser() : base(ParserConfig) { }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            base.BeforeTokenProcessed(args);

            switch (args.CurrentToken.Token.Name)
            {
                case "A":
                    EnterTriviaLimit("NoWhiteSpace");
                    break;
                case "B":
                    EnterTriviaLimit("NoABCs");
                    break;
                case "C":
                    ExitTriviaLimit();
                    break;
            }
        }
    }
}
