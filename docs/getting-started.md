# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. First install `quantum-script`
and the extensions it needs to the SDK: `quantum-script--console`,
`quantum-script--buffer`, `quantum-script--thread`, `quantum-script--shell`,
`quantum-script--shellfind` and `quantum-script--math` (and
`quantum-script--application` for the test). Then, from the repository root:

```bash
fabricare make       # build into output/ (make.prepare first turns Library.js into Library.Source.cpp)
fabricare test       # build and run test.01 (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

The build produces two libraries:

| Project                      | Kind                                | Use it when                                         |
|------------------------------|-------------------------------------|-----------------------------------------------------|
| `quantum-script--job`        | DLL / shared library (`dll-or-lib`) | scripts run by `quantum-script`, or a host using the engine DLL |
| `quantum-script--job.static` | static library, static CRT          | self-contained hosts built with `quantum-script.static` |

After `fabricare install`, the SDK `bin` folder holds
`quantum-script--job.dll` (Windows) / `libquantum-script--job.so` (Linux)
next to `quantum-script.exe`. `Script.requireExtension("Job")` finds it
there.

## 2. A first script

```javascript
Script.requireExtension("Console");
Script.requireExtension("Job");

var job = new Job();
job.clockTick = 100;          // check the jobs 10 times per second
job.processMaxCount = 2;      // at most 2 jobs at the same time
job.setProcessMaxTime(5000);  // stop a job after 5 seconds (set it after clockTick)

job.onStart = function(process) {
	Console.writeLn("start " + process.info);
};
job.onEnd = function(process, result) {
	Console.writeLn("end   " + process.info + " -> " + result);
};
job.onTimedout = function(process) {
	Console.writeLn("timed out " + process.info);
};

var square = function(x) {
	Script.requireExtension("Thread");  // a new thread starts with empty globals
	CurrentThread.sleep(200);           // the real work
	return x * x;
};

for (var k = 1; k <= 4; ++k) {
	job.addThread(square, null, [k], "square " + k);
};
job.addProcess("quantum-script --version", "version");

job.process(); // runs everything; returns when every job is done
Console.writeLn("done");
```

Run it with:

```bash
quantum-script first-job.js
```

Two jobs run at a time; each `end` line is printed when `process()` notices
the end, at the next tick. The `--version` program writes to the same
console as the script.

Things to know from the start:

- **Nothing runs until you call `process()`**, and `process()` returns only
  when every job has ended or timed out.
- **A thread function cannot see the variables of the script that adds
  it.** It is compiled again in a new thread with empty globals. Pass data
  with `parameters` (copied), call `Script.requireExtension` inside it, and
  `return` the result.
- **`clockTick` is the time between checks, 1000 ms by default.** Each job
  end is noticed up to one tick late. For short jobs set a smaller tick,
  then set the time limit again (the limit is stored in ticks).
- **A program has no exit code and no captured output.** To get the exit
  code, run the command from a thread with `Shell.system(cmd)` (see
  [Recipes](recipes.md#get-the-exit-code-of-each-command)).

`Script.requireExtension("Job")` loads the extension once (a second call
does nothing). Its script library itself calls
`Script.requireExtension("Thread")`, `("Shell")` and `("Math")`, so
`Thread`, `CurrentThread`, `Atomic`, `Processor`, `Shell` and `Math` are
available afterwards too. A missing extension throws `Unable to open "Job"`
(or the name of the missing dependency).

## 3. fabricare build scripts

`fabricare` registers `Job` as an internal extension and its library
already calls `Script.requireExtension("Job")`, so build scripts can use
`new Job()` directly:

```javascript
// in a fabricare script: check every source file, two at a time
var job = new Job();
job.processMaxCount = 2;
job.setProcessMaxTimeMinutes(30);
job.onEnd = function(process) {
	messageAction("checked " + process.info);
};
var files = Shell.getFileList("source/*.js");
for (var k = 0; k < files.length; ++k) {
	job.addProcess("quantum-script tools/check.js " + files[k], files[k]);
};
job.process();
```

## 4. Register it in a C++ host

A host that embeds Quantum Script registers the extension in its init
callback. The Job library requires `Thread`, `Shell` and `Math`, so register
those too (or ship their DLLs next to the host):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Shell.hpp>
#include <XYO/QuantumScript.Extension/Thread.hpp>
#include <XYO/QuantumScript.Extension/Math.hpp>
#include <XYO/QuantumScript.Extension/Job.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Shell::registerInternalExtension(executive);
	Extension::Thread::registerInternalExtension(executive);
	Extension::Math::registerInternalExtension(executive);
	Extension::Job::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeFile("script.js")) {
			printf("%s\n%s", ExecutiveX::getError().value(), ExecutiveX::getStackTrace().value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

Registering only makes the extension *available*. Scripts still call
`Script.requireExtension("Job")`. See `test/test.01.cpp`.

There is no C++ API beyond registration: `Job` is plain Quantum Script,
compiled into the executive by `initExecutive`.

In `fabricare.json`, depend on `quantum-script--job`:

```json
{
	"name" : "my-host",
	"make" : "exe",
	"dependency" : [
		"quantum-script--job"
	]
}
```

## 5. Static builds

Link `quantum-script--job.static` (it depends on `quantum-script.static`
and the `.static` projects of `console`, `buffer`, `thread`, `shell`,
`shellfind` and `math`). The static project defines
`XYO_QUANTUMSCRIPT_EXTENSION_JOB_LIBRARY`, which:

- makes `XYO_QUANTUMSCRIPT_EXTENSION_JOB_EXPORT` empty, and
- leaves out the `quantumScriptExtension` DLL entry point.

So in a static host the extension must be registered with
`Extension::Job::registerInternalExtension(executive)`, together with
`Thread`, `Shell` and `Math`; there is no library to load.
