# Job model

This page explains how `process()` runs the list. The whole runner is about
200 lines of Quantum Script in
`source/XYO/QuantumScript.Extension/Job/Library.js`; read it when in doubt.

## The list

`new Job()` keeps an array, `processJobList`. `addProcess` and `addThread`
append a **job record** (a plain object, see
[Script API](script-api.md#the-job-record)) and return nothing. Nothing is
started when a job is added. A record is never removed: it is marked
`done` when it ends or times out.

## Passes

`process()` loops in **passes** until every record is `done`. Each pass:

1. **Sleeps `clockTick` milliseconds** (`CurrentThread.sleep`). The sleep
   comes first, so the first job starts one tick after `process()` is
   called, and an empty list still waits one tick.
2. **Checks the running jobs**, in list order. For each running job:
   - adds one to `processTicks`; if it reached `processMaxTicks`, the job
     **times out**: it is marked `done` and `processTimedout`, a program is
     killed with `Shell.terminateProcess(processId)`, a thread is asked to
     stop with `thread.requestToTerminate()`, and `onTimedout(job)` is
     called;
   - otherwise, if it has ended (`Shell.isProcessTerminated(processId)` /
     `thread.isTerminated()`), it is marked `done` and `onEnd(job)` /
     `onEnd(job, thread.getReturnedValue())` is called;
   - otherwise it counts as running.
3. If `processMaxCount` jobs are still running, the pass ends here.
4. **Starts new jobs**, in list order, until `processMaxCount` jobs run:
   for each one, `onStart(job)` is called, then `processTicks` is set to `0`
   and the job is started with `Shell.executeNoWait(cmd)` or
   `Thread.newThread(fn, fnThis, parameters)`.

`process()` returns after a pass in which every record was already `done`
at the start of step 2. So after the last job ends there is one more
sleep: a run takes the time of the jobs plus about two ticks.

## Timing

| Topic | Rule |
|-------|------|
| Check interval | `clockTick` milliseconds (default `1000`) plus the time of the callbacks |
| Start delay | the first jobs start one tick after `process()` is called |
| End noticed | at the next pass: up to one tick after the job really ended |
| Next job starts | in the same pass that noticed an end |
| Time limit | `processMaxTicks` passes after the start (default `60`): with the default tick, 1 minute |
| Short jobs | set `clockTick` to `50` - `100`; with the default 1 second tick, many short jobs spend most of their time waiting |

**The time limit is stored in ticks.** `setProcessMaxTime(ms)` computes
`Math.floor(ms / clockTick)` (at least `1`) with the `clockTick` of that
moment. Changing `clockTick` afterwards changes the real limit: after
`job.clockTick = 100;` alone, the default limit of 60 ticks is 6 seconds,
not 1 minute. Always set `clockTick` first, then call a
`setProcessMaxTime*` function.

The check for the time limit comes before the check for the end, so a job
that ends during its last tick is reported as timed out.

## The limit on parallel jobs

`processMaxCount` (default `Processor.getCount()`, the number of logical
processors) is the most jobs that run at the same time. It counts programs
and threads together. `processMaxCount = 1` runs the jobs one after the
other, in list order.

Each thread job is a thread of the script process; each program job is a
separate process. Choose the limit after what the jobs use: for programs
that are themselves parallel (a compiler using every core), a lower limit
is better.

## Groups: `synchronizedKey`

A job added with a `synchronizedKey` (any value except `undefined`) never
runs at the same time as another job with an equal key (`==`). Jobs with
different keys, and jobs without a key, run in parallel as usual. Jobs of
one group start in list order, one after the other.

When a job of a group waits for its group, it may still take one place of
`processMaxCount` for that pass, so the parallelism can be one lower for a
tick. The jobs still all run.

## Order

- Jobs start in the order they were added, as places become free, skipping
  jobs whose group is busy.
- `onEnd` / `onTimedout` are called in list order within a pass, not in the
  order the jobs really ended.
- The callbacks run in the thread that called `process()`, one at a time,
  never while another callback runs.

## Callbacks

`onStart`, `onEnd` and `onTimedout` are called as methods of the `Job`
(`this` is the `Job`), with the job record as the first argument. `onEnd`
of a thread job gets the value returned by the thread function as the
second argument (`undefined` for a program).

- `onStart(job)` is called **before** the job starts: `job.cmd`,
  `job.fn`, `job.fnThis` and `job.parameters` may still be changed there.
  `job.processId` and `job.thread` are not set yet.
- `onEnd(job, result)` and `onTimedout(job)` are called after the record is
  marked `done`.
- A callback may add jobs (`this.addProcess`, `this.addThread`): the loop
  reads the list length on every pass, so the new jobs run in the same
  `process()` call.

## Errors

| Case | What happens |
|------|--------------|
| A program cannot be started | `executeNoWait` returns `0`; `isProcessTerminated(0)` is `true`, so the job **ends normally** at the next pass (`onEnd`), with `processId` `0` |
| A program fails | `onEnd`, as for success: the exit code is not available |
| A thread function throws | the thread ends; `onEnd(job, undefined)` |
| A thread function uses a variable of the outer script | it is `undefined` in the thread (the function is compiled again there) |
| A callback throws | the exception leaves `process()`. Running jobs keep running; the record of the job being reported is already `done`. Calling `process()` again continues with the rest |
| `Job()` without `new` | throws (`setPropertyBySymbol`) |

## Stopping

- A **program** that times out is closed (Windows: `WM_CLOSE` to its
  windows, then `TerminateProcess` without waiting; Linux: `SIGTERM`, then
  `SIGKILL`). Its own child processes are **not** stopped: a command run
  through `cmd.exe /c` or a launcher may leave children running.
- A **thread** that times out is only **asked** to stop. The thread
  function must test `CurrentThread.isRequestToTerminate()` and return. A
  thread that never tests it keeps running after `onTimedout` (and the
  script waits for it when the `Job` is released).

## Programs: `Shell.executeNoWait`

`addProcess(cmd)` starts `cmd` **without a shell**: the first word is the
program, the rest are its arguments (Linux: split on spaces, double quotes
group, `PATH` searched). Pipes, redirection (`>`), `&&` and shell built-ins
(`exit`, `dir`, `echo` on Windows) do not work; run `cmd /c ...` or
`sh -c "..."`, or use a thread with `Shell.system(cmd)`.

The program shares the console of the script: its output is mixed with the
script output. It is not captured and its exit code is not available.

## Threads: `Thread.newThread`

`addThread(fn, fnThis, parameters)` runs `fn` in a new `Thread` (see the
`quantum-script--thread` documentation):

- `fn` is **compiled again** in the new thread from its source text: it
  cannot use variables of the enclosing scope, and the new thread starts
  with empty globals. Call `Script.requireExtension(...)` inside `fn` for
  every extension it uses (even `Thread`).
- `fnThis` and `parameters` (an Array) are **copied** into the thread.
  `null` / `undefined` as `parameters` means no arguments.
- The returned value is copied back and passed to `onEnd`. Functions inside
  it become `undefined`. To share live data, pass an `Atomic`.

## Reusing a `Job`

`process()` may be called again: records already `done` are skipped, new
records run. Properties and callbacks may be changed between calls. Done
records stay in `processJobList`; for very long runs, create a new `Job`
per batch.

## Limitations

- `process()` blocks; there is no asynchronous mode. Run it in a `Thread`
  if the main script must do other work.
- No exit code, no output capture, no working folder per program, no
  environment per program.
- No priorities and no dependencies between jobs other than groups; build
  sequences from `onEnd` (see [Recipes](recipes.md#run-a-follow-up-step)).
- The time limit is per job and in whole ticks.
- `processTime` in the record is always `0`; the elapsed time is
  `processTicks * clockTick`.
