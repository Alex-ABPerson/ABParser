using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Internal
{
    internal static class NativeMethods
    {
        internal const string COREDLL = "ABParserCore";
        internal const CharSet CHARSET = CharSet.Unicode;
        internal const CallingConvention CALLING_CONVENTION = CallingConvention.Cdecl;

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern IntPtr CreateBaseParser(IntPtr singleCharTokens, IntPtr multiCharTokens, int singleCharTokensLength, int multiCharTokensLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void InitializeTokens(string[] tokenData, int[] tokenLengths, int tokenDataLength, ref IntPtr outSingleCharTokens, ref IntPtr outMultiCharTokens, ref int outSingleCharTokensLength, ref int outMultiCharTokensLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern ContinueExecutionResult ContinueExecution(IntPtr parser, ushort[] outData);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void InitString(IntPtr parser, string text, int textLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void DisposeDataForNextParse(IntPtr parser);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void DeleteBaseParser(IntPtr baseParser);
    }
}
