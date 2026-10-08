# Quantum Script Extension Job

Quantum Script extension
- Runs a list of jobs in parallel, a few at a time: programs
(`addProcess`) and script functions in threads (`addThread`).
- A limit on how many jobs run together (`processMaxCount`, the number of
processors by default) and a time limit per job: programs are killed,
threads asked to stop.
- Groups: jobs with the same `synchronizedKey` never run at the same time.
- Callbacks `onStart`, `onEnd` (with the value returned by a thread) and
`onTimedout`; `onEnd` may add follow-up jobs.
- `process()` runs the list and returns when every job is done.
- Windows and Linux, the same API.

```javascript
Script.requireExtension("Job");

Job();
this.onStart(process);
this.onEnd(process,result);
this.onTimedout(process);
this.processMaxCount;
this.processMaxTicks;
this.clockTick;
this.setProcessMaxTime(miliSeconds);
this.setProcessMaxTimeSeconds(seconds);
this.setProcessMaxTimeMinutes(minutes);
this.addProcess(cmd,info,synchronizedKey);
this.addThread(fn,fnThis,parameters,info,synchronizedKey);
this.process();
```

`Job` must be called with `new`: `var job = new Job();`. Set `clockTick`
before the time limit: the limit is stored in ticks.

Built on `quantum-script`, `quantum-script--thread`,
`quantum-script--shell` and `quantum-script--math`, part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, a first script, fabricare scripts, register in a C++ host, static builds
- [Job model](docs/job-model.md) - how `process()` runs the list: passes, limits, time limits, groups, callbacks, errors, limitations
- [Script API](docs/script-api.md) - every property and function, the job record
- [Recipes](docs/recipes.md) - commands in parallel, exit codes, files in parallel, groups, follow-up steps, time limits, progress
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--job](.claude/skills/quantum-script--job/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
