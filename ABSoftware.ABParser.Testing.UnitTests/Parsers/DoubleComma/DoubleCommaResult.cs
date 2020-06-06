using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers.DoubleComma
{
    public class DoubleCommaResult
    {
        public List<DoubleCommaArray> Arrays = new List<DoubleCommaArray>();
    }

    public class DoubleCommaArray
    {
        public List<DoubleCommaItem> Items = new List<DoubleCommaItem>();
    }

    public abstract class DoubleCommaItem { }
    public class DoubleCommaString : DoubleCommaItem
    {
        public string Text;

        public DoubleCommaString(string text) => Text = text;
    }

    public class DoubleCommaInt : DoubleCommaItem
    {
        public int Num;

        public DoubleCommaInt(int num) => Num = num;
    }
}
