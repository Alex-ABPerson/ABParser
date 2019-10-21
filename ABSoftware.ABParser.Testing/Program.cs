using System;
using ABSoftware.ABParser.Events;

namespace ABSoftware.ABParser.Testing
{
    public class TestParser : ABParser
    {
        static readonly ABParserTokensArray TestParserTokens = ABParserTokensArray.GenerateTokensArray(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("NumberOne"), new ABParserText("1")),
            new ABParserToken(new ABParserText("Foo"), new ABParserText("Foo"))
        });

        public TestParser() : base(TestParserTokens) { }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            Console.WriteLine("C# BEFORETOKENPROCESSED HIT");
        }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            Console.WriteLine("C# ONTOKENPROCESSED HIT");
        }
    }

    public class Program
    {

        public static void Main()
        {
            var parser = new TestParser();
            parser.Start("Hi1&Foo");

            //var result = new StringBuilder(6);
            //SayHello(result);

            //Console.WriteLine(result);
            Console.WriteLine("Done!");
            Console.ReadLine();

            // We're now done with it.
            parser.Dispose();
            Console.ReadLine();
        }
    }
}
