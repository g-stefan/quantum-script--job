//
// Quantum Script Extension Job
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

Script.requireExtension("Thread");
Script.requireExtension("Shell");
Script.requireExtension("Math");

function Job() {
	.processMaxCount = Processor.getCount();
	.processMaxTicks = 60 * 1; // 1 minute
	.processJobList = [];

	.onStart = function(process) {};
	.onEnd = function(process) {};
	.onTimedout = function(process) {};

	.clockTick = 1000; // 1 second

	.setProcessMaxTime = function(miliSeconds) {
		.processMaxTicks = Math.floor(miliSeconds / .clockTick);
		if(.processMaxTicks <= 0) {
			.processMaxTicks = 1;
		};
	};

	.setProcessMaxTimeSeconds = function(seconds) {
		.setProcessMaxTime(seconds * 1000);
	};

	.setProcessMaxTimeMinutes = function(minutes) {
		.setProcessMaxTimeSeconds(minutes * 60);
	};

	.addProcess = function(cmd, info, synchronizedKey) {
		.processJobList[.processJobList.length] = {
			cmd: cmd,
			done: false,
			processRunning: false,
			processId:  0,
			processTime: 0,
			processTimedout: false,
			info: info,
			synchronizedJob: !Script.isUndefined(synchronizedKey),
			synchronizedKey: synchronizedKey,
			isThread: false,
			thread: {}
		};
	};

	.addThread = function(fn, fnThis, parameters, info, synchronizedKey) {
		.processJobList[.processJobList.length] = {
			fn: fn,
			fnThis: fnThis,
			parameters: parameters,
			done: false,
			processRunning: false,
			processId:  0,
			processTime: 0,
			processTimedout: false,
			info: info,
			synchronizedJob: !Script.isUndefined(synchronizedKey),
			synchronizedKey: synchronizedKey,
			isThread: true,
			thread: {}
		};
	};


	.process = function() {
		var allDone;
		var count;
		var list;
		var syncronizedList;
		var k, m;

		allDone = false;

		while(!allDone) {
			CurrentThread.sleep(.clockTick);
			count = 0;
			allDone = true;
			syncronizedList = [];
			// check running
			for(k = 0; k < .processJobList.length; ++k) {
				if(!.processJobList[k].done) {
					allDone = false;
					if(.processJobList[k].processRunning) {
						.processJobList[k].processTicks++;
						if(.processJobList[k].processTicks >= .processMaxTicks) {
							.processJobList[k].done = true;
							.processJobList[k].processTimedout = true;
							if(.processJobList[k].isThread) {
								.processJobList[k].thread.requestToTerminate();
							} else {
								Shell.terminateProcess(.processJobList[k].processId);
							};
							.processJobList[k].processRunning = false;
							.onTimedout(.processJobList[k]);
							continue;
						};

						if(.processJobList[k].isThread) {

							if(.processJobList[k].thread.isTerminated()) {
								.processJobList[k].done = true;
								.processJobList[k].processRunning = false;
								.onEnd(.processJobList[k], .processJobList[k].thread.getReturnedValue());
								continue;
							};

						} else {

							if(Shell.isProcessTerminated(.processJobList[k].processId)) {
								.processJobList[k].done = true;
								.processJobList[k].processRunning = false;
								.onEnd(.processJobList[k]);
								continue;
							};

						};

						if(.processJobList[k].synchronizedJob) {
							syncronizedList[syncronizedList.length] = .processJobList[k];
						};

						count++;
						if(count == .processMaxCount) {
							break;
						};
					};
					continue;
				};
			};
			if(count == .processMaxCount) {
				continue;
			};
			// run new processes
			list = [];
			for(k = 0; k < .processJobList.length; ++k) {
				if(!.processJobList[k].done) {
					if(!.processJobList[k].processRunning) {
						if(.processJobList[k].synchronizedJob) {
							for(m = 0; m < syncronizedList.length; ++m) {
								if(syncronizedList[m].synchronizedKey == .processJobList[k].synchronizedKey) {
									break;
								};
							};
							if(m < syncronizedList.length) {
								continue;
							};
						};
						list[list.length] = .processJobList[k];
						count++;
						if(count == .processMaxCount) {
							break;
						};
					};
				};
			};
			for(k = 0; k < list.length; ++k) {
				if(list[k].synchronizedJob) {
					for(m = 0; m < list.length; ++m) {
						if(list[m].processRunning) {
							if(list[m].synchronizedKey == list[k].synchronizedKey) {
								break;
							};
						};
					};
					if(m < list.length) {
						continue;
					};
				};

				.onStart(list[k]);

				list[k].processTicks = 0;
				list[k].processTimedout = false;
				list[k].processRunning = true;

				if(list[k].isThread) {
					list[k].thread = Thread.newThread(list[k].fn, list[k].fnThis, list[k].parameters);
				} else {
					list[k].processId = Shell.executeNoWait(list[k].cmd);
				};
			};
		};

	};
};

