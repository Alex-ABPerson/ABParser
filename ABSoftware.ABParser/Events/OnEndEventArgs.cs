using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;

namespace ABSoftware.ABParser.Events
{
    public class OnEndEventArgs
    {
        private string _leadingAsString;

        public char[] Leading;
        public string GetLeadingAsString() => _leadingAsString ?? (_leadingAsString = new string(Leading));
    }
}
