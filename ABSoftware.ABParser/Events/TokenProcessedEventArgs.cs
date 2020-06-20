using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Events
{
    public class TokenProcessedEventArgs
    {
        protected ABParser _parser;

        internal string LeadingAsString;
        public char[] Leading { get; internal set; }
#pragma warning disable IDE0074 // Use compound assignment - Doesn't exist in .NET Standard 1.1!
        public string GetLeadingAsString() => LeadingAsString ?? (LeadingAsString = new string(Leading));
#pragma warning restore IDE0074 // Use compound assignment

        public TokenInformation PreviousToken;
        public TokenInformation CurrentToken;

        internal TokenProcessedEventArgs(ABParser parser) => _parser = parser;
    }
}
