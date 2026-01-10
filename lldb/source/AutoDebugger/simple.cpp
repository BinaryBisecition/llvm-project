#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBBreakpointLocation.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBEvent.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBLineEntry.h"
#include "lldb/API/SBListener.h"
#include "lldb/API/SBModule.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBValue.h"
#include "lldb/API/SBValueList.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace lldb;
using namespace std;

static vector<string> splitCmdLine(const string &cmdline) {
  vector<string> tokens;
  string current;
  bool in_quotes = false;

  for (char c : cmdline) {
    if (c == '"') {
      in_quotes = !in_quotes;
    } else if (isspace(c) && !in_quotes) {
      if (!current.empty()) {
        tokens.push_back(current);
        current.clear();
      }
    } else {
      current.push_back(c);
    }
  }
  if (!current.empty())
    tokens.push_back(current);

  return tokens;
}

static void dumpFrame(SBFrame frame) {
  cerr << "Function: " << frame.GetFunctionName() << "\n";
  SBLineEntry le = frame.GetLineEntry();
  cerr << "Location: " << le.GetFileSpec().GetFilename() << ":" << le.GetLine()
       << "\n";

  SBValueList vars = frame.GetVariables(true, true, true, true);
  for (size_t i = 0; i < vars.GetSize(); i++) {
    SBValue v = vars.GetValueAtIndex(i);
    cerr << "  " << v.GetName() << " = "
         << (v.GetValue() ? v.GetValue() : "<noval>") << "\n";
  }
}

int main(int argc, char **argv) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " \"<cmdline>\" \"<file:line>\"\n";
    return 1;
  }

  string cmdline = argv[1];
  string bpspec = argv[2];

  // Parse bp spec
  auto pos = bpspec.find_last_of(':');
  assert(pos != string::npos);

  string file = bpspec.substr(0, pos);
  int line = stoi(bpspec.substr(pos + 1));

  // Parse cmdline
  vector<string> parts = splitCmdLine(cmdline);
  assert(!parts.empty());

  string binary = parts[0];
  vector<const char *> args;
  for (size_t i = 1; i < parts.size(); i++)
    args.push_back(parts[i].c_str());
  args.push_back(nullptr);

  // Initialize LLDB
  SBDebugger::Initialize();
  SBDebugger dbg = SBDebugger::Create(false);
  dbg.SetAsync(false);

  // Create target
  SBError error;
  SBTarget target =
      dbg.CreateTarget(binary.c_str(), nullptr, nullptr, true, error);
  if (!target.IsValid()) {
    cerr << "Target invalid: " << error.GetCString() << "\n";
    return 1;
  }

  // Create breakpoint
  SBBreakpoint bp = target.BreakpointCreateByLocation(file.c_str(), line);
  cerr << "BP locations = " << bp.GetNumLocations() << "\n";

  // Launch
  SBLaunchInfo info(nullptr);
  info.SetArguments(args.data(), false);
  info.SetLaunchFlags(eLaunchFlagStopAtEntry | eLaunchFlagDisableASLR |
                      eLaunchFlagDebug);

  SBProcess process = target.Launch(info, error);
  if (!process.IsValid() || error.Fail()) {
    cerr << "Launch failed: " << error.GetCString() << "\n";
    return 1;
  }

  // Continue until we stop on BP
  while (true) {
    StateType state = process.GetState();
    if (state == eStateStopped) {
      // inspect breakpoint
      for (uint32_t i = 0; i < process.GetNumThreads(); i++) {
        SBThread th = process.GetThreadAtIndex(i);
        if (th.GetStopReason() == eStopReasonBreakpoint) {
          cerr << "\n*** BREAKPOINT HIT ***\n";
          dumpFrame(th.GetFrameAtIndex(0));
          SBDebugger::Terminate();
          return 0;
        }
      }
    }
    if (state == eStateCrashed) {
      cerr << "Process crashed unexpectedly.\n";
      break;
    }
    if (state == eStateExited) {
      break;
    }
    process.Continue();
  }

  SBDebugger::Terminate();
  return 0;
}