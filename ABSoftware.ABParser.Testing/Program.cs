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
            var parser = new ABParser(new ABParserToken[0]);
            parser.Start(new char[] { 'H', 'i' });

            //var result = new StringBuilder(6);
            //SayHello(result);

            //Console.WriteLine(result);
            Console.ReadLine();

            // We're now done with it.
            parser.Dispose();
        }
    }
}
