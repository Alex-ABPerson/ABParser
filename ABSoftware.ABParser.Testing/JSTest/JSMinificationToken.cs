using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.JSTest
{
    public class JSMinificationToken
    {
        /// <summary>
        /// The name that is used to identify this token.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// The actual character the token is made of.
        /// </summary>
        public char[] Token { get; set; }

        /// <summary>
        /// The way this token is written when minified.
        /// </summary>
        public char[] Writing { get; set; }

        public JSMinificationToken(string name, char[] tokenAndWriting)
        {
            Name = name;
            Token = tokenAndWriting;
            Writing = tokenAndWriting;
        }

        public JSMinificationToken(string name, char[] token, char[] writing)
        {
            Name = name;
            Token = token;
            Writing = writing;
        }
    }
}
