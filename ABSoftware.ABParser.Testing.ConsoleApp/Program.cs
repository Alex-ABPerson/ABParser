using ABSoftware.ABParser.Testing.UnitTests.Parsers;
using System;
using System.Collections.Generic;
using System.Linq;

namespace ABSoftware.ABParser.Testing.ConsoleApp
{
    class Program
    {
        static void Main()
        {
            using (var parser = new SingleTokenParser())
            {
                parser.SetText("aAb");
                parser.Start();
            }

            Console.WriteLine("FINISHED!");
            Console.ReadLine();
        }
    }
}
