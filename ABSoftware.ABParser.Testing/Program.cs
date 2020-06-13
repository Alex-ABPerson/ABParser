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
            new ABParserToken(new ABParserText("A")),
            new ABParserToken(new ABParserText("B")),
            new ABParserToken(new ABParserText("C")),
        }, 2).AddTriviaLimit("NoWhiteSpace", ' ', '\r', '\n', '\t').AddTriviaLimit("NoABCs", 'a', 'b', 'c');

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

    public class Program
    {
        public static void Main()
        {
            using (var parser = new TestParser())
            {
                parser.SetText(new ABParserText("h Aj \tkBadcCl oCl o"));
                parser.Start();
            }

            // We're now done with it.
            Console.ReadLine();
        }
    }
}
