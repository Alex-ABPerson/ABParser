using ABSoftware.ABParser;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.JSTest
{
    /// <summary>
    /// Provides JS tokens to everything else.
    /// </summary>
    public static class JSMinificationTokenProvider
    {
        public static JSMinificationToken[] Tokens = new JSMinificationToken[]
        {
            // ===============
            // KEYWORDS
            // ===============
            new JSMinificationToken("break", "break".ToCharArray()),
            new JSMinificationToken("case", "case ".ToCharArray()),
            new JSMinificationToken("catch", "catch".ToCharArray()),
            new JSMinificationToken("const", "const ".ToCharArray()),
            new JSMinificationToken("continue", "continue".ToCharArray()),
            new JSMinificationToken("delete", "delete ".ToCharArray()),
            new JSMinificationToken("do", "do".ToCharArray()),
            new JSMinificationToken("else", "else".ToCharArray()),
            new JSMinificationToken("debugger", "debugger".ToCharArray()),
            new JSMinificationToken("enum", "enum ".ToCharArray()),
            new JSMinificationToken("default", "default ".ToCharArray()),
            new JSMinificationToken("false", "false".ToCharArray()),
            new JSMinificationToken("finally", "finally".ToCharArray()),
            new JSMinificationToken("for", "for".ToCharArray()),
            new JSMinificationToken("function", "function".ToCharArray()),
            new JSMinificationToken("if", "if".ToCharArray()),
            new JSMinificationToken("in", " in ".ToCharArray()),
            new JSMinificationToken("instanceof", "instanceof ".ToCharArray()),
            new JSMinificationToken("new", "new ".ToCharArray()),
            new JSMinificationToken("null", "null".ToCharArray()),
            new JSMinificationToken("return", "return".ToCharArray()), // Can have a space after it if it is returning something.
            new JSMinificationToken("switch", "switch".ToCharArray()),
            new JSMinificationToken("this", "this".ToCharArray()),
            new JSMinificationToken("throw", "throw ".ToCharArray()),
            new JSMinificationToken("true", "true".ToCharArray()),
            new JSMinificationToken("try", "try".ToCharArray()),
            new JSMinificationToken("typeof", "typeof".ToCharArray()),
            new JSMinificationToken("var", "var ".ToCharArray()),
            new JSMinificationToken("void", "void ".ToCharArray()),
            new JSMinificationToken("while", "while".ToCharArray()),
            new JSMinificationToken("with", "with ".ToCharArray()),

            // ===============
            // OPERATORS
            // ===============
            new JSMinificationToken("[op]", ">>>=".ToCharArray()),
            new JSMinificationToken("[op]", ">>=".ToCharArray()),
            new JSMinificationToken("[op]", "|=".ToCharArray()),
            new JSMinificationToken("[op]", "<<=".ToCharArray()),
            new JSMinificationToken("[op]", "^=".ToCharArray()),
            new JSMinificationToken("[op]", "&=".ToCharArray()),
            new JSMinificationToken("[op]", "+=".ToCharArray()),
            new JSMinificationToken("[op]", "-=".ToCharArray()),
            new JSMinificationToken("[op]", "*=".ToCharArray()),
            new JSMinificationToken("[op]", "/=".ToCharArray()),
            new JSMinificationToken("[op]", "%=".ToCharArray()),
            new JSMinificationToken("[op]", ";".ToCharArray()),
            new JSMinificationToken("[op]", ",".ToCharArray()),
            new JSMinificationToken("[op]", "?".ToCharArray()),
            new JSMinificationToken("[op]", ":".ToCharArray()),
            new JSMinificationToken("[op]", "||".ToCharArray()),
            new JSMinificationToken("[op]", "&&".ToCharArray()),
            new JSMinificationToken("[op]", "|".ToCharArray()),
            new JSMinificationToken("[op]", "&".ToCharArray()),
            new JSMinificationToken("[op]", "^".ToCharArray()),
            new JSMinificationToken("[op]", "===".ToCharArray()),
            new JSMinificationToken("[op]", "==".ToCharArray()),
            new JSMinificationToken("[op]", "=".ToCharArray()),
            new JSMinificationToken("[op]", "!==".ToCharArray()),
            new JSMinificationToken("[op]", "!=".ToCharArray()),
            new JSMinificationToken("[op]", "<<".ToCharArray()),
            new JSMinificationToken("[op]", "<=".ToCharArray()),
            new JSMinificationToken("[op]", "<".ToCharArray()),
            new JSMinificationToken("[op]", ">>>".ToCharArray()),
            new JSMinificationToken("[op]", ">>".ToCharArray()),
            new JSMinificationToken("[op]", ">=".ToCharArray()),
            new JSMinificationToken("[op]", ">".ToCharArray()),
            new JSMinificationToken("[op]", "++".ToCharArray()),
            new JSMinificationToken("[op]", "--".ToCharArray()),
            new JSMinificationToken("[op]", "+".ToCharArray()),
            new JSMinificationToken("[op]", "-".ToCharArray()),
            new JSMinificationToken("[op]", "*".ToCharArray()),
            new JSMinificationToken("[op]", "/".ToCharArray()),
            new JSMinificationToken("[op]", "%".ToCharArray()),
            new JSMinificationToken("[op]", "!".ToCharArray()),
            new JSMinificationToken("[op]", "~".ToCharArray()),
            new JSMinificationToken("[op]", ".".ToCharArray()),
            new JSMinificationToken("[op]Comma", ",".ToCharArray()),
            new JSMinificationToken("[op]OpenSquare", "[".ToCharArray()),
            new JSMinificationToken("[op]CloseSquare", "]".ToCharArray()),
            new JSMinificationToken("[op]OpenCurly", "{".ToCharArray()),
            new JSMinificationToken("[op]CloseCurly", "}".ToCharArray()),
            new JSMinificationToken("[op]OpenBracket", "(".ToCharArray()),
            new JSMinificationToken("[op]CloseBracket", ")".ToCharArray()),
            
            // ===============
            // OTHER
            // ===============
            new JSMinificationToken("Comment", "//".ToCharArray()),
            new JSMinificationToken("CommentStart", "/*".ToCharArray()),
            new JSMinificationToken("CommentEnd", "*/".ToCharArray())
        };

        public static ABParserToken[] ABParserConfig
        {
            get
            {
                var newArr = new ABParserToken[Tokens.Length];

                // Go through and add all the tokens to the new array.
                for (int i = 0; i < Tokens.Length; i++)
                    newArr[i] = new ABParserToken(new ABParserText(i.ToString()), new ABParserText(Tokens[i].Token));

                return newArr;
            }
        }
     }
}
