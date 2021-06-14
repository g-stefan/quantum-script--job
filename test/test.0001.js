// Public domain
// http://unlicense.org/

Script.requireExtension("Application");
Script.requireExtension("Console");
Script.requireExtension("Thread");
Script.requireExtension("Job");

var job = new Job();

job.onStart = function(process) {
	Console.writeLn("START: " + process.info);
};

job.onEnd = function(process) {
	Console.writeLn("END: " + process.info);
};

job.onTimedout = function(process) {
	Console.writeLn("TIMEDOUT: " + process.info);
};

job.addProcess(Application.getExecutable() + " test/test.0001.sub.10.js", "10");
job.addProcess(Application.getExecutable() + " test/test.0001.sub.20.js", "20");
job.addProcess(Application.getExecutable() + " test/test.0001.sub.10.js", "30-sync", "30");
job.addProcess(Application.getExecutable() + " test/test.0001.sub.10.js", "31-sync", "30");
job.addProcess(Application.getExecutable() + " test/test.0001.sub.10.js", "40-sync", "40");
job.addProcess(Application.getExecutable() + " test/test.0001.sub.10.js", "41-sync", "40");

job.addThread(function() {
	Script.requireExtension("Thread");
	CurrentThread.sleep(10 * 100);
}, null, null, "50");

job.addThread(function() {
	Script.requireExtension("Thread");
	CurrentThread.sleep(20 * 100);
}, null, null, "60");

job.addThread(function() {
	Script.requireExtension("Thread");
	CurrentThread.sleep(10 * 100);
}, null, null, "50-sync", "50");

job.addThread(function() {
	Script.requireExtension("Thread");
	CurrentThread.sleep(10 * 100);
}, null, null, "51-sync", "50");

job.process();

Console.writeLn("-> test 0001 ok");
