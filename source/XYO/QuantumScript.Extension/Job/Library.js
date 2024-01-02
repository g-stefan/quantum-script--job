// Quantum Script Extension Job
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

Script.requireExtension("Thread");
Script.requireExtension("Shell");
Script.requireExtension("Math");

function Job() {
	this.processMaxCount = Processor.getCount();
	this.processMaxTicks = 60 * 1; // 1 minute
	this.processJobList = [];

	this.onStart = function(process) {};
	this.onEnd = function(process) {};
	this.onTimedout = function(process) {};

	this.clockTick = 1000; // 1 second

	this.setProcessMaxTime = function(miliSeconds) {
		this.processMaxTicks = Math.floor(miliSeconds / this.clockTick);
		if(this.processMaxTicks <= 0) {
			this.processMaxTicks = 1;
		};
	};

	this.setProcessMaxTimeSeconds = function(seconds) {
		this.setProcessMaxTime(seconds * 1000);
	};

	this.setProcessMaxTimeMinutes = function(minutes) {
		this.setProcessMaxTimeSeconds(minutes * 60);
	};

	this.addProcess = function(cmd, info, synchronizedKey) {
		this.processJobList[this.processJobList.length] = {
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

	this.addThread = function(fn, fnThis, parameters, info, synchronizedKey) {
		this.processJobList[this.processJobList.length] = {
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


	this.process = function() {
		var allDone;
		var count;
		var list;
		var syncronizedList;
		var k, m;

		allDone = false;

		while(!allDone) {
			CurrentThread.sleep(this.clockTick);
			count = 0;
			allDone = true;
			syncronizedList = [];
			// check running
			for(k = 0; k < this.processJobList.length; ++k) {
				if(!this.processJobList[k].done) {
					allDone = false;
					if(this.processJobList[k].processRunning) {
						this.processJobList[k].processTicks++;
						if(this.processJobList[k].processTicks >= this.processMaxTicks) {
							this.processJobList[k].done = true;
							this.processJobList[k].processTimedout = true;
							if(this.processJobList[k].isThread) {
								this.processJobList[k].thread.requestToTerminate();
							} else {
								Shell.terminateProcess(this.processJobList[k].processId);
							};
							this.processJobList[k].processRunning = false;
							this.onTimedout(this.processJobList[k]);
							continue;
						};

						if(this.processJobList[k].isThread) {

							if(this.processJobList[k].thread.isTerminated()) {
								this.processJobList[k].done = true;
								this.processJobList[k].processRunning = false;
								this.onEnd(this.processJobList[k], this.processJobList[k].thread.getReturnedValue());
								continue;
							};

						} else {

							if(Shell.isProcessTerminated(this.processJobList[k].processId)) {
								this.processJobList[k].done = true;
								this.processJobList[k].processRunning = false;
								this.onEnd(this.processJobList[k]);
								continue;
							};

						};

						if(this.processJobList[k].synchronizedJob) {
							syncronizedList[syncronizedList.length] = this.processJobList[k];
						};

						count++;
						if(count == this.processMaxCount) {
							break;
						};
					};
					continue;
				};
			};
			if(count == this.processMaxCount) {
				continue;
			};
			// run new processes
			list = [];
			for(k = 0; k < this.processJobList.length; ++k) {
				if(!this.processJobList[k].done) {
					if(!this.processJobList[k].processRunning) {
						if(this.processJobList[k].synchronizedJob) {
							for(m = 0; m < syncronizedList.length; ++m) {
								if(syncronizedList[m].synchronizedKey == this.processJobList[k].synchronizedKey) {
									break;
								};
							};
							if(m < syncronizedList.length) {
								continue;
							};
						};
						list[list.length] = this.processJobList[k];
						count++;
						if(count == this.processMaxCount) {
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

				this.onStart(list[k]);

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

