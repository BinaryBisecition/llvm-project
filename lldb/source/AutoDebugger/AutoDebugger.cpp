#include "lldb/API/SBAddress.h"
#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBBreakpointLocation.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBEvent.h"
#include "lldb/API/SBFileSpec.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBLineEntry.h"
#include "lldb/API/SBListener.h"
#include "lldb/API/SBModule.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBValue.h"
#include "lldb/API/SBValueList.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace lldb;
using namespace std;

#define LOGS true

static vector<string> splitCmdLine(string &cmdline) {
  // Split by spaces.
  vector<string> tokens;
  istringstream iss(cmdline);
  string token;
  while (iss >> token)
    tokens.push_back(token);
  return tokens;
}

class LLSession {
public:
  SBDebugger debugger;
  SBTarget target;
  SBProcess process;
  SBListener listener;
  SBBreakpoint breakpoint;
  string cmdline;
  string bp_file;
  vector<string> argv;
  int32_t bp_line = -1;

  LLSession() = default;

  bool isValidSession() const { return debugger.IsValid() && target.IsValid(); }

  bool isValidTarget() { return target.IsValid(); }

  void setCmdLine(string &cmdline) {
    this->cmdline = cmdline;
    argv = splitCmdLine(cmdline);
  }

  void createSessDebugger() {
    debugger = SBDebugger::Create(false);
    this->debugger.SetAsync(false);
    assert(debugger.IsValid() and "Invalid debugger.");
  }

  void createSessTarget(string &bin_path) {
    lldb::SBError err;
    target =
        debugger.CreateTarget(bin_path.c_str(), nullptr, nullptr, true, err);
    assert(isValidTarget() and "Invalid target.");
  }

  string getBPFile() { return bp_file; }

  void setBPFile(string &file) { this->bp_file = file; }

  void setBPLine(uint32_t line) { this->bp_line = line; }

  void createBreakPointByLocation() {
    assert(bp_file.size() and bp_line != -1 and "Breakpoint spec is corrupt.");
    breakpoint = target.BreakpointCreateByLocation(bp_file.c_str(), bp_line);

    if (!breakpoint.IsValid() or breakpoint.GetNumLocations() == 0) {
      cerr << "Warning: breakpoint created but has 0 locations for " << bp_file
           << ":" << bp_line << "\n";
    }
  }

  void setListener() {
    listener =
        SBListener(("listener." + bp_file + ":" + to_string(bp_line)).c_str());
  }

  bool verifySession() {
    return (isValidSession() and listener.IsValid() and bp_file.size() and
            bp_line != -1);
  }
};

static bool parseBpSpec(string &bp_spec, LLSession &sess) {

  auto pos = bp_spec.find_last_of(':');
  if (pos == std::string::npos)
    return false;
  auto file = bp_spec.substr(0, pos);
  sess.setBPFile(file);
  try {
    sess.setBPLine((uint32_t)(stol(bp_spec.substr(pos + 1))));
  } catch (...) {
    return false;
  }
  return true;
}

static bool prepareSession(LLSession &sess, string &bin_path, string &cmdline,
                           string &bp_spec) {
  sess.setCmdLine(cmdline);
  if (sess.argv.empty() or cmdline.empty()) {
    cerr << "Empty command line.\n";
    return false;
  }

  sess.createSessDebugger();

  sess.createSessTarget(bin_path);

  if (!sess.isValidTarget()) {
    cerr << "Failed to create target for : " << bin_path << "\n";
    return false;
  }

  if (!parseBpSpec(bp_spec, sess)) {
    cerr << "Invalid breakpoint spec : " << bp_spec << "\n";
    return false;
  }

  assert(sess.isValidSession() and "Session is not valid.\n");

  sess.createBreakPointByLocation();

  sess.setListener();

  return true;
}

static bool launchSession(LLSession &sess) {
  SBLaunchInfo launchInfo(nullptr);

  vector<const char *> cargv;
  for (int i = 1; i < sess.argv.size(); i++)
    cargv.push_back(sess.argv[i].c_str());
  cargv.push_back(nullptr);

  launchInfo.SetLaunchFlags(eLaunchFlagStopAtEntry | eLaunchFlagDisableASLR |
                            eLaunchFlagDebug);
  launchInfo.SetArguments(cargv.data(), /* append */ false);

  SBError error;
  sess.process = sess.target.Launch(launchInfo, error);

  if (!sess.process.IsValid() || error.Fail())
    return false;

  return true;
}

// Check if the process has stopped at designated bp.
bool hasStoppedAtBreakpoint(LLSession &sess, SBThread &ExThread) {
  if (!sess.process.IsValid())
    return false;
  StateType state = sess.process.GetState();
  if (state != eStateStopped)
    return false;
  for (uint32_t ti = 0; ti < sess.process.GetNumThreads(); ti++) {
    SBThread thread = sess.process.GetThreadAtIndex(ti);
    if (thread.GetStopReason() == eStopReasonBreakpoint) {
      // Also check the top frame line equals the request line.
      SBFrame topFrame = thread.GetFrameAtIndex(0);
      SBLineEntry lineEntry = topFrame.GetLineEntry();
      if (lineEntry.IsValid()) {
        if (lineEntry.GetFileSpec().GetFilename() == sess.bp_file or
            lineEntry.GetLine() == sess.bp_line) {
          ExThread = thread;
          return true;
        }
      }
    } else {
      ExThread = thread;
      return true;
    }
  }
  return false;
}
struct FrameSnapShot {
  string funcn;
  string file;
  int line;
  vector<string> variables;
};

static bool isApplicationFileCode(const SBFileSpec &fs) {
  if (!fs.IsValid())
    return false;
  string fnm = fs.GetFilename();
  return fnm == "Queens.cpp";
}

static string indent(int d) { return string(d * 2, ' '); }

string stringyFySBValue(SBValue val, uint32_t depth = 0,
                        uint32_t max_depth = 6) {
  if (!val.IsValid())
    return "<invalid>";

  std::ostringstream out;
  const char *name = val.GetName();
  const char *type = val.GetTypeName();
  const char *value = val.GetValue();
  const char *summary = val.GetSummary();

  out << indent(depth);

  if (name && *name)
    out << name;
  else
    out << "<anon>";

  out << " : ";
  out << (type ? type : "<type?>");

  if (summary && *summary)
    out << summary;
  else if (value && *value)
    out << value;

  if (depth >= max_depth)
    return out.str();

  size_t nchilds = val.GetNumChildren();
  if (nchilds > 0) {
    out << " {\n";
    for (size_t i = 0; i < nchilds; i++) {
      SBValue child = val.GetChildAtIndex((uint32_t)i);
      out << stringyFySBValue(child, depth + 1, max_depth);
    }
    out << indent(depth) << "}";
  }

  return out.str();
}

static FrameSnapShot captureFrameSnapshot(SBFrame frame) {
  FrameSnapShot snap;

  const char *fn = frame.GetFunctionName();
  snap.funcn = (fn ? fn : "<unknown>");

  SBLineEntry le = frame.GetLineEntry();
  if (le.IsValid()) {
    snap.file = le.GetFileSpec().GetFilename();
    snap.line = le.GetLine();
  } else {
    snap.file = "<unknown>";
    snap.line = -1;
  }
  SBValueList vars =
      frame.GetVariables(true /* args */, true /* locals */,
                         false /* statics */, true /* in scope only*/);

  for (int i = 0; i < vars.GetSize(); i++) {
    SBValue v = vars.GetValueAtIndex(i);
    // SBDeclaration decl = v.GetDeclaration();
    // if (!isApplicationFileCode(decl.GetFileSpec()))
    //   continue;
    snap.variables.push_back(stringyFySBValue(v));
  }
  return snap;
}

static void dumpSnapShot(const FrameSnapShot &snap) {
  cerr << "Function : " << snap.funcn << "\n";
  cerr << "Location : " << snap.file << ":" << snap.line << "\n";
  for (auto &v : snap.variables)
    cerr << v << "\n";
}

struct StackSig {
  vector<string> funcs;
};

static StackSig captureStackSig(SBThread &th) {
  StackSig sigState;

  for (int i = 0; i < th.GetNumFrames(); i++) {
    SBFrame f = th.GetFrameAtIndex(i);
    const char *fnm = f.GetFunctionName();
    sigState.funcs.push_back((fnm ? fnm : "<unknown>"));
  }

  return sigState;
}

static int depthDiff(const StackSig &prev, const StackSig &curr) {
  return (int)curr.funcs.size() - (int)prev.funcs.size();
}

static bool runSynchronizedLoop(LLSession &base, LLSession &exp) {
  if (!base.process.IsValid())
    return false;

  SBThread thrd = base.process.GetThreadAtIndex(0);
  if (!thrd.IsValid())
    return false;

  StackSig prev_stack = captureStackSig(thrd);
  SBLineEntry prev_line_entry =
      thrd.GetFrameAtIndex(thrd.GetNumFrames() - 1).GetLineEntry();
  cerr << "[BASE] Root entry\n";
  dumpSnapShot(
      captureFrameSnapshot(thrd.GetFrameAtIndex(thrd.GetNumFrames() - 1)));

  while (true) {
    base.process.Continue();

    StateType state;
    do {
      state = base.process.GetState();
    } while (state == eStateRunning);

    if (state == eStateExited) {
      cerr << "[BASE ] process exited\n";
      return true;
    } else if (state == eStateCrashed) {
      cerr << "[BASE ] process crashed\n";
      return false;
    } else if (state != eStateStopped)
      continue;

    thrd = base.process.GetThreadAtIndex(0);
    if (!thrd.IsValid())
      continue;

    StackSig currStack = captureStackSig(thrd);
    int diff = depthDiff(prev_stack, currStack);

    SBFrame currFrame = thrd.GetFrameAtIndex(thrd.GetNumFrames() - 1);
    SBLineEntry curr_le = currFrame.GetLineEntry();

    // Call detected
    if (diff == 1) {
      const string &callee = currStack.funcs.back();
      cerr << "[BASE] Call -> " << callee << "\n";

      SBFrame callee_frame = thrd.GetFrameAtIndex(thrd.GetNumFrames() - 1);
      dumpSnapShot(captureFrameSnapshot(callee_frame));
    } else if (diff < 0) {
      int returns = -diff;
      for (int i = 0; i < returns; i++) {
        const string &retfn = prev_stack.funcs[prev_stack.funcs.size() - 1 - i];
        cerr << "[BASE] Returns <- " << retfn << "\n";
      }
    } else {
      bool progressed = false;
      if (curr_le.IsValid() and prev_line_entry.IsValid())
        progressed = curr_le.GetLine() != prev_line_entry.GetLine();
      if (progressed) {
        cerr << "[BASE] Progress in " << currStack.funcs.back() << " at line "
             << curr_le.GetLine() << "\n";
      }
    }

    prev_stack = currStack;
    prev_line_entry = curr_le;
  }
  return true;
}

static bool driver(char **argv) {

  string cmd_base = argv[1];
  string cmd_exp = argv[2];
  string bp_base = argv[3];
  string bp_exp = argv[4];

  auto split_base = splitCmdLine(cmd_base);
  auto split_exp = splitCmdLine(cmd_exp);

  string bin_base = split_base[0];
  string bin_exp = split_exp[0];

  assert(bin_base.size() and bin_exp.size() and
         "Invalid command line split.\n");

  SBDebugger::Initialize();

  LLSession sess_base, sess_exp;

  if (!prepareSession(sess_base, bin_base, cmd_base, bp_base)) {
    cerr << "Failed to prepare base session!\n";
    SBDebugger::Terminate();
    return 1;
  }

  if (!prepareSession(sess_exp, bin_exp, cmd_exp, bp_exp)) {
    cerr << "Failed to prepare experimental session!\n";
    SBDebugger::Terminate();
    return 1;
  }

  if (!launchSession(sess_base)) {
    cerr << "Failed to launch baseline session!\n";
    SBDebugger::Terminate();
    return 1;
  }

  if (!launchSession(sess_exp)) {
    cerr << "Failed to launch exp session!\n";
    SBDebugger::Terminate();
    return 1;
  }

#if LOGS
  cout << "> Both Sessions established, Entering Syncronized loop.\n";
#endif

  if (!runSynchronizedLoop(sess_base, sess_exp)) {
    cerr << "Process Crashed unexpectedly.\n";
    SBDebugger::Terminate();
    return 1;
  }

  SBDebugger::Terminate();
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage : " << argv[0]
              << " <cmd_base> <cmd_exp> <bp_base> <bp_exp>\n";
    return 1;
  }

  return driver(argv);
}
