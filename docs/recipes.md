# Recipes

Every recipe below is a complete script, tested with `quantum-script`.
Remember that nothing runs until `process()` is called, that it returns
only when every job is done, and that a thread function cannot see the
variables of the script (see [Job model](job-model.md)).

Some recipes run `sleep.js`, a small stand-in for real work that sleeps the
number of milliseconds given on its command line:

```javascript
// sleep.js
Script.requireExtension("Thread");
Script.requireExtension("Console");
Script.requireExtension("Application");
CurrentThread.sleep(1 * Application.getArgument(1, 1000));
Console.writeLn("slept " + Application.getArgument(1, 1000));
```

## Run commands in parallel, a few at a time

```javascript
Script.requireExtension("Console");
Script.requireExtension("Job");

var commands = [
	"quantum-script sleep.js 300",
	"quantum-script sleep.js 100",
	"quantum-script sleep.js 200"
];

var job = new Job();
job.clockTick = 100;           // check 10 times per second
job.processMaxCount = 2;       // at most 2 at the same time
job.setProcessMaxTimeMinutes(5);

job.onStart = function(process) {
	Console.writeLn("start: " + process.info);
};
job.onEnd = function(process) {
	Console.writeLn("end: " + process.info);
};
job.onTimedout = function(process) {
	Console.writeLn("killed: " + process.info);
};

for (var k = 0; k < commands.length; ++k) {
	job.addProcess(commands[k], commands[k]);
};
job.process();
Console.writeLn("all done");
```

The third command starts as soon as the second ends. Set `clockTick`
before `setProcessMaxTimeMinutes`: the limit is stored in ticks.

## Get the exit code of each command

`addProcess` gives no exit code. Run each command from a thread with
`Shell.system` (it uses the system shell, so pipes and redirection work
too) and return the code: it arrives in `onEnd`.

```javascript
Script.requireExtension("Console");
Script.requireExtension("Job");

var commands = [
	"quantum-script sleep.js 100",
	"quantum-script does-not-exist.js",
	"exit 3"
];

var failed = [];
var job = new Job();
job.clockTick = 100;

job.onEnd = function(process, exitCode) {
	if (exitCode != 0) {
		failed[failed.length] = process.info + " (exit " + exitCode + ")";
	};
};
job.onTimedout = function(process) {
	failed[failed.length] = process.info + " (timed out)";
};

for (var k = 0; k < commands.length; ++k) {
	job.addThread(function(cmd) {
		Script.requireExtension("Shell");
		return Shell.system(cmd);
	}, null, [commands[k]], commands[k]);
};
job.process();

if (failed.length) {
	Console.writeLn("failed:\n  " + failed.join("\n  "));
	Script.setExitCode(1);
};
```

The trade-off: on time out, the thread is only asked to stop, and
`Shell.system` does not check that request, so the command is not killed.
Use `addProcess` when commands must be killed on time out.

## Process files in parallel and collect the results

Pass each file name as a parameter; the thread returns its result, `onEnd`
stores it under the `info` of the job.

```javascript
Script.requireExtension("Console");
Script.requireExtension("Shell");
Script.requireExtension("Job");

var lines = {};
var job = new Job();
job.clockTick = 50;

job.onEnd = function(process, result) {
	lines[process.info] = result;
};

var files = Shell.getFileList("data/*.txt");
for (var k = 0; k < files.length; ++k) {
	job.addThread(function(file) {
		Script.requireExtension("Shell");
		return Shell.fileGetContents(file).length;
	}, null, [files[k]], files[k]);
};
job.process();

for (var file in lines) {
	Console.writeLn(file + ": " + lines[file] + " bytes");
};
```

## Keep jobs that share a resource from overlapping

Give them the same `synchronizedKey`. Jobs with another key, or no key,
still run next to them.

```javascript
Script.requireExtension("Console");
Script.requireExtension("Job");

var job = new Job();
job.clockTick = 50;
job.onStart = function(process) {
	Console.writeLn("start " + process.info);
};

var step = function(ms) {
	Script.requireExtension("Thread");
	CurrentThread.sleep(ms);
};

// the two "release" jobs never overlap, "docs" runs next to them
job.addThread(step, null, [300], "release win64", "release-folder");
job.addThread(step, null, [300], "release win32", "release-folder");
job.addThread(step, null, [100], "docs");
job.process();
```

```
start release win64
start docs
start release win32
```

## Run a follow-up step

`onEnd` may add jobs; they run in the same `process()` call. Here each
build is followed by its test, and the tests of different builds run in
parallel.

```javascript
Script.requireExtension("Console");
Script.requireExtension("Job");

var job = new Job();
job.clockTick = 50;

var build = function(name) {
	Script.requireExtension("Thread");
	CurrentThread.sleep(100);
	return name;
};
var test = function(name) {
	Script.requireExtension("Thread");
	CurrentThread.sleep(50);
	return name + " ok";
};

job.onEnd = function(process, result) {
	Console.writeLn(process.info + ": " + result);
	if (process.info.indexOf("build ") == 0) {
		this.addThread(test, null, [result], "test " + result);
	};
};

job.addThread(build, null, ["alpha"], "build alpha");
job.addThread(build, null, ["beta"], "build beta");
job.process();
```

`build` and `test` are written as variables of the script, but each one is
compiled again in its own thread: they cannot call each other or use other
script variables.

## Let a thread stop on time out

A thread job is only asked to stop. Test `CurrentThread.isRequestToTerminate()`
in its loop.

```javascript
Script.requireExtension("Console");
Script.requireExtension("Job");

var job = new Job();
job.clockTick = 100;
job.setProcessMaxTime(500);

job.onEnd = function(process, result) {
	Console.writeLn(process.info + ": " + result);
};
job.onTimedout = function(process) {
	Console.writeLn(process.info + ": timed out, asked to stop");
};

job.addThread(function() {
	Script.requireExtension("Thread");
	var n = 0;
	while (!CurrentThread.isRequestToTerminate()) {
		++n;                       // one small piece of work
		CurrentThread.sleep(10);
	};
	return n;
}, null, [], "endless");
job.process();
```

## Show progress

Count in `onEnd` and `onTimedout`. One function can serve both, because
`this` is the `Job` in every callback.

```javascript
Script.requireExtension("Console");
Script.requireExtension("Job");

var total = 0;
var done = 0;
var job = new Job();
job.clockTick = 50;

job.onEnd = function(process) {
	++done;
	Console.writeLn("[" + done + "/" + total + "] " + process.info);
};
job.onTimedout = job.onEnd;

for (var k = 1; k <= 4; ++k) {
	job.addThread(function(ms) {
		Script.requireExtension("Thread");
		CurrentThread.sleep(ms);
	}, null, [k * 50], "part " + k);
	++total;
};
job.process();
```

## Decide the command when the job starts

`onStart` runs before the job starts, so it may change `process.cmd` (or
`process.parameters` of a thread job), for example to pick a free output
name or to add options known only at that time.

```javascript
job.onStart = function(process) {
	if (process.info == "fast") {
		process.cmd = "quantum-script sleep.js 200";
	};
};
```

## Run one at a time

`job.processMaxCount = 1;` runs the jobs in list order, one after the
other, with the same callbacks and time limit.
