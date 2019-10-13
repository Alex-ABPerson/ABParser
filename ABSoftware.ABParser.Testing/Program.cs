using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing
{
    public class Program
    {
        [DllImport("ABParserCore", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        static extern void SayHello(StringBuilder output);

        public static void Main()
        {
            var parser = new ABParser(new ABParserToken[] {
                new ABParserToken(new ABParserText("NumberOne"), new ABParserText("1")),
                new ABParserToken(new ABParserText("Foo"), new ABParserText("Foo"))
            });
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
