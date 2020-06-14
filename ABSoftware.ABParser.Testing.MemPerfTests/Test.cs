using ABSoftware.ABParser.Exceptions;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.MemPerfTests
{
    public abstract class Test
    {
        public static bool IsMemory = false;

        protected abstract int NumberOfIterations { get; }
        protected abstract string TestName { get; }
        protected abstract void Prepare();
        protected abstract void DoIteration(int i);
        protected abstract void Finish();

        public void Run()
        {
            Prepare();

            if (IsMemory) 
                for (int i = 0; i < NumberOfIterations; i++)
                    DoIteration(i);
            else
            {
                long totalTicks = 0;

                DoIteration(0);

                Stopwatch timer = new Stopwatch();
                for (int i = 0; i < NumberOfIterations; i++)
                {
                    timer.Restart();
                    DoIteration(i);
                    timer.Stop();
                    totalTicks += timer.ElapsedTicks;
                }

                long finalTicks = totalTicks / NumberOfIterations;

                Console.WriteLine("[" + TestName + "] TICKS: " + finalTicks + " MILLISECONDS: " + finalTicks / 10000);
            }

            Finish();
        }
    }
}
