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

        internal static int WideCharacterSize = GetWideCharacterSize();

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern int GetWideCharacterSize();

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern IntPtr CreateBaseParser(string[] tokenData, int[] tokenLengths, int tokenDataLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void SetText(IntPtr parser, string text, int textLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern ContinueExecutionResult ContinueExecution(IntPtr parser, byte[] outData);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void DeleteBaseParser(IntPtr baseParser);
    }
}
