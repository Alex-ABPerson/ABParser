using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class TriviaLimitParser : TrackingParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("A")),
            new ABParserToken(new ABParserText("B")),
            new ABParserToken(new ABParserText("C")),
        }, 2).AddTriviaLimit("NoWhiteSpace", ' ', '\r', '\n', '\t').AddTriviaLimit("NoABCs", 'a', 'b', 'c');

        public TriviaLimitParser() : base(ParserConfig) { }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            base.BeforeTokenProcessed(args);

            switch (args.Token.TokenName.AsString())
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
