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
            new ABParserToken(new ABParserText("the")),
            new ABParserToken(new ABParserText("they")),
            new ABParserToken(new ABParserText("theyare"))
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
            using (var parser = new TheyParser())
            {
                parser.SetText(new ABParserText("AtheBtheyCtheyarDtheyareE"));
                parser.Start();
            }

            // We're now done with it.
            Console.ReadLine();
        }
    }
}
