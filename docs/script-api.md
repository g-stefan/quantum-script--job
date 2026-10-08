# Script API

All symbols below are available after `Script.requireExtension("Job")`.
Loading `Job` also loads `Thread`, `Shell` and `Math`.

## `new Job()`

Creates an empty job runner. `new` is required: `Job()` alone throws
(`setPropertyBySymbol`). Each `Job` is independent; several may exist, but
`process()` runs only its own list.

## Properties

| Property | Default | Meaning |
|----------|---------|---------|
| `processMaxCount` | `Processor.getCount()` | most jobs running at the same time (programs and threads together) |
| `clockTick` | `1000` | milliseconds slept at the start of each pass of `process()` |
| `processMaxTicks` | `60` | time limit of each job, in passes; set it with the `setProcessMaxTime*` functions |
| `processJobList` | `[]` | the job records, in the order they were added |
| `onStart` | empty function | called before a job starts |
| `onEnd` | empty function | called when a job has ended |
| `onTimedout` | empty function | called when a job has been stopped for running too long |

All of them are plain properties: assign them directly
(`job.processMaxCount = 4;`, `job.onEnd = function(process, result) {...};`).

## Time limit

### `job.setProcessMaxTime(milliSeconds)`

Sets `processMaxTicks = Math.floor(milliSeconds / clockTick)`, at least
`1`. It uses the **current** `clockTick`: set `clockTick` first. Returns
`undefined`.

### `job.setProcessMaxTimeSeconds(seconds)`

`setProcessMaxTime(seconds * 1000)`.

### `job.setProcessMaxTimeMinutes(minutes)`

`setProcessMaxTimeSeconds(minutes * 60)`.

The limit applies to every job of this `Job`, counted from the pass that
started it. See [Job model](job-model.md#timing).

## Adding jobs

### `job.addProcess(cmd, info, synchronizedKey)`

Appends a program job. Returns `undefined`.

- `cmd`: the command line, started with `Shell.executeNoWait(cmd)` (no
  shell; see [Job model](job-model.md#programs-shellexecutenowait)). It is
  read when the job starts, so `onStart` may change `process.cmd`.
- `info`: any value, kept in the record as `info` for your callbacks (a
  name, an object with details). Optional.
- `synchronizedKey`: optional. Jobs with equal keys never run at the same
  time. `undefined` (or missing) means no group.

### `job.addThread(fn, fnThis, parameters, info, synchronizedKey)`

Appends a thread job. Returns `undefined`.

- `fn`: a script function, run with `Thread.newThread(fn, fnThis,
  parameters)` in a new thread with empty globals. It cannot use variables
  of the enclosing scope. Its returned value is passed to `onEnd`.
- `fnThis`: `this` inside `fn`, copied into the thread. `null` for none.
- `parameters`: an Array of arguments, copied into the thread. `null` or
  `[]` for none.
- `info`, `synchronizedKey`: as for `addProcess`.

Jobs may be added before `process()` or from a callback during
`process()`.

## Running

### `job.process()`

Runs passes until every job in the list is done (ended or timed out), then
returns `undefined`. Blocks the calling thread. Exceptions thrown by the
callbacks leave `process()`; calling it again continues with the jobs that
are not done. See [Job model](job-model.md#passes).

## Callbacks

Each callback is called with `this` set to the `Job`. Replace the defaults
by assignment. The return value is ignored.

### `job.onStart(process)`

Called before a job is started, in the pass that starts it. The job may be
changed here (`process.cmd`, `process.parameters`, ...). `processId` and
`thread` are not set yet.

### `job.onEnd(process, result)`

Called in the first pass that sees the job has ended. `process.done` is
`true`. `result` is the value returned by the thread function (a copy), or
`undefined` for a program or a thread that threw.

### `job.onTimedout(process)`

Called when a job reached `processMaxTicks`. The program has been killed or
the thread asked to stop (`requestToTerminate()`). `process.done` and
`process.processTimedout` are `true`. `onEnd` is **not** called for this
job.

## The job record

The object passed to the callbacks and stored in `processJobList`:

| Field | Program | Thread | Meaning |
|-------|---------|--------|---------|
| `info` | yes | yes | the `info` argument |
| `cmd` | yes | - | the command line |
| `fn`, `fnThis`, `parameters` | - | yes | the `addThread` arguments |
| `isThread` | `false` | `true` | the kind of job |
| `synchronizedJob` | yes | yes | `true` if a `synchronizedKey` was given |
| `synchronizedKey` | yes | yes | the key, or `undefined` |
| `done` | yes | yes | `true` once ended or timed out |
| `processRunning` | yes | yes | `true` while running |
| `processTicks` | yes | yes | passes since the start (set to `0` at the start) |
| `processTimedout` | yes | yes | `true` if it was stopped by the time limit |
| `processId` | yes | `0` | the process id from `Shell.executeNoWait`, `0` if it could not start |
| `thread` | `{}` | yes | the `Thread` object once started |
| `processTime` | `0` | `0` | not used |

You may add your own fields to a record in a callback (it is a normal
object). Do not change `done`, `processRunning` or `processTicks`.
