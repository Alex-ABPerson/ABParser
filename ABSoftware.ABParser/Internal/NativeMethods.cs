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
        internal static unsafe extern IntPtr InitializeConfiguration(string[] tokensData, ushort* tokenLengths, ushort numberOfTokens, string[] limitNames, byte* limitNameSizes, ushort* limitsPerToken);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern IntPtr CreateBaseParser(IntPtr tokenData);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static unsafe extern ContinueExecutionResult ContinueExecution(IntPtr parser, ushort* outData);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static unsafe extern void ConfigSetTriviaLimits(IntPtr config, string[] limitNames, byte* limitNameSizes, string[] limitContents, ushort* limitContentLengths, int numberOfLimits);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void InitString(IntPtr parser, string text, int textLength);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void DisposeDataForNextParse(IntPtr parser);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void EnterTokenLimit(IntPtr baseParser, string limitName, byte limitNameSize);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void ExitTokenLimit(IntPtr baseParser, int levels);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void EnterTriviaLimit(IntPtr baseParser, string limitName, byte limitNameSize);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void ExitTriviaLimit(IntPtr baseParser, int levels);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void DeleteBaseParser(IntPtr baseParser);

        [DllImport(COREDLL, CharSet = CHARSET, CallingConvention = CALLING_CONVENTION)]
        internal static extern void DeleteConfiguration(IntPtr configuration);
    }
}