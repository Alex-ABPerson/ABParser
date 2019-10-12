using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser
{
    /// <summary>
    /// Represents a block of text in ABParser.
    /// This technically stores data in either a string, character array or StringBuilder, but puts it all together so that it doesn't require extra conversions.
    /// </summary>
    public class ABParserText
    {
        enum OriginalDataType
        {
            StringBuilder,
            CharArray,
            String
        }

        readonly OriginalDataType originalType;
        StringBuilder _sb;
        char[] _charArray;
        string _string = null;

        public override string ToString()
        {
            if (_string == null)
                if (originalType == OriginalDataType.StringBuilder)
                    return _string = _sb.ToString();
                else
                    return _string = new string(_charArray);
            else return _string;
        }

        public string AsString() => ToString();

        public StringBuilder AsStringBuilder()
        {
            if (_sb == null)
                if (originalType == OriginalDataType.String)
                    return _sb = new StringBuilder(_string);
                else
                    return _sb = new StringBuilder().Append(_charArray);
            else return _sb;
        }

        public char[] AsCharArray()
        {
            if (_charArray == null)
                if (originalType == OriginalDataType.String)
                    return _charArray = _string.ToCharArray();
                else
                {
                    _charArray = new char[_sb.Length];
                    _sb.CopyTo(0, _charArray, 0, _sb.Length);
                    return _charArray;
                }
            else return _charArray;
        }

        public int GetLength()
        {
            switch (originalType)
            {
                case OriginalDataType.CharArray:
                    return _charArray.Length;
                case OriginalDataType.StringBuilder:
                    return _sb.Length;
                case OriginalDataType.String:
                    return _string.Length;
                default:
                    return 0;
            }
        }

        public ABParserText(string str)
        {
            originalType = OriginalDataType.String;
            _string = str;
        }

        public ABParserText(StringBuilder sb)
        {
            originalType = OriginalDataType.StringBuilder;
            _sb = sb;
        }

        public ABParserText(char[] chArr)
        {
            originalType = OriginalDataType.CharArray;
            _charArray = chArr;
        }
    }
}
