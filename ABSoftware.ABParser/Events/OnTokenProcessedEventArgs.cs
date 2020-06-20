using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Events
{
    public class OnTokenProcessedEventArgs : TokenProcessedEventArgs
    {
        public TokenInformation NextToken;

        internal string TrailingAsString;
        public char[] Trailing { get; internal set; }

#pragma warning disable IDE0074 // Use compound assignment - Doesn't exist in .NET Standard 1.1!
        public string GetTrailingAsString() => TrailingAsString ?? (TrailingAsString = new string(Trailing));
#pragma warning restore IDE0074 // Use compound assignment

        internal OnTokenProcessedEventArgs(ABParser parser) : base(parser) { }
    }
}
