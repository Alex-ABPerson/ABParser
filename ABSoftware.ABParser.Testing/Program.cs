using System;
using System.Diagnostics;
using ABSoftware.ABParser.Events;
using ABSoftware.ABParser.Testing.JSTest;

namespace ABSoftware.ABParser.Testing
{

    public class TestParser : ABParser
    {
        static readonly ABParserTokensArray ParserTokens = ABParserTokensArray.GenerateTokensArray(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("the"), new ABParserText("the")),
            new ABParserToken(new ABParserText("they"), new ABParserText("they")),
            new ABParserToken(new ABParserText("theyare"), new ABParserText("theyare"))
        });

        public TestParser() : base(ParserTokens) { }
    }

    public class Program
    {

        public static void Main()
        {
            for (int i = 0; i < 1000; i++)
                using (var parser = new JSMinificationParser())
                {
                    //var stopwatch = Stopwatch.StartNew();
                    //parser.Start(new ABParserText("AtheBtheyCtheyarDtheyareE"));
                    parser.Start(new ABParserText("ahah   function hi(a,b,c)"));
                    //stopwatch.Stop();
                    //Console.WriteLine(stopwatch.ElapsedMilliseconds / 100);
                }

            //var result = new StringBuilder(6);
            //SayHello(result);

            //Console.WriteLine(result);
            Console.WriteLine("Done!");
            Console.ReadLine();

            // We're now done with it.
            Console.ReadLine();
        }
    }
}
