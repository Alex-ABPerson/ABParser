using System;
using System.Diagnostics;
using ABSoftware.ABParser.Events;
using ABSoftware.ABParser.Testing.UnitTests.Parsers;

namespace ABSoftware.ABParser.Testing
{

    public class TestParser : ABParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("DOUBLE_QUOTE"), new ABParserText("\"")).SetLimits("DoubleStringLimit"),
            new ABParserToken(new ABParserText("SINGLE_QUOTE"), new ABParserText("'")).SetLimits("SingleStringLimit"),
            new ABParserToken(new ABParserText("CAPITAL_B"), new ABParserText("B")),
        });

        public TestParser() : base(ParserConfig) { }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            Console.WriteLine("OnTokenProcessed: " + args.Token.TokenName.AsString());
            Console.WriteLine("OnTokenProcessed Leading: " + args.Leading.AsString());
            Console.WriteLine("OnTokenProcessed Trailing: " + args.Trailing.AsString());
        }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            Console.WriteLine("BeforeTokenProcessed: " + args.Token.TokenName.AsString());
            Console.WriteLine("BeforeTokenProcessed Leading: " + args.Leading.AsString());
        }
    }

    public class Program
    {
        public static void Main()
        {
            using (var parser = new TestParser())
            {
                parser.SetText(new ABParserText("A\"aBdc\"B"));
                parser.Start();
            }

            // We're now done with it.
            Console.ReadLine();
        }
    }
}
