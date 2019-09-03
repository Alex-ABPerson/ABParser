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
        internal static extern IntPtr CreateBaseParser(StringBuilder[] tokenData, int tokenDataLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void SetText(IntPtr parser, string output, int textLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void SetText(IntPtr parser, char[] output, int textLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void SetText(IntPtr parser, StringBuilder output, int textLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern ContinueExecutionResult ContinueExecution(int[] data);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void DeleteBaseParser(IntPtr baseParser);
    }
}
