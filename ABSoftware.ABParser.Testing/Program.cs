using System;
using ABSoftware.ABParser.Events;
using ABSoftware.ABParser.Testing.JSTest;

namespace ABSoftware.ABParser.Testing
{

    public class TestParser : ABParser
    {
        static readonly ABParserTokensArray TestParserTokens = ABParserTokensArray.GenerateTokensArray(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("the"), new ABParserText("the")),
            new ABParserToken(new ABParserText("they"), new ABParserText("they"))
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
            var parser = new JSMinificationParser();
            //parser.Start("Atheyarh{215}");
            parser.Start("ahah   function hi(a,b,c)");

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
