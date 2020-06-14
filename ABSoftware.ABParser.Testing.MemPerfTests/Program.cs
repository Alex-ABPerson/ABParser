using ABSoftware.ABParser.Testing.MemPerfTests.Tests;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.MemPerfTests
{
    public class Program
    {
        public static void Main(string[] args)
        {
            PerformanceTest();
            Console.ReadLine();
        }

        public static async void MemoryTest()
        {
            Test.IsMemory = true;

            // ABPARSER MEMORY TEST - TO DETECT MEMORY LEAKS. (Designed for the Visual Studio Diagnostic Tools)
            // 1. Choose which tests you want to run - to look at all of the tests available, simply look inside the "Tests" directory in this project.
            var tests = new Test[]
            {
                new MultipleReuseTest(),
                new MultipleStringSetsTest()
            };

            // 2. Start debugging, and the program will break on the line below, then, go into the diagnostic tools, and the "Memory Usage" tab, and choose "Take Snapshot" - ensure "Heap Profiling" is turned on.
            // 3. Then, click on "Continue".
            Debugger.Break();

            for (int i = 0; i < tests.Length; i++)
                tests[i].Run();

            await Task.Delay(5000); // Wait 5 seconds to ensure that all of the disposal threads have gone through. TODO: Remove this?

            // 4. The program will break on this line after a while. Then, click on "Take Snapshot" AGAIN.
            // 5. Now, if you look at the second snapshot you can see the size changes - but, we're mainly interesting in looking at the native objects left. So, on the "Native Allocations" columns, click on the number (not the one with the arrow after).
            Debugger.Break();

            // 6. You can now see all of the objects that were left lying around! The ONLY objects that SHOULD be there are the tokens and one uint16_t array for each of those tokens.
            // 7. If you do see others objects in there, try firstly increasing the 5 seconds up there to something higher, and if that still doesn't fix it - then there's a memory leak in ABParser!
            // 8. Once you've fixed the memory leak, reset the number up there to 5 seconds, as that's how it should be kept in the master repo.
        }

        public static void PerformanceTest()
        {
            Test.IsMemory = false;

            var tests = new Test[]
            {
                new MultipleReuseTest(),
                new MultipleStringSetsTest()
            };

            for (int i = 0; i < tests.Length; i++)
                tests[i].Run();
        }
    }
}
