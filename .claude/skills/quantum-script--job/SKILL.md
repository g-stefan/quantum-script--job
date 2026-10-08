---
name: quantum-script--job
description: >-
  How to use the Quantum Script Job extension (quantum-script--job), the
  parallel job runner loaded with Script.requireExtension("Job"): new Job(),
  addProcess(cmd, info, synchronizedKey) for programs started with
  Shell.executeNoWait, addThread(fn, fnThis, parameters, info,
  synchronizedKey) for script functions run in a Thread, processMaxCount
  (parallel limit, default Processor.getCount()), clockTick (ms per pass,
  default 1000), the time limit processMaxTicks with setProcessMaxTime /
  Seconds / Minutes (stored in ticks: set clockTick first), the callbacks
  onStart(process) / onEnd(process, result) / onTimedout(process) with this
  = the Job, the job record fields, synchronizedKey groups that never
  overlap, and process(), the blocking loop; the rules (no shell, no exit
  code and no output capture for programs, thread functions compiled again
  with empty globals and copied parameters / result, threads only asked to
  stop on time out, programs killed but not their children, exceptions
  from callbacks leave process(), jobs may be added from onEnd). Use when
  writing or reviewing Quantum Script or fabricare .js code that uses new
  Job(), addProcess, addThread or job.process(), runs build steps or
  commands in parallel, C++ code that includes
  <XYO/QuantumScript.Extension/Job.hpp>, a fabricare.json depending on
  "quantum-script--job", or when working inside the quantum-script--job
  repository.
---

# quantum-script--job

This is the parallel job runner of Quantum Script. The rules of the
`quantum-script` skill (the language and its differences from JavaScript)
apply here too. Purpose: **run a list of jobs (programs or script functions
in threads) a few at a time**, with a parallel limit, a time limit per job,
groups that must not overlap, and callbacks for start / end / time out.
Use it for build steps, batch conversions, test runs. `process()` blocks
until every job is done.

Full documentation lives in `docs/` of the quantum-script--job repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--job\docs` on this machine):

- README: purpose, concepts, source map
- getting-started: build, first script, fabricare scripts, C++ host, static
- **job-model**: passes, timing, the limit, time limits, groups, order,
  callbacks, errors, stopping, programs vs threads, limitations
- **script-api**: every property and function, the job record
- **recipes**: commands in parallel, exit codes via threads, files in
  parallel, groups, follow-up steps, thread stop on time out, progress
  (all tested)
- reference

Read the matching page when you need more than this summary. The whole
implementation is `source/XYO/QuantumScript.Extension/Job/Library.js`
(~200 lines of Quantum Script); read it when in doubt.

## Script API

```javascript
Script.requireExtension("Job");      // also loads Thread, Shell, Math; fabricare scripts have it already

var job = new Job();                 // `new` REQUIRED (Job() throws)
job.processMaxCount = 4;             // parallel limit, default Processor.getCount()
job.clockTick = 100;                 // ms per pass, default 1000 -- set BEFORE the time limit
job.setProcessMaxTimeMinutes(10);    // also setProcessMaxTime(ms), setProcessMaxTimeSeconds(s); default 60 ticks

job.onStart = function(process) {};          // before start; may change process.cmd / .parameters
job.onEnd = function(process, result) {};    // result = thread return value (copy); undefined for programs
job.onTimedout = function(process) {};       // program killed / thread asked to stop; onEnd NOT called

job.addProcess("tool --option file", info, synchronizedKey);          // info, key optional
job.addThread(function(a, b) {                                         // runs in a new Thread
	Script.requireExtension("Shell");          // empty globals: require what you use
	return a + b;                              // goes to onEnd
}, fnThis, [a, b], info, synchronizedKey);

job.process();                       // blocks until every job ended or timed out
```

Record fields: `info`, `cmd` | `fn` / `fnThis` / `parameters`, `isThread`,
`done`, `processRunning`, `processTicks`, `processTimedout`, `processId`
(program, `0` if it could not start), `thread` (thread),
`synchronizedJob`, `synchronizedKey`, `processTime` (unused, always `0`).

## Hard rules

1. **Nothing runs before `process()`**, and it returns only when every job
   is done. It blocks the calling thread.
2. **A pass = sleep `clockTick`, check running jobs, start new ones.** The
   first job starts one tick after `process()`; each end is noticed up to
   one tick late; a run ends about two ticks after the last job. For short
   jobs use `clockTick = 50 .. 100`.
3. **The time limit is stored in ticks** (`Math.floor(ms / clockTick)`,
   min 1, computed at the call). Set `clockTick` first, then
   `setProcessMaxTime*`. `job.clockTick = 100;` alone turns the default
   1 minute limit into 6 seconds.
4. **Programs (`addProcess`) run without a shell** (`Shell.executeNoWait`):
   no pipes, redirection, `&&`, or shell built-ins; they share the console;
   **no exit code, no captured output**. A program that cannot start ends
   normally (`onEnd`, `processId` `0`). On time out it is killed
   immediately, but **its children are not**.
5. **To get exit codes**, use `addThread` with `return Shell.system(cmd);`
   (shell syntax works). Trade-off: on time out the command is not killed.
6. **Thread functions are compiled again** in a new thread with empty
   globals: no access to enclosing variables (they read as `undefined`),
   call `Script.requireExtension` inside for every extension (even
   `Thread`). `fnThis`, `parameters` and the returned value are copied;
   functions inside them become `undefined`. Share live data with `Atomic`.
7. **Threads are only asked to stop on time out** (`requestToTerminate`).
   Long loops must test `CurrentThread.isRequestToTerminate()`.
8. **Equal `synchronizedKey` (`==`) jobs never overlap**, and start in list
   order. Other jobs run next to them. A waiting group member may take a
   parallel slot for one pass.
9. **Callbacks run one at a time in the `process()` thread** with `this` =
   the `Job`, in list order within a pass (not real end order). `onEnd` and
   `onTimedout` are exclusive.
10. **Callbacks may add jobs** (`this.addThread(...)` in `onEnd`); they run
    in the same `process()`. Use this for follow-up steps / pipelines.
11. **Exceptions from callbacks leave `process()`**; running jobs keep
    running; calling `process()` again continues. A thread that throws just
    ends with `result` `undefined`.
12. A `Job` can be reused: `process()` again runs only records not `done`.
    Records are never removed.

## Patterns

```javascript
// exit codes, failures collected
job.onEnd = function(process, code) { if (code != 0) { failed[failed.length] = process.info; }; };
job.onTimedout = function(process) { failed[failed.length] = process.info + " (timed out)"; };
job.addThread(function(cmd) { Script.requireExtension("Shell"); return Shell.system(cmd); }, null, [cmd], cmd);

// steps sharing an output folder never overlap
job.addProcess(cmdA, "a", "out-folder"); job.addProcess(cmdB, "b", "out-folder");

// follow-up step
job.onEnd = function(process, r) { if (process.info == "build") { this.addThread(test, null, [r], "test"); }; };

// thread that honors the time limit
job.addThread(function() { Script.requireExtension("Thread");
	while (!CurrentThread.isRequestToTerminate()) { /* a slice of work */ }; return "stopped"; }, null, [], "loop");

// progress: one function for both
job.onEnd = function(process) { Console.writeLn("[" + (++done) + "/" + total + "] " + process.info); };
job.onTimedout = job.onEnd;
```

`docs/recipes.md` has complete, tested versions of these. For timers and
an event loop in one thread, use `quantum-script--task`; for a single
background thread, `quantum-script--thread`; for running one command and
waiting, `Shell.system` / `Shell.execute`.

## C++

```cpp
#include <XYO/QuantumScript.Extension/Shell.hpp>
#include <XYO/QuantumScript.Extension/Thread.hpp>
#include <XYO/QuantumScript.Extension/Math.hpp>
#include <XYO/QuantumScript.Extension/Job.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Shell::registerInternalExtension(executive);    // Job's Library.js requires
	Extension::Thread::registerInternalExtension(executive);   // Thread, Shell and Math
	Extension::Math::registerInternalExtension(executive);
	Extension::Job::registerInternalExtension(executive);
};
```

- There is no C++ API beyond `registerInternalExtension` /
  `initExecutive` and the metadata (`Copyright`, `License`, `Version`).
  `Job` is script code compiled by `initExecutive` with
  `executive->compileStringX(librarySource)`.
- Static build: `quantum-script--job.static` defines
  `XYO_QUANTUMSCRIPT_EXTENSION_JOB_LIBRARY` (empty export macro, no
  `quantumScriptExtension` entry point). Register it as internal, with
  `Thread`, `Shell` and `Math`.
- fabricare and `quantum-script--magnet` register `Job` internally.

## Working in this repository

- Build with `fabricare make`, `fabricare test` and `fabricare install` (see
  the `fabricare` skill). Install `quantum-script` and the `console`,
  `buffer`, `thread`, `shell`, `shellfind`, `math` (and `application` for
  the test) extensions first. `test.01` is a C++ host run in `output/test`
  that executes `../../test/test.01.js`, which starts
  `quantum-script ../../test/test.01.sub.*.js` processes and thread jobs,
  with and without groups.
- `Job` is written in script in `Library.js`. `fabricare/make.prepare.js`
  (`file-to-cs`) turns it into `Library.Source.cpp` (`librarySource`),
  compiled in `initExecutive`. Edit `Library.js`, not the generated file.
- Quick check of a change without a C++ build: run a script with the
  installed `quantum-script` after `fabricare install`.
- When you change behavior, update `README.md`, `docs/script-api.md`,
  `docs/job-model.md`, `docs/reference.md` and this skill.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX headers: MIT for `source/` and `docs/`,
  Unlicense for `test/`, `fabricare/` and `.claude/` (see `.reuse/dep5`).
