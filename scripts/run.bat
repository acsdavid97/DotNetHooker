@echo off

set COR_ENABLE_PROFILING=1
set COR_PROFILER={bd9083fc-ae69-4925-aab2-0ee90281e6de}
:: be loaded into .NET 4 as well.
set COMPlus_ProfAPI_ProfilerCompatibilitySetting=EnableV2Profiler

:: ';' separated list of function names for which arguments should be dumped to files.
set DNH_ARGUMENT_FILTER=Assembly.Load

:: launch executable given at the commandline
%1
