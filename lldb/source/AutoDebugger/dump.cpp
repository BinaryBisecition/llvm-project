
// Helper to check if thread stopped on a breakpoint and return top frame
static bool threadStoppedOnBreakpoint(SBProcess &process,
                                      SBThread &out_thread) {
  for (uint32_t ti = 0; ti < process.GetNumThreads(); ++ti) {
    SBThread th = process.GetThreadAtIndex(ti);
    if (th.GetStopReason() == eStopReasonBreakpoint) {
      out_thread = th;
      return true;
    }
  }
  return false;
}

void runSynchronizedLoop(LLSession &base, LLSession &exp) {
  const int MAX_WAIT_MS = 15000; // 15s watchdog
  bool base_snapshot = false, exp_snapshot = false;
  std::string base_snap, exp_snap;
  SBThread th_base, th_exp;

  // BEFORE resuming, ensure listeners are attached for both processes
  if (base.process.IsValid())
    base.process.GetBroadcaster().AddListener(
        base.listener, SBProcess::eBroadcastBitStateChanged);
  if (exp.process.IsValid())
    exp.process.GetBroadcaster().AddListener(
        exp.listener, SBProcess::eBroadcastBitStateChanged);

  // Continue both after attaching listeners (they were stopped at entry)
  if (base.process.IsValid())
    base.process.Continue();
  if (exp.process.IsValid())
    exp.process.Continue();

  while (true) {
    // Check exit conditions
    StateType sb =
        base.process.IsValid() ? base.process.GetState() : eStateInvalid;
    StateType se =
        exp.process.IsValid() ? exp.process.GetState() : eStateInvalid;
    if ((sb == eStateExited || sb == eStateDetached || sb == eStateInvalid) &&
        (se == eStateExited || se == eStateDetached || se == eStateInvalid)) {
      std::cerr << "> both processes finished\n";
      break;
    }

    // Wait for an event on either listener with a reasonable timeout
    SBEvent event;
    bool got_base = base.listener.WaitForEvent(5000, event);
    if (got_base) {
      StateType s = SBProcess::GetStateFromEvent(event);
      std::cerr << "[event] base process state from event = " << s << "\n";
      // inspect threads & stop reasons
      for (uint32_t t = 0; t < base.process.GetNumThreads(); ++t) {
        SBThread th = base.process.GetThreadAtIndex(t);
        std::cerr << " base thread[" << t
                  << "] stopReason=" << th.GetStopReason() << "\n";
      }
      if (threadStoppedOnBreakpoint(base.process, th_base)) {
        base_snapshot = true;
        base_snap = captureFrameSnapshot(th_base.GetFrameAtIndex(0));
        std::cerr << "[BASE] breakpoint hit and snapshot captured\n";
      }
    }

    bool got_exp = exp.listener.WaitForEvent(5000, event);
    if (got_exp) {
      StateType s = SBProcess::GetStateFromEvent(event);
      std::cerr << "[event] exp process state from event = " << s << "\n";
      for (uint32_t t = 0; t < exp.process.GetNumThreads(); ++t) {
        SBThread th = exp.process.GetThreadAtIndex(t);
        std::cerr << " exp thread[" << t
                  << "] stopReason=" << th.GetStopReason() << "\n";
      }
      if (threadStoppedOnBreakpoint(exp.process, th_exp)) {
        exp_snapshot = true;
        exp_snap = captureFrameSnapshot(th_exp.GetFrameAtIndex(0));
        std::cerr << "[EXP] breakpoint hit and snapshot captured\n";
      }
    }

    // If we have both snapshots, print and continue both
    if (base_snapshot && exp_snapshot) {
      std::cout << "\n===== SYNCED SNAPSHOTS =====\n";
      std::cout << "BASE (" << base.cmdline << ")\n" << base_snap << "\n";
      std::cout << "EXP  (" << exp.cmdline << ")\n" << exp_snap << "\n";
      std::cout << "============================\n\n";

      base_snapshot = exp_snapshot = false;
      base_snap.clear();
      exp_snap.clear();

      if (base.process.IsValid())
        base.process.Continue();
      if (exp.process.IsValid())
        exp.process.Continue();
      continue;
    }

    // If one side arrived but the other didn't within a bigger timeout, log and
    // proceed
    if (base_snapshot && !exp_snapshot) {
      static auto t0 = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::steady_clock::now() - t0)
                         .count();
      if (elapsed > 15) {
        std::cerr << "Timeout waiting for EXP snapshot; base has it. Dumping "
                     "states...\n";
        // dump per-process info
        std::cerr << "BASE state=" << base.process.GetState() << "\n";
        std::cerr << "EXP  state=" << exp.process.GetState() << "\n";
        // decide policy: continue both to avoid deadlock
        if (base.process.IsValid())
          base.process.Continue();
        if (exp.process.IsValid())
          exp.process.Continue();
        base_snapshot = exp_snapshot = false;
      }
    }
    if (exp_snapshot && !base_snapshot) {
      static auto t1 = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::steady_clock::now() - t1)
                         .count();
      if (elapsed > 15) {
        std::cerr << "Timeout waiting for BASE snapshot; continuing both\n";
        if (base.process.IsValid())
          base.process.Continue();
        if (exp.process.IsValid())
          exp.process.Continue();
        base_snapshot = exp_snapshot = false;
      }
    }
  } // end while
}
