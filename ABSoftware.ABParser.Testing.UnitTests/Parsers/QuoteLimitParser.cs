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

        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[] {
            new ABParserToken("DOUBLE_QUOTE", "\"").SetLimits("DoubleStringLimit"),
            new ABParserToken("SINGLE_QUOTE", "'").SetLimits("SingleStringLimit"),
            new ABParserToken("CAPITAL_B", "B"),
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
                switch (args.CurrentToken.Token.Name)
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

            var Name = args.CurrentToken.Token.Name;
            if (Name == "SINGLE_QUOTE" || Name == "DOUBLE_QUOTE")
                    InString = !InString;
        }
    }
}
