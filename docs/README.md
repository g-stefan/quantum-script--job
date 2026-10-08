# Quantum Script Extension Job — Documentation

`quantum-script--job` is the **parallel job runner of Quantum Script**. You
load it with `Script.requireExtension("Job")`. It runs a list of jobs
(external programs or script functions in threads) a few at a time, with a
limit on how many run together, a time limit for each job, groups of jobs
that must not overlap, and callbacks when a job starts, ends or times out.
Windows and Linux use the same API.

A build or a batch tool often has many independent steps: compile these
projects, convert these files, run these tests. Running them one after the
other wastes the machine; starting all of them at once overloads it. `Job`
sits between the two:

- **A list.** `new Job()` is an empty list. `addProcess(cmd, ...)` adds a
  program to run, `addThread(fn, this, args, ...)` adds a script function
  to run in its own `Thread`.
- **A limit.** At most `processMaxCount` jobs run at the same time (the
  number of processors by default). The next job starts when one ends.
- **A time limit.** A job that runs longer than the limit (1 minute by
  default) is stopped: a program is killed, a thread is asked to stop.
- **Groups.** Jobs added with the same `synchronizedKey` never run at the
  same time, for example two steps that write to the same folder.
- **Callbacks.** `onStart(job)`, `onEnd(job, result)` and
  `onTimedout(job)` report progress, collect results, and may add new jobs.
- **The loop.** Nothing runs by itself. `process()` runs the list: it
  starts jobs, checks them once per `clockTick` (1 second by default), and
  returns when every job is done.

`process()` blocks the thread that calls it. The jobs themselves run in
parallel, as separate processes or threads; the callbacks run one at a time
in the thread that called `process()`.

```
scripts: fabricare build scripts, quantum-script .js, tools
quantum-script--job        <-- this extension: Job (addProcess, addThread, process)
quantum-script--thread                 (Thread.newThread, CurrentThread.sleep, Processor.getCount)
quantum-script--shell                  (Shell.executeNoWait, isProcessTerminated, terminateProcess)
quantum-script--math                   (Math.floor)
quantum-script                         (Executive, ExecutiveX, Variable)
xyo-system, xyo-multithreading, xyo-encoding, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What the extension gives |
|------|--------------------------|
| Run many commands in parallel, but not all at once | `addProcess(cmd)` for each, `processMaxCount`, `process()` |
| Run script functions in parallel and collect their results | `addThread(fn, this, args)`, the result is the second argument of `onEnd` |
| Stop a step that hangs | `setProcessMaxTime*`: programs are killed, threads asked to stop |
| Keep steps that share a resource from overlapping | the same `synchronizedKey` |
| Report progress, count failures | `onStart`, `onEnd`, `onTimedout`, and the `info` value of each job |
| Run a follow-up step when a step ends | call `this.addProcess` / `this.addThread` from `onEnd` |
| Parallel steps in fabricare build scripts | `Job` is loaded in fabricare scripts already |

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("Job");` | also loads `Thread`, `Shell` and `Math`; fabricare scripts have it already |
| Create a runner | `var job = new Job();` | `new` is required |
| Add a program | `job.addProcess(cmd, info, synchronizedKey)` | started with `Shell.executeNoWait(cmd)`: no shell, no exit code |
| Add a function | `job.addThread(fn, fnThis, parameters, info, synchronizedKey)` | runs in a new `Thread`; returns its result to `onEnd` |
| Limit parallel jobs | `job.processMaxCount = 4;` | default `Processor.getCount()` |
| Check faster | `job.clockTick = 100;` | milliseconds per pass, default `1000`; **then** set the time limit |
| Time limit | `job.setProcessMaxTimeMinutes(10);` | default 60 ticks; counted in ticks of `clockTick` |
| Callbacks | `job.onStart`, `job.onEnd`, `job.onTimedout` | `this` is the `Job`; the argument is the job record |
| Run | `job.process();` | blocks until every job is done |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, a first script, fabricare scripts, register it in a C++ host, static builds |
| [Job model](job-model.md) | How `process()` runs the list: passes, the limit, time limits, groups, order, callbacks, errors, limitations |
| [Script API](script-api.md) | Every property and function: arguments, exact behavior, the job record |
| [Recipes](recipes.md) | Commands in parallel, exit codes, files in parallel, groups, follow-up steps, time limits, progress |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding)
is documented in the `quantum-script` repository, `docs/`. The `Thread` and
`Shell` extensions have their own repositories and documentation.

## Source map

```
source/XYO/QuantumScript.Extension/Job.hpp            umbrella header (Library.hpp)
source/XYO/QuantumScript.Extension/Job.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/Job/
    Dependency.hpp                 <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]             initExecutive (compiles Library.js), registerInternalExtension, DLL entry point
    Library.js                     Job, written in Quantum Script
    Library.Source.cpp             Library.js as a C string (generated by fabricare/make.prepare.js)
    Copyright / License / Version  library metadata
fabricare/make.prepare.js          file-to-cs: Library.js -> Library.Source.cpp
test/test.01.cpp, test.01.js       C++ host with Console, Buffer, Application, Shell, ShellFind, Thread, Math and Job internal
test/test.01.sub.10.js, .sub.20.js the programs test.01.js runs as processes
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--job/`](../.claude/skills/quantum-script--job/SKILL.md).
Claude Code loads it automatically inside this repository. To use it in the
projects that use `Job` (fabricare scripts, Quantum Script tools), copy the
folder to `~/.claude/skills/`.
