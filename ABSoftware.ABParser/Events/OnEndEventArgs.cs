using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;

namespace ABSoftware.ABParser.Events
{
    public class OnEndEventArgs
    {
        internal string LeadingAsString;

        public char[] Leading;
        public string GetLeadingAsString() => LeadingAsString ?? (LeadingAsString = new string(Leading));
    }
}
