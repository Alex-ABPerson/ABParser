using ABSoftware.ABParser.Exceptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser
{
    public sealed class ABParserToken
    {
        public string[] TokenLimits = null;
        public char[] DetectionLimits = null;

        /// <summary>
        /// The name this token can be given to identify it.
        /// </summary>
        public string Name;

        /// <summary>
        /// The data this token is made up of.
        /// </summary>
        public string Data;

        /// <summary>
        /// If you want to attach extra data alongside the token, you can use this to do that.
        /// </summary>
        public object Tag;

        public ABParserToken(string name, string tokenData, object tag = null)
        {
            if (name.Length > 255) throw new ABParserNameTooLong();
            Name = name;
            Data = tokenData;
            Tag = tag;
        }

        public ABParserToken(string nameAndData, object tag = null)
        {
            if (nameAndData.Length > 255) throw new ABParserNameTooLong();
            Name = nameAndData;
            Data = nameAndData;
            Tag = tag;
        }

        public ABParserToken SetLimits(params string[] limits)
        {
            TokenLimits = limits;
            return this;
        }

        public ABParserToken SetDetectionLimits(params char[] detectionLimits)
        {
            DetectionLimits = detectionLimits;
            return this;
        }
    }
}
