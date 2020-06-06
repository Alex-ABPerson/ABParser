using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    public class QuoteLimitParser : TrackingParser
    {
        bool InString = false;

        static readonly ABParserConfiguration ParserConfig = ABParserConfiguration.Create(new ABParserToken[] {
            new ABParserToken(new ABParserText("DOUBLE_QUOTE"), new ABParserText("\"")).AddToLimit("DoubleStringLimit"),
            new ABParserToken(new ABParserText("SINGLE_QUOTE"), new ABParserText("'")).AddToLimit("SingleStringLimit"),
            new ABParserToken(new ABParserText("CAPITAL_B"), new ABParserText("B")),
        });

        public QuoteLimitParser() : base(ParserConfig) { }

        protected override void OnStart()
        {
            InString = false;
            base.OnStart();
        }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            base.BeforeTokenProcessed(args);
            if (InString) ExitTokenLimit();
            else
                switch (args.Token.TokenName.AsString())
                {
                    case "SINGLE_QUOTE":
                        EnterTokenLimit("SingleStringLimit");
                        break;
                    case "DOUBLE_QUOTE":
                        EnterTokenLimit("DoubleStringLimit");
                        break;
                }
        }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            base.OnTokenProcessed(args);

            var tokenName = args.Token.TokenName.AsString();
            if (tokenName == "SINGLE_QUOTE" || tokenName == "DOUBLE_QUOTE")
                    InString = !InString;
                
        }
    }
}
