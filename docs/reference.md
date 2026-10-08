# API reference

## Script

These symbols are available after `Script.requireExtension("Job")`. The
extension also requires `Thread`, `Shell` and `Math`, so their symbols are
available too.

### Globals

| Symbol | Kind | Notes |
|--------|------|-------|
| `Job()` | constructor | `new Job()`: an empty, independent job runner; without `new` it throws |

### Job properties

| Property | Default | Notes |
|----------|---------|-------|
| `processMaxCount` | `Processor.getCount()` | most jobs running together |
| `clockTick` | `1000` | milliseconds per pass |
| `processMaxTicks` | `60` | time limit in passes; set with `setProcessMaxTime*` after `clockTick` |
| `processJobList` | `[]` | the job records |
| `onStart(process)` | empty | before a job starts; `this` is the `Job` |
| `onEnd(process, result)` | empty | after a job ended; `result` is the thread's returned value, `undefined` for a program |
| `onTimedout(process)` | empty | after a job was stopped by the time limit; `onEnd` is not called |

### Job methods

| Method | Returns | Behavior |
|--------|---------|----------|
| `job.setProcessMaxTime(milliSeconds)` | `undefined` | `processMaxTicks = max(1, floor(milliSeconds / clockTick))` |
| `job.setProcessMaxTimeSeconds(seconds)` | `undefined` | `setProcessMaxTime(seconds * 1000)` |
| `job.setProcessMaxTimeMinutes(minutes)` | `undefined` | `setProcessMaxTimeSeconds(minutes * 60)` |
| `job.addProcess(cmd, info, synchronizedKey)` | `undefined` | appends a program job, started with `Shell.executeNoWait(cmd)` |
| `job.addThread(fn, fnThis, parameters, info, synchronizedKey)` | `undefined` | appends a thread job, started with `Thread.newThread(fn, fnThis, parameters)` |
| `job.process()` | `undefined` | runs passes until every job is done; exceptions from callbacks pass through |

### Job record fields

`info`, `cmd` (program), `fn` / `fnThis` / `parameters` (thread),
`isThread`, `synchronizedJob`, `synchronizedKey`, `done`,
`processRunning`, `processTicks`, `processTimedout`, `processId`
(program), `thread` (thread), `processTime` (unused). See
[Script API](script-api.md#the-job-record).

### Rules in short

| Topic | Rule |
|-------|------|
| When things run | only inside `process()`; jobs run in parallel, callbacks one at a time in the calling thread |
| End of `process()` | when every job has ended or timed out; about two ticks after the last end |
| Pass | sleep `clockTick`, check running jobs, start new ones |
| Limit | at most `processMaxCount` jobs (programs and threads together) |
| Time limit | in passes; set `clockTick` first, then `setProcessMaxTime*` |
| Order | jobs start in the order added; callbacks in list order within a pass |
| Groups | equal `synchronizedKey` (`==`): never at the same time, in list order |
| Programs | no shell, shared console, no exit code; killed on time out (not their children) |
| Threads | compiled again with empty globals; `fnThis` / `parameters` / result copied; only asked to stop on time out |
| Start failure | a program that cannot start ends normally with `processId` `0` |
| Exceptions | from callbacks leave `process()`; call it again to continue |
| Adding jobs | allowed from callbacks; they run in the same `process()` |

## C++

Namespace `XYO::QuantumScript::Extension::Job`; export macro
`XYO_QUANTUMSCRIPT_EXTENSION_JOB_EXPORT` (empty with
`XYO_QUANTUMSCRIPT_EXTENSION_JOB_LIBRARY`).

| Symbol | Header | Notes |
|--------|--------|-------|
| `registerInternalExtension(Executive *)` | `Job.hpp` | registers `"Job"` as internal; also register `Thread`, `Shell` and `Math` |
| `initExecutive(Executive *, void *extensionId)` | `Job.hpp` | extension init, called by the engine: sets name / info / version, compiles `Library.js` |
| `quantumScriptExtension(Executive *, void *)` | DLL export | `extern "C"`, DLL builds only |
| `Copyright::copyright()`, `License::license()`, `Version::version()`, ... | `Job/Copyright.hpp`, `License.hpp`, `Version.hpp` | library metadata |

| Build project | Kind | Defines |
|---------------|------|---------|
| `quantum-script--job` | DLL / shared library | exports `quantumScriptExtension` |
| `quantum-script--job.static` | static library, static CRT | `XYO_QUANTUMSCRIPT_EXTENSION_JOB_LIBRARY` for users |

| Dependency | Why |
|------------|-----|
| `quantum-script` | the engine |
| `quantum-script--thread` | `Thread.newThread`, `CurrentThread.sleep`, `Processor.getCount` |
| `quantum-script--shell` | `Shell.executeNoWait`, `isProcessTerminated`, `terminateProcess` |
| `quantum-script--math` | `Math.floor` in `setProcessMaxTime` |
| `quantum-script--console`, `--buffer`, `--shellfind` | build dependencies of the project |
| `quantum-script--application` | used by the test host |
